#ifndef MESSAGE_H
#define MESSAGE_H

#include "../improvc/improvc.h"
#include "../utils/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#define BUFFER_LENGTH 100
#define TIMEOUT 5
#define TRIES 3
#define socket_address_t struct sockaddr_in

typedef struct message{
	int router_id;
	char message[BUFFER_LENGTH];
    int buffer_length;
}message_t;

message_t* new_message();
message_t* ask_for_message();

#include "message.c"

#endif