#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "improvc/improvc.h"
#include "network/network.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#define BUFLEN 512  //Max length of buffer

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct MessageData{
	
	int routerId;
	char* message;

}MessageData;

MessageData* NewMessageData(){

	MessageData* data = malloc(sizeof(MessageData));
	data->message = malloc(sizeof(char));
	return data;
}

MessageData* GetMessage(){

	MessageData* data = NewMessageData();
	printf("Type router name to send message: ");
	scanf("%d", &(data->routerId));
	printf("Type message to send: ");
	scanf("%s", data->message);
	return data;
}


void die(char *s)
{
    perror(s);
    exit(1);
}

void* call_sender(void* arg_router)
{
	router_t* router = (router_t*)arg_router;

    struct sockaddr_in si_other;
    int s, i, slen=sizeof(si_other);
    char buf[BUFLEN];
    char message[BUFLEN];
 
    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        die("sender: Error getting socket\r\n");
    
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    
 
    while(1)
    {
		MessageData* data = GetMessage();
		// find in routing table the from instancied router to destination;
		node_t* node = (node_t*)list_get_by_data((router->routingTable), &(data->routerId), compare_dest_path);

		if(node == NULL){
			printf("sender: router with id %d not exist", data->routerId);
			continue;
		}
		// get table info with path to send package
		graph_path_t* routerToSend = (graph_path_t*)node->data;
		// get first neighboor to send package
		router_t* neighboor = (router_t*)routerToSend->start->data;
		// send message to correct port
		si_other.sin_port = htons(neighboor->port);
     	// set ip to destination ip
    	if (inet_aton(neighboor->ip , &si_other.sin_addr) == 0) 
    	{
        	fprintf(stderr, "sender: inet_aton() failed\n");
        	exit(1);
    	}

		printf("sender: send to router %d\n", neighboor->id);

        if (sendto(s, data->message, strlen(data->message) , 0 , (struct sockaddr *) &si_other, slen)==-1)
        {
            die("sender: sendto()\n");
        }
     
        //receive a reply and print it
        //clear the buffer by filling null, it might have previously received data
        memset(buf,'\0', BUFLEN);
        //try to receive some data, this is a blocking call
        if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == -1)
        {
            die("sender: recvfrom()\n");
        }
         
        puts(buf);
    }
 
    close(s);
    return 0;
}

void* call_receiver(void* arg_router)
{
	router_t* router = (router_t*)arg_router;

	print_router(router);

    struct sockaddr_in si_me, si_other;
     
    int s, i, slen = sizeof(si_other) , recv_len;
    char buf[BUFLEN];
     
    //create a UDP socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("receiver: socket\n");
    }
     
    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));
     
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(router->port);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
     
    //bind socket to port
    if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        die("receiver: bind\n");
    }
     
    //keep listening for data
    while(1)
    {
        //printf("receiver: Waiting for data...\n");
        fflush(stdout);
        //receive a reply and print it
        //clear the buffer by filling null, it might have previously received data
        memset(buf,'\0', BUFLEN);

        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
        {
            die("receiver: recvfrom()\n");
        }
         
        //print details of the client/peer and the data received
        printf("receiver: Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        printf("receiver: Data: %s\n" , buf);
         
        //now reply the client with the same data
        if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1)
        {
            die("receiver: sendto()\n");
        }
    }
 
    close(s);
    return 0;
}

void start_operation(router_t* router){
	pthread_t threads[2];
	pthread_create(&(threads[0]), NULL, call_receiver, router);
	pthread_create(&(threads[1]), NULL, call_sender, router);
	pthread_join(threads[1], NULL);
	pthread_join(threads[0], NULL);
}

int main(int argc, char **argv){

	int router_id = atoi(argv[1]);
	printf("Starting router %d\n", router_id);

	list_t* routers = read_routers();
	list_t* links = read_links();

	graph_t* graph = graph_from_routers(routers, links);

	list_t* routingTable = get_routing_table(graph, 1);

	router_t* router = (router_t*)list_get_by_data(routers, &router_id, compare_id_to_router)->data;
	router->routingTable = routingTable;

	start_operation(router);

	return 0;
}