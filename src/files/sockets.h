#ifndef SOCKETS_H
#define SOCKETS_H



#define PORT 65432

#define RES_SIZE 256*256

#define MAGIC_WORD "Demo"
#define MAGIC_SIZE 4
#define MAX_DATA_SIZE 256

typedef struct message{
    char magic[MAGIC_SIZE]; //should be 'Demo'
    int  counter;
    int  command_type;
    int  data_size;
    char data[MAX_DATA_SIZE];
} message;

enum command_types {
    get_device_status = 0,
    get_files_status = 1,
    stop_all = 2,
    upload_file_to_stream = 3,
    download_file_from_stream = 4,
    delete_stream = 5,
    format = 6,
    test = 7
} ;

void *recv_socket_func(void *vargp);
void *trans_socket_func(void *vargp);

#endif
