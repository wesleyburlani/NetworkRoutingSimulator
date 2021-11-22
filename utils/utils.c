void print_routing_table(void* v){
	graph_path_t* path = (graph_path_t*)v;
	printf("weight %8.2lf ", path->distance);
	if(path->to != NULL)
		printf("to destination %d ",  *(int*)path->to->data);
	if(path->start != NULL)
		printf("starting by: %d\n",  *(int*)path->start->data);
	else
		printf("unreachable\n");
}

void print_resume(router_t* router){
	printf("Router info...\n");
	printf("----------------------------------------------\n");
	printf("Router   id: %d\n", router->id);
	printf("Router port: %d\n", router->port);
	printf("Router   ip: %s\n" , router->ip);
	printf("----------------------------------------------\n");
	printf("Loading routing table...\n");
	printf("----------------------------------------------\n");
	list_print(router->routingTable, print_routing_table);
	printf("----------------------------------------------\n");
}

void die(char *s){
    perror(s);
    exit(1);
}
