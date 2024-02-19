#ifndef STORAGE_H
#define STORAGE_H

#define MAX_FILE_NAME_LENGTH 256


void init_big_arrays();

/*------------------
-----HARD DRIVE-----
------------------*/
#define MAX_FILES_IN_HARD_DRIVE 256
typedef struct
{
    char name[MAX_FILE_NAME_LENGTH];
    double size;
} file_in_hard_drive;

extern file_in_hard_drive files_array[MAX_FILES_IN_HARD_DRIVE];
extern int files_array_size;

void get_files_from_hard_drive();

int get_file_index_by_name(const char *str);


/*------------------
------NUSTREAM------
------------------*/
typedef enum
{
    open = 0,
    closed = 1,
    unused
} stream_state_e;

#define NUM_STREAMS 198
typedef struct
{
    stream_state_e stream_state;
    char name[MAX_FILE_NAME_LENGTH];
    double size;
    double read_perf;
    double write_perf;
} stream_in_storage;

extern char stream_state_string[2][7];
extern stream_in_storage streams_array[NUM_STREAMS];

extern double used_storage;

void get_streams_status_from_storage();
int get_free_stream_from_storage();
void format_storage();
void delete_stream_from_storage(int index);
void upload_file_to_stream_in_storage(int file_index, int stream_index);
void download_file_from_stream_in_storage(int stream_index);

int get_stream_index_by_name(const char *str);

#endif
