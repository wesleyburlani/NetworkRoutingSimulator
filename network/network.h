#ifndef NETWORKC_H
#define NETWORKC_H

#include "../improvc/improvc.h"
#include <stdio.h>

typedef struct router_t{

	int id;
	int port;
	char* ip;

}router_t;

typedef struct link_t{

	int router1;
	int router2;
	double coust;

}link_t;

router_t* new_router();
link_t* new_link();
void print_router(void* router);
void print_link(void* link);
void print_path(void* v);
int compare_router(void* r1, void* r2);
list_t* read_routers();
list_t* read_links();
graph_t* graph_from_routers(list_t* routers, list_t* links);
list_t* get_routing_table(graph_t* graph, int id_router);

#include "network.c"

#endif