#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "improvc/improvc.h"
#include "network/network.h"
#include "message/message.h"
#include "utils/utils.h"
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *start_sender_process(void *arg_router)
{
  router_t *router = (router_t *)arg_router;
  while (1)
  {
    message_t *data = ask_for_message();
    send_message(router, data);
  }
  return 0;
}

void *start_receiver_process(void *arg_router)
{
  router_t *router = (router_t *)arg_router;
  socket_address_t current_socket, destination_socket;
  int socket_id, received_data_length;
  socklen_t socket_length = sizeof(destination_socket);

  if ((socket_id = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    die("receiver: socket\n");

  // zero out the structure
  memset((char *)&current_socket, 0, sizeof(current_socket));

  current_socket.sin_family = AF_INET;
  current_socket.sin_port = htons(router->port);
  current_socket.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(socket_id, (struct sockaddr *)&current_socket, sizeof(current_socket)) == -1)
    die("receiver: bind\n");

  while (1)
  {
    message_t *data = new_message();
    fflush(stdout);

    if ((received_data_length = recvfrom(socket_id,
                                         data,
                                         sizeof(data) + (data->buffer_length),
                                         0,
                                         (void *)&destination_socket,
                                         &socket_length)) == -1)
    {
      die("receiver: recvfrom()\n");
    }

    printf("\nreceiver: Received packet from %s:%d\n",
           inet_ntoa(destination_socket.sin_addr),
           ntohs(destination_socket.sin_port));

    if (data->router_id == router->id)
    {
      printf("data received: %s\n", data->message);
    }
    else
    {
      printf("package will be sent to router: %d\n", data->router_id);
      send_message(router, data);
    }

    int ack = 1;
    if (sendto(socket_id,
               &ack,
               sizeof(ack),
               0,
               (void *)&destination_socket,
               socket_length) == -1)
    {
      die("receiver: sendto()\n");
    }
  }
  close(socket_id);
  return 0;
}

void start_operation(router_t *router)
{
  pthread_t threads[2];
  pthread_create(&(threads[0]), NULL, start_receiver_process, router);
  pthread_create(&(threads[1]), NULL, start_sender_process, router);
  pthread_join(threads[1], NULL);
  pthread_join(threads[0], NULL);
}

int main(int argc, char **argv)
{
  int router_id = atoi(argv[1]);
  printf("Starting router %d\n", router_id);

  list_t *routers = read_routers();
  list_t *links = read_links();

  graph_t *graph = graph_from_routers(routers, links);
  list_t *routingTable = get_routing_table(graph, router_id);
  router_t *router = (router_t *)list_get_by_data(routers, &router_id, compare_id_to_router)->data;
  router->routingTable = routingTable;

  print_resume(router);
  start_operation(router);
  return 0;
}