#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/icmp6.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <netinet/ip6.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "dietdns.h"

union common_sockaddr {
  struct sockaddr sa;
  struct sockaddr_in sin;
  struct sockaddr_in6 sin6;
};
typedef union common_sockaddr sockaddr_any;

static int getaddr (const char *name, sockaddr_any *addr, int af) {
	int ret;
	struct addrinfo hints, *ai, *res = NULL;

	memset (&hints, 0, sizeof (hints));
	hints.ai_family = af;
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_flags = AI_ADDRCONFIG;

	ret = diet_getaddrinfo (name, NULL, &hints, &res);
	if (ret) {
		fprintf (stderr, "%s: %s\n", name, diet_gai_strerror (ret));
		return -1;
	}

	for (ai = res; ai; ai = ai->ai_next) {
	    if (ai->ai_family == af)  break;
	}
	if (!ai)  ai = res;	/*  anything...  */

	if (ai->ai_addrlen > sizeof (*addr))
		return -1;	/*  paranoia   */
	memcpy (addr, ai->ai_addr, ai->ai_addrlen);

        if(res != NULL)
          diet_freeaddrinfo (res);

	return 0;
}

static void printaddr(sockaddr_any *addr) {
  char addr_str[INET6_ADDRSTRLEN];

  if(addr->sa.sa_family == AF_INET) {
    printf("family = AF_INET\n");
    if(inet_ntop(addr->sin.sin_family, &addr->sin.sin_addr, (char *)&addr_str, sizeof(addr_str)) == NULL) {
      perror("inet_ntop failed");
      return;
    }
    printf("addr = %s\n", addr_str);
  } else if(addr->sa.sa_family == AF_INET6) {
    printf("family = AF_INET6\n");
    if(inet_ntop(addr->sin6.sin6_family, &addr->sin6.sin6_addr, (char *)&addr_str, sizeof(addr_str)) == NULL) {
      perror("inet_ntop failed");
      return;
    }
    printf("addr = %s\n", addr_str);
  } else {
    printf("unknown family = %d\n",addr->sa.sa_family);
  }
}

int main() {
  sockaddr_any dst_addr = {{ 0, }, };
  struct sockaddr_in lsa_u;

  if (getaddr("www.datapipe.net", &dst_addr, AF_INET) < 0) {
    fprintf(stderr, "getaddr failed\n");
  } else {
    printaddr(&dst_addr);
  }

  if (getaddr("www.datapipe.net", &dst_addr, AF_INET6) < 0) {
    fprintf(stderr, "getaddr failed\n");
  } else {
    printaddr(&dst_addr);
  }

  inet_aton("127.0.0.1",&lsa_u.sin_addr);
  lsa_u.sin_port=htons(53);
  lsa_u.sin_family=AF_INET;

  _diet_res.nscount = 1;
  _diet_res.nsaddr_list[0] = lsa_u;

  if (getaddr("www.datapipe.net", &dst_addr, AF_INET6) < 0) {
    fprintf(stderr, "getaddr failed\n");
  } else {
    printaddr(&dst_addr);
  }

  return 0;
}
