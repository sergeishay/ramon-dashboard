#ifndef FIFO_H
#define FIFO_H

#define NO_ERR 0
#define FAILED -1

typedef enum {
    no_err,
    failed
} ret_val;

#define FIFO_SIZE 50
typedef struct {
    int producer;
    int consumer;
    void *buffers[FIFO_SIZE];
} fifo;

extern fifo requests_fifo;
extern fifo updates_fifo;

void init_fifo(fifo *fifo_ptr);

ret_val fifo_enq(fifo *fifo_ptr, void *data);

void *fifo_deq(fifo *fifo_ptr);

#endif
