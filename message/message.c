
message_t* new_message(){
	message_t* data = malloc(sizeof(message_t));
    data->buffer_length = BUFFER_LENGTH;
	return data;
}

message_t* ask_for_message(){
	message_t* data = new_message();
	printf("type router name to send message: ");
	scanf("%d", &(data->router_id));
	getchar();
	printf("type message to send: ");
	fflush(stdout);
	fflush(stdin);
	fgets(data->message, BUFFER_LENGTH, stdin);
	return data;
}

int is_unreachable_node(node_t* node, int router_id){
	if(node == NULL){
		printf("sender: router with id %d not exist", router_id);
		return;
	}
	// get table info with path to send package
	graph_path_t* route_to_send = (graph_path_t*)node->data;

	if(route_to_send == NULL){
		printf("sender: unreacheable\n");
		return;
	}
	// get first neighboor to send package
	if(route_to_send->start == NULL){
		printf("sender: unreacheable\n");
		return;
	}

	router_t* neighboor = (router_t*)route_to_send->start->data;
	if(neighboor == NULL){
		printf("sender: unreacheable\n");
		return;
	}
}

void send_message(router_t* router, message_t* data){
	socket_address_t socket_address;
    int socket_id;
	socklen_t socket_length = sizeof(socket_address);
	 
    if ((socket_id=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        die("sender: Error getting socket\r\n");
    
    memset((char *) &socket_address, 0, sizeof(socket_address));
    socket_address.sin_family = AF_INET;

	node_t* node = (node_t*)list_get_by_data((router->routingTable), 
												&(data->router_id), 
												compare_dest_path
											);
	if(node == NULL){
		printf("sender: router with id %d not exist", data->router_id);
		return;
	}
	is_unreachable_node(node, data->router_id);

	graph_path_t* route_to_send = (graph_path_t*)node->data;
	router_t* neighboor = (router_t*)route_to_send->start->data;

	// send message to correct port
	socket_address.sin_port = htons(neighboor->port);
 	// set ip to destination ip
	if (inet_aton(neighboor->ip , &socket_address.sin_addr) == 0) {
    	fprintf(stderr, "sender: inet_aton() failed\n");
    	exit(1);
	}
	printf("sender: sending packet to router %d\n", neighboor->id);

	int tries = TRIES;
	int ack = 0;
	while(tries--){

		if (sendto(socket_id, 
					data, 
					sizeof(data)+(data->buffer_length) , 
					0 , 
					(struct sockaddr *) &socket_address, 
					socket_length
			)==-1) {
			die("sender: sendto()\n");
		}
				
		struct timeval read_timeout;
		read_timeout.tv_sec = TIMEOUT;
		read_timeout.tv_usec = 10;
		
		setsockopt(socket_id, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof read_timeout);

		if (recvfrom(socket_id, 
					&ack, 
					sizeof(ack), 
					0, 
					(struct sockaddr *) &socket_address, 
					&socket_length
			) == -1) {
			printf("Error send package, retrying\n");
		}
		
		if(ack)
			break;
	}
	if(!ack)
		printf("It's not possible to send package\n");
}