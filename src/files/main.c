#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <unistd.h>

#include "fifo.h"
#include "sockets.h"
#include "storage.h"
#include "json/json-maker.h"

#define LEN_SMALL_UPDATE 25
#define LEN_BIG_UPDATE 320

pthread_t recv_socket_thread_id;
pthread_t trans_socket_thread_id;

int response_ready = 0;
char response_buffer[RES_SIZE];



void prepare_files_json();
void prepare_streams_json();

double total_read_perf = 11.1;//TODO
double total_write_perf = 12.3;//TODO

void init()
{
    init_big_arrays();
    
    pthread_create(&recv_socket_thread_id, NULL, recv_socket_func, NULL);
    pthread_create(&trans_socket_thread_id, NULL, trans_socket_func, NULL);
    
    init_fifo(&requests_fifo);
    init_fifo(&updates_fifo);
    
    get_files_from_hard_drive();
    get_streams_status_from_storage();
}

#define add_small_update(str) add_update(str, LEN_SMALL_UPDATE)

void add_update(const char *str, int len)
{
    void *update = malloc(len + 1);
    if (update == NULL)
    {
        perror("malloc: ");
        exit(EXIT_FAILURE);
    }
    
    memcpy((char *)update, str, len + 1);
    if (fifo_enq(&updates_fifo, update) == failed)
    {
        printf("error: fifo_enq failed\n");
        free(update);
        perror("fifo_enq");
        exit(EXIT_FAILURE);
    }
}

int main()
{
    char buffer[LEN_BIG_UPDATE];
    
    init();
    
    while (1)
    {
        void *ptr = fifo_deq(&requests_fifo);
        int file_index;
        int stream_index;
        if (ptr != NULL)
        {
            message *msg_p = (message *)ptr;
            //printf("main: \nmagic: %s\ncounter: %d\ntype: %d\nsize: %d\ndata: %s\n\n", 
            //        msg_p->magic, msg_p->counter, msg_p->command_type, msg_p->data_size, msg_p->data);
            switch (msg_p->command_type)
            {
                case get_device_status:
                    
                    while (response_ready);
                    
                    add_small_update("get_device_status done.");
                    
                    prepare_streams_json();
                    response_ready = 1;
                    
                    break;
                
                case get_files_status:
                    
                    while (response_ready);
                    
                    add_small_update("get_files_status done.");
                    
                    prepare_files_json();
                    response_ready = 1;
                    
                    break;
                
                case stop_all:
                    
                    while (response_ready);
                    
                    add_small_update("stop_all done.");
                    
                    memset(response_buffer, 0, RES_SIZE);
                    response_ready = 1;
                    break;
                
                case upload_file_to_stream:
                    
                    if (msg_p->data_size <= 0)
                    {
                        add_update("ERROR: upload_file can't identify file without name.", 40);
                        break;
                    }
                    
                    file_index = get_file_index_by_name(msg_p->data);
                    if (file_index == -1)
                    {
                        memset(response_buffer, 0, LEN_BIG_UPDATE);
                        sprintf(buffer, "ERROR: can't upload_file, %s not found.", msg_p->data);
                        add_update(buffer, LEN_BIG_UPDATE);
                        break;
                    }
                    
                    stream_index = get_free_stream_from_storage();
                    if (stream_index == -1)
                    {
                        add_update("ERROR: can't upload_file, too many used streams.", 48);
                        break;
                    }
                    
                    memset(response_buffer, 0, LEN_BIG_UPDATE);
                    sprintf(buffer, "upload_file %s started.", msg_p->data);
                    add_update(buffer, LEN_BIG_UPDATE);
                    
                    upload_file_to_stream_in_storage(file_index, stream_index);
                    
                    memset(response_buffer, 0, LEN_BIG_UPDATE);
                    sprintf(buffer, "upload_file %s done.", msg_p->data);
                    add_update(buffer, LEN_BIG_UPDATE);
                    
                    break;
                
                case download_file_from_stream:
                    
                    if (msg_p->data_size <= 0)
                    {
                        add_update("ERROR: download_file can't identify file without name.", 40);
                        break;
                    }
                    
                    stream_index = get_stream_index_by_name(msg_p->data);
                    if (file_index == -1)
                    {
                        memset(response_buffer, 0, LEN_BIG_UPDATE);
                        sprintf(buffer, "ERROR: can't download_file, stream %s not found.", msg_p->data);
                        add_update(buffer, LEN_BIG_UPDATE);
                        break;
                    }
                    
                    memset(response_buffer, 0, LEN_BIG_UPDATE);
                    sprintf(buffer, "download_file %s started.", msg_p->data);
                    add_update(buffer, LEN_BIG_UPDATE);
                    
                    download_file_from_stream_in_storage(stream_index);
                    
                    memset(response_buffer, 0, LEN_BIG_UPDATE);
                    sprintf(buffer, "download_file %s done.", msg_p->data);
                    add_update(buffer, LEN_BIG_UPDATE);
                    
                    break;
                
                case delete_stream:
                    if (msg_p->data_size <= 0)
                    {
                        add_update("ERROR: can't delete_stream without name.", 40);
                        break;
                    }
                    
                    file_index = get_stream_index_by_name(msg_p->data);
                    if (file_index == -1)
                    {
                        memset(response_buffer, 0, LEN_BIG_UPDATE);
                        sprintf(buffer, "ERROR: can't delete_stream, %s not found.", msg_p->data);
                        add_update(buffer, LEN_BIG_UPDATE);
                        break;
                    }
                    memset(response_buffer, 0, LEN_BIG_UPDATE);
                    sprintf(buffer, "delete_stream %s started.", msg_p->data);
                    add_update(buffer, LEN_BIG_UPDATE);
                    delete_stream_from_storage(file_index);
                    memset(response_buffer, 0, LEN_BIG_UPDATE);
                    sprintf(buffer, "delete_stream %s finished.", msg_p->data);
                    add_update(buffer, LEN_BIG_UPDATE);
                    break;
                
                case format:
                    
                    add_small_update("format started.");
                    format_storage();
                    add_small_update("format done.");
                    
                    break;
                
                default:
                    printf("%s %d.\n", __FUNCTION__, __LINE__);
                    printf("main: \nmagic: %s\ncounter: %d\ntype: %d\nsize: %d\ndata: %s\n\n", 
                                msg_p->magic, msg_p->counter, msg_p->command_type, msg_p->data_size, msg_p->data);
                    break;
            }
            
            free(ptr);
        }
    }
    return 0;
}

