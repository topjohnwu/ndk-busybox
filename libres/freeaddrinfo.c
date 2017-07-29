#include <sys/socket.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include "dietdns.h"

void diet_freeaddrinfo(struct addrinfo *res) {
  while (res) {
    struct addrinfo *duh;
    duh=res;
    res=res->ai_next;
    free(duh);
  }
}
