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


#define BUFFERLENGTH 100

int get_socket(){

	int isocket;

    if ((isocket=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){

    	printf("Can't to get socket");
    	exit(1);
    }  

    return isocket;
}

struct sockaddr_in get_address_in(router_t* router){

	struct sockaddr_in si_other;
	int slen=sizeof(si_other);
	memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(router->port);
     
    if (inet_aton(router->ip->string , &si_other.sin_addr) == 0) 
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
    return si_other;
}

void send_message(char* message, int socket, struct sockaddr_in address_in){

	if(sendto(socket, message, strlen(message),
	 0, (struct sockaddr *) &address_in, sizeof(address_in))==-1){
        
        printf("Can't sent message");
    	exit(1);
    }
}

char* get_message(int socket, struct sockaddr_in address_in){

	char* buf = malloc(sizeof(char*)*BUFFERLENGTH);
	memset(buf,'\0', BUFFERLENGTH);
	unsigned int len = sizeof(address_in);
	if (recvfrom(socket, buf, BUFFERLENGTH,
	 0, (struct sockaddr *) &address_in, &len) == -1)
    {
        printf("Can't receive message");
    	exit(1);
    }	
         
    puts(buf);
	close(socket);
    return buf;
}

int main(int argc, char **argv){

	int router_id = 1;

	list_t* routers = read_routers();
	list_t* links = read_links();

	graph_t* graph = graph_from_routers(routers, links);

	list_t* l = get_routing_table(graph, 1);
	
	router_t* router = (router_t*)list_get_by_data(routers, &router_id, compare_router)->data;

	int socket = get_socket();
	struct sockaddr_in address_in = get_address_in(router);

	send_message("oi", socket, address_in);

	char* message = get_message(socket, address_in);

	printf("finished router %s\n", message);


	return 0;
}