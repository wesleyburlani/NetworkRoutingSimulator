
router_t* new_router(){

	router_t* router = malloc(sizeof(router_t));
	return router;
}

link_t* new_link(){
	link_t* link = malloc(sizeof(link_t));
	return link;
}

void print_router(void* router){

	router_t* r = (router_t*)router;
	printf("id: %d, port: %d, ", r->id, r->port);
	printf("%s\n" , r->ip);
}

void print_link(void* link){
	link_t* l = (link_t*)link;
	printf("router1: %d, ", l->router1);
	printf("router2: %d, ", l->router2);
	printf("coust: %lf\n", l->coust);
}

void print_path(void* v){

	graph_path_t* path = (graph_path_t*)v;
	printf("from: %d ",  *(int*)path->from->data);
	printf("to: %d ",  *(int*)path->to->data);
	printf("weight: %lf", path->distance);
	printf("(init: %d)\n",  *(int*)path->start->data);
}


int compare_dest_path(void* r1, void* r2){
	return *(int*)((graph_path_t*)r1)->to->data == *(int*)r2;
}

int compare_router(void* r1, void* r2){

	return ((router_t*)r1)->id == ((router_t*)r2)->id;
}

int compare_id_to_router(void* r1, void* r2){

	return *(int*)r2 == ((router_t*)r1)->id;
}

list_t* read_routers(){

	list_t* list = new_list(sizeof(router_t));
	char* router_config = file_read("inputs/roteador.config");
	list_t* lines = string_split(router_config, "\n");

	node_t* element = lines->head;
	while(element != NULL){ 

		char* data = ((char*)element->data);
		printf("%s\n", data);
		router_t* router = new_router();
		list_t* columns = string_split(data, "\t");
		router->id = atoi((char*)list_get_element(columns, 0)->data);
		router->port = atoi((char*)list_get_element(columns, 1)->data);
		router->ip = (char*)list_get_element(columns, 2)->data;
		list_append(list, router);
		element = element->next;
	}

	return list;
}

list_t* read_links(){

	list_t* list = new_list(sizeof(link_t));
	char* router_config = file_read("inputs/enlaces.config");
	list_t* lines = string_split(router_config, "\n");

	node_t* element = lines->head;
	while(element != NULL){ 

		char* data = ((char*)element->data);
		link_t* link = new_link();
		list_t* columns = string_split(data, "\t");
		link->router1 = atoi((char*)list_get_element(columns, 0)->data);
		link->router2 = atoi((char*)list_get_element(columns, 1)->data);
		link->coust = atof((char*)list_get_element(columns, 2)->data);
		list_append(list, link);
		element = element->next;
	}

	return list;
}

graph_t* graph_from_routers(list_t* routers, list_t* links){

	graph_t* graph = new_graph(sizeof(router_t), 0);

	node_t* element = routers->head;
	while(element != NULL){
		router_t* router = (router_t*)element->data;
		graph_add_vertex(graph, router);
		element = element->next;
	}

	element = links->head;
	while(element != NULL){
		link_t* link = (link_t*)element->data;
		int id1 = link->router1;
		int id2 = link->router2;
		graph_add_adjacency_by_data(graph, &id1, &id2, link->coust, compare_router);
		element = element->next;
	}

	return graph;
}

list_t* get_routing_table(graph_t* graph, int id_router){

	list_t* routing_table = new_list(sizeof(graph_path_t));

	list_t* total = GetMinDistances(graph, compare_router);

	node_t* element = total->head;

	while(element != NULL){

		graph_path_t* path = (graph_path_t*)element->data;

		int from = ((router_t*)path->from->data)->id;

		if(from == id_router)
			list_append(routing_table, path);

		element = element->next;
	}

	return routing_table;
}