void prepare_files_json()
{
    memset(response_buffer, 0, RES_SIZE);
    size_t rem_len = RES_SIZE;
    char *p = response_buffer;
    
    p = json_objOpen( p, NULL, &rem_len );
    
    for (int i = 0; i < files_array_size; i++)
    {
        p = json_float(p, files_array[i].name, files_array[i].size, &rem_len);
    }
    
    p = json_objClose(p, &rem_len );
    p = json_end(p, &rem_len );
    return;
}


void prepare_streams_json()
{
    memset(response_buffer, 0, RES_SIZE);
    size_t rem_len = RES_SIZE;
    char *p = response_buffer;
    
    p = json_objOpen( p, NULL, &rem_len );
    
    p = json_objOpen( p, "device", &rem_len );
    p = json_float(p, "used_size", used_storage, &rem_len);
    p = json_float(p, "total_read_perf", total_read_perf, &rem_len);
    p = json_float(p, "total_write_perf", total_write_perf, &rem_len);
    
    p = json_arrOpen(p, "terminal", &rem_len);
    while (1)
    {
        char *str = (char *)fifo_deq(&updates_fifo);
        if (str == NULL)
            break;
        
        p = json_str(p, NULL, str, &rem_len);
        free(str);
    }
    p = json_arrClose(p, &rem_len );//terminal close
    p = json_objClose(p, &rem_len );//device close
    
    p = json_objOpen( p, "streams", &rem_len );
    for (int i = 0; i < NUM_STREAMS; i++)
    {
        if (streams_array[i].stream_state != unused)
        {
            p = json_objOpen( p, streams_array[i].name, &rem_len );
            p = json_int( p, "ID", i, &rem_len ); 
            p = json_str( p, "state", stream_state_string[streams_array[i].stream_state], &rem_len );
            p = json_float(p, "size", streams_array[i].size, &rem_len);
            p = json_float(p, "read_perf", streams_array[i].read_perf, &rem_len);
            p = json_float(p, "write_perf", streams_array[i].write_perf, &rem_len);
            p = json_objClose(p, &rem_len );
        }
    }
    p = json_objClose(p, &rem_len );//streams close
    
    p = json_objClose(p, &rem_len );//final close
    p = json_end(p, &rem_len );
    
    return;
}

