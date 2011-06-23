
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 

#include "simplenet.h"

void NetError(const char *msg) {
    perror(msg);
    exit(1);
}

int NetGetConnection(int port, connection_t *c) {
  c->sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (c->sockfd < 0) 
    NetError("ERROR opening socket");
  bzero((char *) &c->serv_addr, sizeof(c->serv_addr));
  c->serv_addr.sin_family = AF_INET;
  c->serv_addr.sin_addr.s_addr = INADDR_ANY;
  c->serv_addr.sin_port = htons(port);
  if (bind(c->sockfd, (struct sockaddr *) &c->serv_addr,
	   sizeof(c->serv_addr)) < 0) 
    NetError("ERROR on binding");
  listen(c->sockfd,5);
  c->clilen = sizeof(c->cli_addr);
  c->newsockfd = accept(c->sockfd, 
			(struct sockaddr *) &c->cli_addr, 
			&c->clilen);
  if (c->newsockfd < 0) 
    NetError("ERROR on accept");

  return 0;
}

int NetRead(connection_t *c, char *buffer, int maxsize) {
  int n;

  bzero(buffer, maxsize);
  n = read(c->newsockfd, buffer, maxsize);
  if (n < 0) NetError("ERROR reading from socket");
  return n;
}

void NetWrite(connection_t *c, char *buffer, int len) {
  int n;

  n = write(c->newsockfd, buffer, len);
  if (n < 0) NetError("ERROR writing to socket");
}

void NetClose(connection_t *c) {
  close(c->newsockfd);
  close(c->sockfd);
}
