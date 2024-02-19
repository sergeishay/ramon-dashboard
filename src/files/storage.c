#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "storage.h"

void init_big_arrays()
{
    for (int i = 0; i < MAX_FILES_IN_HARD_DRIVE; i++)
    {
        files_array[i].size = 0;
        memset(files_array[i].name, 0, MAX_FILE_NAME_LENGTH);
    }
    
    for (int i = 0; i < NUM_STREAMS; i++)
    {
        streams_array[i].stream_state = unused;
        streams_array[i].size = 0;
        memset(streams_array[i].name, 0, MAX_FILE_NAME_LENGTH);
        streams_array[i].read_perf = 0;
        streams_array[i].write_perf = 0;
    }
}

/*------------------
-----HARD DRIVE-----
------------------*/

file_in_hard_drive files_array[MAX_FILES_IN_HARD_DRIVE];
int files_array_size = 0;

#define NUM_MOCK_FILES 31
void get_files_from_hard_drive()
{//MOCK
    //files_array[MAX_FILES_IN_HARD_DRIVE];
    char arr[NUM_MOCK_FILES][MAX_FILE_NAME_LENGTH] = {"S01E01_Winter_Is_Coming.mkv", "S01E02_The_Kingsroad.mkv", "S01E03_Lord_Snow.mkv", "S01E04_Cripples_Bastards_and_Broken_Things.mkv", "S01E05_The_Wolf_and_the_Dragon.mkv", "S01E06_A_Golden_Crown.mkv", "S01E07_A_Clash_of_Kings.mkv", "S01E08_Fire_and_Blood.mkv", "S01E09_Baelor.mkv", "S02E01_The_North_Remembers.mkv", "S02E02_The_Night_Lands.mkv", "S02E03_What_Is_Dead_May_Never_Die.mkv", "S02E04_Garden_of_Bones.mkv", "S02E05_Ghost_of_Harrenhal.mkv", "S02E06_The_Old_Gods_and_the_New.mkv", "S02E07_A_Man_Without_Honor.mkv", "S02E08_The_Prince_of_Winterfell.mkv", "S02E09_Blackwater.mkv", "S02E10_Valar_Morghulis.mkv", "S03E01_Valar_Dohaeris.mkv", "S03E02_Dark_Wings_Dark_Words.mkv", "S03E03_Walk_of_Punishment.mkv", "S03E04_And_Now_His_Watch_Is_Ended.mkv", "S03E05_Kissed_by_Fire.mkv", "S03E06_The_Climb.mkv", "S03E07_The_Bear_and_the_Maiden_Fair.mkv", "S03E08_Second_Sons.mkv", "S03E09_The_Rains_of_Castamere.mkv", "S03E10_Mhysa.mkv", "S04E01_Two_Swords.mkv", "S04E02_The_Lion_and_the_Rose.mkv"};
    double another_arr[] = {1.40, 1.52, 1.55, 1.76, 1.52, 1.55, 1.47, 1.54, 1.38, 1.50, 1.44, 1.83, 1.68, 1.62, 1.71, 1.45, 1.48, 1.58, 1.59, 1.43, 1.27, 1.46, 1.76, 1.34, 1.44, 1.52, 1.43, 1.66, 1.32, 1.50, 1.54};
    
    for (int i = 0; i < NUM_MOCK_FILES; i++)
    {
        memcpy(files_array[i].name, arr[i], MAX_FILE_NAME_LENGTH);
        files_array[i].size = another_arr[i];
    }
    
    files_array_size = NUM_MOCK_FILES;
    return;
}

int get_file_index_by_name(const char *str)
{
    for (int i = 0; i < files_array_size; i++)
    {
        if (memcmp(files_array[i].name, str, MAX_FILE_NAME_LENGTH) == 0)
            return i;
    }
    return -1;
}


/*------------------
------NUSTREAM------
------------------*/

char stream_state_string[2][7] = {"open", "closed"};
stream_in_storage streams_array[NUM_STREAMS];
double used_storage = 0;
int last_opened_stream = 0;

