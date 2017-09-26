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

#define BUFLEN 100  //Max length of buffer
#define SocketAddress struct sockaddr_in
#define TIMEOUT 5
#define TRIES 3

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct MessageData{
	
	int routerId;
	char message[BUFLEN];

}MessageData;

MessageData* NewMessageData(){

	MessageData* data = malloc(sizeof(MessageData));
	//data->message = malloc(sizeof(char));
	return data;
}

MessageData* GetMessage(){

	MessageData* data = NewMessageData();
	printf("type router name to send message: ");
	scanf("%d", &(data->routerId));
	printf("type message to send: ");
	scanf("%s", data->message);
	return data;
}

void die(char *s){
    perror(s);
    exit(1);
}

void send_to_next(router_t* router, MessageData* data){

	SocketAddress socketAddress;
    int socketId;
	socklen_t slen = sizeof(socketAddress);
	 
    if ((socketId=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        die("sender: Error getting socket\r\n");
    
    memset((char *) &socketAddress, 0, sizeof(socketAddress));
    socketAddress.sin_family = AF_INET;

    node_t* node = (node_t*)list_get_by_data((router->routingTable), &(data->routerId), compare_dest_path);

	if(node == NULL){
		printf("sender: router with id %d not exist", data->routerId);
		return;
	}
	// get table info with path to send package
	graph_path_t* routerToSend = (graph_path_t*)node->data;
	// get first neighboor to send package
	router_t* neighboor = (router_t*)routerToSend->start->data;
	// send message to correct port
	socketAddress.sin_port = htons(neighboor->port);
 	// set ip to destination ip
	if (inet_aton(neighboor->ip , &socketAddress.sin_addr) == 0) {
    	fprintf(stderr, "sender: inet_aton() failed\n");
    	exit(1);
	}

	printf("sender: sending packet to router %d\n", neighboor->id);

	int tries = TRIES;
	int ack = 0;
	while(tries--){

		if (sendto(socketId, data, sizeof(data)+(BUFLEN) , 0 , (struct sockaddr *) &socketAddress, slen)==-1)
			die("sender: sendto()\n");
				
		struct timeval read_timeout;
		read_timeout.tv_sec = TIMEOUT;
		read_timeout.tv_usec = 10;
		
		setsockopt(socketId, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof read_timeout);

		if (recvfrom(socketId, &ack, sizeof(ack), 0, (struct sockaddr *) &socketAddress, &slen) == -1)
			printf("Error send package, retrying\n");
		
		if(ack)
			break;
	}
	if(!ack)
		printf("It's not possible to send package\n");
}

void* call_sender(void* arg_router){

	router_t* router = (router_t*)arg_router;  
    while(1){
		MessageData* data = GetMessage();
		send_to_next(router, data);
    }	
    return 0;
}

void* call_receiver(void* arg_router){

	router_t* router = (router_t*)arg_router;
    SocketAddress currentSocket, destinationSocket;     
    int socketId, receivedDataLength;
    socklen_t slen = sizeof(destinationSocket);
     
    if ((socketId=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        die("receiver: socket\n");
     
    // zero out the structure
    memset((char *) &currentSocket, 0, sizeof(currentSocket));
     
    currentSocket.sin_family = AF_INET;
    currentSocket.sin_port = htons(router->port);
    currentSocket.sin_addr.s_addr = htonl(INADDR_ANY);
     
    if(bind(socketId , (struct sockaddr*)&currentSocket, sizeof(currentSocket)) == -1)
        die("receiver: bind\n");

    while(1){

    	MessageData* data = NewMessageData();
        fflush(stdout);

        if ((receivedDataLength = recvfrom(socketId, data, sizeof(data)+(BUFLEN), 0, (struct sockaddr *) &destinationSocket, &slen)) == -1)
            die("receiver: recvfrom()\n");
         
        printf("\nreceiver: Received packet from %s:%d\n", inet_ntoa(destinationSocket.sin_addr), ntohs(destinationSocket.sin_port));
        
        if(data->routerId == router->id)
        	printf("Data: %s\n" , data->message);
		
		else{
        	printf("package will be sent to router: %d\n" , data->routerId);
        	send_to_next(router, data);
        }
        
        int ack = 1;
        if (sendto(socketId, &ack, sizeof(ack), 0, (struct sockaddr*) &destinationSocket, slen) == -1)
            die("receiver: sendto()\n");
    }
 
    close(socketId);
    return 0;
}

void start_operation(router_t* router){
	pthread_t threads[2];
	pthread_create(&(threads[0]), NULL, call_receiver, router);
	pthread_create(&(threads[1]), NULL, call_sender, router);
	pthread_join(threads[1], NULL);
	pthread_join(threads[0], NULL);
}

void print_routingTable(void* v){

	graph_path_t* path = (graph_path_t*)v;
	printf("weight %8.2lf ", path->distance);
	printf("to destination %d ",  *(int*)path->to->data);
	printf("starting by: %d\n",  *(int*)path->start->data);
}

void print_init(router_t* router){
	printf("Router info...\n");
	printf("----------------------------------------------\n");
	printf("Router   id: %d\n", router->id);
	printf("Router port: %d\n", router->port);
	printf("Router   ip: %s\n" , router->ip);
	printf("----------------------------------------------\n");
	printf("Loading routing table...\n");
	printf("----------------------------------------------\n");
	list_print(router->routingTable, print_routingTable);
	printf("----------------------------------------------\n");
}

int main(int argc, char **argv){

	int router_id = atoi(argv[1]);
	printf("Starting router %d\n", router_id);

	list_t* routers = read_routers();
	list_t* links = read_links();

	graph_t* graph = graph_from_routers(routers, links);

	list_t* routingTable = get_routing_table(graph, router_id);

	router_t* router = (router_t*)list_get_by_data(routers, &router_id, compare_id_to_router)->data;
	router->routingTable = routingTable;

	print_init(router);	

	start_operation(router);

	return 0;
}