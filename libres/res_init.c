#include <resolv.h>
#include <netdb.h>
#include <netinet/in.h>
#include "dietdns.h"

extern void __dns_readstartfiles(void);

int diet_res_init(void) {
  _diet_res.nscount=0;
  __dns_readstartfiles();
  return 0;
}