#define NUM_MOCK_STREAMS 6
void get_streams_status_from_storage()
{//MOCK
    //get-all stream status
    //find all the open streams
    //restore their data
    //generate name??? TBD
    int stream_nums[NUM_MOCK_STREAMS] = {17, 18, 20, 100, 170, 178};
    int file_nums[NUM_MOCK_STREAMS] = {2, 3, 2, 5, 10, 12};
    for (int i = 0; i < NUM_MOCK_STREAMS; i++)
    {
        streams_array[stream_nums[i]].stream_state = closed;
        streams_array[stream_nums[i]].size = files_array[file_nums[i]].size;
        
        memcpy(streams_array[stream_nums[i]].name, files_array[file_nums[i]].name, MAX_FILE_NAME_LENGTH);
        if (i == 2)
            strncat(streams_array[stream_nums[i]].name, " (2)", 5);
        
        streams_array[stream_nums[i]].read_perf = 0;
        streams_array[stream_nums[i]].write_perf = 0;
    }
    
    for (int i = 0; i < NUM_STREAMS; i++)
        if (streams_array[i].stream_state != unused)
            used_storage += streams_array[i].size;
}

int get_free_stream_from_storage()
{
    for (int i = last_opened_stream; i < NUM_STREAMS; i++)
    {
        if (streams_array[i].stream_state == unused)
        {
            streams_array[i].stream_state = open;
            last_opened_stream = i;
            return i;
        }
    }
    
    for (int i = 0; i < last_opened_stream; i++)
    {
        if (streams_array[i].stream_state == unused)
        {
            streams_array[i].stream_state = open;
            last_opened_stream = i;
            return i;
        }
    }
    
    return -1;
}

void format_storage()
{//MOCK
    //send format request.
    for (int i = 0; i < NUM_STREAMS; i++)
    {
        if (streams_array[i].stream_state != unused)
        {
            streams_array[i].stream_state = unused;
            streams_array[i].size = 0;
            memset(streams_array[i].name, 0, MAX_FILE_NAME_LENGTH);
            streams_array[i].read_perf = 0;
            streams_array[i].write_perf = 0;
        }
    }
    
    used_storage = 0;
}

int get_stream_index_by_name(const char *str)
{
    for (int i = 0; i < NUM_STREAMS; i++)
    {
        if (streams_array[i].stream_state != unused)
        {
            if (memcmp(streams_array[i].name, str, MAX_FILE_NAME_LENGTH) == 0)
                return i;
        }
    }
    return -1;
}

void delete_stream_from_storage(int index)
{//MOCK
    //send delete stream
    used_storage -= streams_array[index].size;
    
    streams_array[index].stream_state = unused;
    streams_array[index].size = 0;
    memset(streams_array[index].name, 0, MAX_FILE_NAME_LENGTH);
    streams_array[index].read_perf = 0;
    streams_array[index].write_perf = 0;
}

void upload_file_to_stream_in_storage(int file_index, int stream_index)
{//MOCK
    //open stream
    
    int first_concat = 1;
    char suggested_name[MAX_FILE_NAME_LENGTH];
    memcpy(suggested_name, files_array[file_index].name, MAX_FILE_NAME_LENGTH);
    while (get_stream_index_by_name(suggested_name) != -1)
    {
        if (first_concat)
        {
            strncat(suggested_name, " (2)", 5);
            first_concat = 0;
        }
        else
        {
            int len = strlen(suggested_name);
            int index_pos = len - 2;
            if (suggested_name[index_pos - 1] == '(')
            {
                char current = suggested_name[index_pos];
                if (current == '9')
                {
                    suggested_name[index_pos] = '1';
                    suggested_name[index_pos + 1] = '0';
                    suggested_name[index_pos + 2] = ')';
                    suggested_name[index_pos + 3] = '\0';
                }
                else
                    suggested_name[index_pos] = current + 1;
            }
            else
            {//two digits
                char ones = suggested_name[index_pos];
                char tens = suggested_name[index_pos - 1];
                if (ones == '9')
                {
                    if (tens == '9')
                    {
                        printf("can't have more than 99 with same name\n");
                        exit(EXIT_FAILURE);
                    }
                    suggested_name[index_pos] = '0';
                    suggested_name[index_pos - 1] = tens + 1;
                }
                else
                    suggested_name[index_pos] = ones + 1;
            }
        }
    }
    memcpy(streams_array[stream_index].name, suggested_name, MAX_FILE_NAME_LENGTH);
    
    streams_array[stream_index].size = files_array[file_index].size;
    used_storage += streams_array[stream_index].size;
    
    streams_array[stream_index].read_perf = 0;
    streams_array[stream_index].write_perf = 0;
    
    streams_array[stream_index].stream_state = closed;
}

void download_file_from_stream_in_storage(int stream_index)
{//MOCK
    return;
}










