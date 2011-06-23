#ifndef SIMPLENET_H_
#define SIMPLENET_H_

// Simple barebones network code based off of a basic sockets tutorial.
// Do not use for anything serious. :)

#include <sys/socket.h>
#include <netinet/in.h>



typedef struct connection_s {
  int sockfd, newsockfd, portno;
  socklen_t clilen;
  struct sockaddr_in serv_addr, cli_addr;
} connection_t;

void NetError(const char *msg);
int NetGetConnection(int port, connection_t *c);
int NetRead(connection_t *c, char *buffer, int maxsize);
void NetWrite(connection_t *c, char *buffer, int len);
void NetClose(connection_t *c);

#endif // SIMPLENET_H_
