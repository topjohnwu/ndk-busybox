#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include "dietdns.h"

/* needs:
 * __dns_gethostbyx_r
 */

/* Oh boy, this interface sucks so badly, there are no words for it.
 * Not one, not two, but _three_ error signalling methods!  (*h_errnop
 * nonzero?  return value nonzero?  *RESULT zero?)  The glibc goons
 * really outdid themselves with this one. */
int diet_gethostbyname2_r(const char* name, int AF, struct hostent* result,
				char *buf, size_t buflen,
				struct hostent **RESULT, int *h_errnop) {
  size_t L=strlen(name);
  int lookfor=0;
  switch (AF) {
  case AF_INET: lookfor=1; break;
  case AF_INET6: lookfor=28; break;
  default: *h_errnop=EINVAL; return 1;
  }
  result->h_name=buf;
  if (buflen<L) { *h_errnop=ERANGE; return 1; }
  strcpy(buf,name);
  return __dns_gethostbyx_r(name,result,buf+L,buflen-L,RESULT,h_errnop,lookfor);
}
