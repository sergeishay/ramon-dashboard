#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "sockets.h"
#include "fifo.h"

extern int response_ready;
extern char response_buffer[RES_SIZE];

void *trans_socket_func(void *vargp)
{
    int server_socket, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    
    // Create a socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    // Set socket options
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    
    // Configure server address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;  // Listen on all interfaces
    address.sin_port = htons(PORT);
    
    // Bind the socket to the address
    if (bind(server_socket, (struct sockaddr *)&address, addrlen) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    // Listen for connections
    if (listen(server_socket, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    // Accept a connection
    if ((new_socket = accept(server_socket, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    
    while (1)
    {
        //---------------------
        //      transmit       
        //---------------------
        if (response_ready)
        {
            //there is a response to send
            send(new_socket, response_buffer, RES_SIZE, 0);
            sleep(0.5);
            response_ready = 0;
        }
        sleep(0.1);
    }
    
    // Close the connection
    close(new_socket);
    close(server_socket);
    
    return 0;
}


void *recv_socket_func(void *vargp)
{
    int server_socket, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    
    // Create a socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    // Set socket options
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    
    // Configure server address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;  // Listen on all interfaces
    address.sin_port = htons(PORT + 1);
    
    // Bind the socket to the address
    if (bind(server_socket, (struct sockaddr *)&address, addrlen) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    // Listen for connections
    if (listen(server_socket, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    // Accept a connection
    if ((new_socket = accept(server_socket, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    
    message *msg_p = (message *)malloc(sizeof(message));
    if (msg_p == NULL)
    {
        perror("malloc: ");
        exit(EXIT_FAILURE);
    }
    
    while (1)
    {
        int valread = read(new_socket, msg_p, sizeof(message));
        if (valread < 0)
        {
            free(msg_p);
            perror("read");
            exit(EXIT_FAILURE);
        }
        else if (valread == 0)
        {
            continue;
        }
        
        // Print received message
        printf("Received from client: \nmagic: %s\ncounter: %d\ntype: %d\nsize: %d\ndata: %s\n\n", 
                    msg_p->magic, ntohl(msg_p->counter), ntohl(msg_p->command_type), ntohl(msg_p->data_size), msg_p->data);
        
        msg_p->counter = ntohl(msg_p->counter);
        msg_p->command_type = ntohl(msg_p->command_type);
        msg_p->data_size = ntohl(msg_p->data_size);
        
        if (fifo_enq(&requests_fifo, (void *)msg_p) == failed)
        {
            printf("error: fifo_enq failed\n");
            free(msg_p);
            perror("fifo_enq");
            exit(EXIT_FAILURE);
        }
        
        //allocate for next time
        msg_p = (message *)malloc(sizeof(message));
        if (msg_p == NULL)
        {
            perror("malloc: ");
            exit(EXIT_FAILURE);
        }
        
    }
    
    // Close the connection
    close(new_socket);
    close(server_socket);
    
    return 0;
}
