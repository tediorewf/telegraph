#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

typedef struct {
    short sun_family;
    u_short sin_port;
    u_long sin_addr;
} sockaddr;

typedef struct {
    int sid;
    short sun_family;
    u_short sin_port;
    u_long sin_addr;
} rcvmsg_args;

void* receive_message(void* args) {
    rcvmsg_args* args_ptr = (rcvmsg_args*)args;

    int sid = args_ptr->sid;

    sockaddr addr;
    addr.sun_family = args_ptr->sun_family;
    addr.sin_port = args_ptr->sin_port;
    addr.sin_addr = args_ptr->sin_addr;

    int addr_len = sizeof(args_ptr);

    while (1) {
        char received_data[1024];
        recvfrom(sid, received_data, sizeof(received_data), 0, &addr, &addr_len);
        printf("You have got a message: %s", received_data);
    }

    return NULL;
}

int main(int argc, const char* argv[]) {
    sockaddr addr;
    addr.sun_family = AF_INET;
    addr.sin_port = htons(4004);
    addr.sin_addr = inet_addr("0.0.0.0");
    int addr_len = sizeof(addr);

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    
    if (bind(sock, &addr, sizeof(addr)) == -1) {
        fprintf(stderr, "Error: failed to bind socket.");
        exit(-1);
    }

    puts("Running telegraph on host 0.0.0.0 and port 4004...");

    pthread_t thid_receive_message;

    rcvmsg_args args_receive_message;
    args_receive_message.sid = sock;
    args_receive_message.sun_family = addr.sun_family;
    args_receive_message.sin_port = addr.sin_port;
    args_receive_message.sin_addr = addr.sin_addr;

    pthread_create(&thid_receive_message, NULL, receive_message, &args_receive_message);

    // TODO: sendto

    pthread_join(thid_receive_message, NULL);

    close(sock);
    exit(EXIT_SUCCESS);
}
