#include <stdio.h>

#include "fifo.h"

fifo requests_fifo;
fifo updates_fifo;


void init_fifo(fifo *fifo_ptr)
{
    fifo_ptr->producer;
    fifo_ptr->consumer;
    for (int i = 0 ; i < FIFO_SIZE; i++)
        fifo_ptr->buffers[i] = NULL;
}


ret_val fifo_enq(fifo *fifo_ptr, void *data)
{
    if (data == NULL)
    {
        printf("fifo_enq ERROR: data is NULL\n");
        return failed;
    }
    
    if ((fifo_ptr->producer == fifo_ptr->consumer - 1) || 
       ((fifo_ptr->producer == FIFO_SIZE - 1) && (fifo_ptr->consumer == 0)))
    {
        return failed;
    }
    
    fifo_ptr->buffers[fifo_ptr->producer] = data;
    if (fifo_ptr->producer == FIFO_SIZE - 1)
        fifo_ptr->producer = 0;
    else
        (fifo_ptr->producer)++;
    
    return no_err;
}


void *fifo_deq(fifo *fifo_ptr)
{
    void *ret = NULL;
    if (fifo_ptr->producer != fifo_ptr->consumer)
    {
        ret = fifo_ptr->buffers[fifo_ptr->consumer];
        if (fifo_ptr->consumer == FIFO_SIZE - 1)
            fifo_ptr->consumer = 0;
        else
            (fifo_ptr->consumer)++;
    }
    return ret;
}