/*
static cpl_error fdir_full_report(packet_t* packet)
{

    short i;
    rc_err_id ret;
    char* p;
    size_t rem_len;
    clock_t start = clock();
    unsigned int number_of_global_faults = 0;



    p = packet->rcp.l4.data;
    rem_len = MAX_DATA_SIZE - 1;

    p = json_objOpen( p, NULL, &rem_len );

    p = json_arrOpen(p, "scrubbers", &rem_len);
    for (i = 0; i < FDIR_NUM_SCRUBBERS; i++)
    {
        p = json_objOpen( p, NULL, &rem_len );
        p = json_str(p, "name",SCRUB_MEM[i], &rem_len);
        p = json_uint(p, "no_of_scrubs", fdir_stat.scrubbers_stat[i].loops, &rem_len);
        p = json_uint(p, "corrected_errors", fdir_stat.scrubbers_stat[i].corrected_errors, &rem_len);
        p = json_objClose(p, &rem_len );
    }
    p = json_arrClose(p, &rem_len );

    p = fdir_spf_report(p, &rem_len );
    p = fdir_spw_report(p, &rem_len );
    p = fdir_fpga_spw_report(p, &rem_len );
    p = fdir_fpga_spf_report(p, &rem_len );

    p = json_objOpen( p, "fpga_voltage", &rem_len );
    p = fdir_fpga_report(TVS_MEAS_1_1V_INT, p, &rem_len);
    p = fdir_fpga_report(TVS_MEAS_1_18V_INT, p, &rem_len);
    p = fdir_fpga_report(TVS_MEAS_2_25_VOLT_INT, p, &rem_len);
    p = json_objClose(p, &rem_len );

    p = json_objOpen( p, "fpga_temperature", &rem_len );
    p = fdir_fpga_report(SYS_CTRL_TEMP_PROCESS_OUT_2, p, &rem_len);
    p = fdir_fpga_report(TVS_MEAS_2_TEMPERATURE_INT, p, &rem_len);
    p = fdir_fpga_report(TEMP_TH_0, p, &rem_len);
    p = fdir_fpga_report(TEMP_TH_1, p, &rem_len);
    p = fdir_fpga_report(TEMP_TH_2, p, &rem_len);
    p = fdir_fpga_report(TEMP_TH_3, p, &rem_len);

    p = fdir_fpga_report(ALERT_1_STAT_COUNT, p, &rem_len);
    p = fdir_fpga_report(ALERT_1_STAT_MAX, p, &rem_len);
    p = fdir_fpga_report(ALERT_2_STAT_COUNT, p, &rem_len);
    p = fdir_fpga_report(ALERT_2_STAT_MAX, p, &rem_len);
    p = fdir_fpga_report(ALERT_OH_STAT_COUNT, p, &rem_len);
    p = fdir_fpga_report(ALERT_OH_STAT_MAX, p, &rem_len);
    p = json_objClose(p, &rem_len );

    p = json_objOpen( p, "sw_update", &rem_len );
    p = json_str(p, "state",SW_UPDATE_STATE[app_sw_update_get_state()], &rem_len);
    p = json_objClose(p, &rem_len );

    p = fdir_boot_flash_report(p, &rem_len);

    p = json_objOpen( p, "RS3", &rem_len );
    p = json_uint(p, "data_plane_dynamic_quality",get_dynamic_quality(), &rem_len);
    p = json_uint(p, "data_plane_static_quality",get_static_quality(), &rem_len);
    p = json_uint(p, "rx_header_crc_error",fdir_spw_get_header_crc_error(), &rem_len);
    p = json_uint(p, "rx_payload_crc_error",fdir_spw_get_payload_crc_error(), &rem_len);
    p = json_objClose(p, &rem_len );

    for (i = 0; i < FDIR_NUM_GLOBAL_TIMERS; i++)
    {
        number_of_global_faults += fdir_stat.global_timer_stat[i].number_of_faults;
    }
    p = json_uint(p, "fdir_global", number_of_global_faults, &rem_len);

    p = json_objClose(p, &rem_len );
    p = json_end(p, &rem_len );

    packet->rcp.l3.packet_length = L3_L4_SIZE + (MAX_DATA_SIZE - rem_len);

    rc_mem_log(FDIR_APP_MODULE,0,"%s() len: %d time %d\n", __FUNCTION__, (MAX_DATA_SIZE - rem_len), (int)(clock() - start));


    //sent to SPW
    ret = tx_manager_send_RCP_packet(packet);
    if(CPL_OK != ret)
    {
        rc_mem_log(FDIR_APP_MODULE,LOG_LEVEL_ERROR,"%s():%d: Error ret=%d\n",__FUNCTION__,__LINE__,ret);
        return CPL_ERROR;
    }

    return CPL_OK;
}
*/













