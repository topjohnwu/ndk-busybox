#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <resolv.h>
#include <net/if.h>
#include <netdb.h>
#include "dietdns.h"

#ifdef __BIONIC__
#include <sys/system_properties.h>
#endif


/* needs:
 * _res
 */

int __dns_fd=-1;

/* the ad-hoc internal API from hell ;-) */
void __dns_make_fd(void);
void __dns_make_fd6(void);
void __dns_readstartfiles(void);
int __dns_decodename(const unsigned char *packet,unsigned int offset,unsigned char *dest,
		     unsigned int maxlen,const unsigned char* behindpacket);

void __dns_make_fd(void) {
  int tmp;
  struct sockaddr_in si;
  if (__dns_fd>=0) return;
  tmp=socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
  if (tmp<0) return;
  fcntl(tmp,F_SETFD,FD_CLOEXEC);
  si.sin_family=AF_INET;
  si.sin_port=0;
  si.sin_addr.s_addr=INADDR_ANY;
  if (bind(tmp,(struct sockaddr*)&si,sizeof(si))) return;
  __dns_fd=tmp;
}

static int parsesockaddr(const char* c,void* x) {
  struct sockaddr_in to;
  if (inet_aton(c,&to.sin_addr)) {
    to.sin_port=htons(53);
    to.sin_family=AF_INET;
    memmove(x,&to,sizeof(struct sockaddr_in));
    return 1;
  }
  return 0;
}

#define MAX_DNS_PROPERTIES 8
#define DNS_PROP_NAME_PREFIX "net.dns"
void __dns_readstartfiles(void) {
#ifdef __BIONIC__
  char propvalue[PROP_VALUE_MAX];
  char propname[PROP_NAME_MAX];
  char propcmd[PROP_NAME_MAX+8];
  char pbuf[256];
  FILE *fp;
  int i;
#endif

  if (_diet_res.nscount>0)
    return;

  _diet_res.options=DIET_RES_RECURSE;

#ifdef __BIONIC__
  for(i = 1; i <= MAX_DNS_PROPERTIES; i++) {
    snprintf(propname, sizeof(propname), "%s%d", DNS_PROP_NAME_PREFIX, i);
    if(__system_property_get(propname, propvalue) < 1) {
      /* attempt to get the property via command instead since the call to Bionic libc failed */
      snprintf(propcmd, sizeof(propcmd), "getprop %s", propname);
      if((fp = popen(propcmd, "r"))) {
        if((fgets(pbuf, sizeof(pbuf)-1, fp) != NULL) && (strlen(pbuf) > 0)) {
          if (pbuf[strlen(pbuf)-1] == '\n')
            pbuf[strlen(pbuf)-1] = '\0'; /* strip trailing newline */
          strncpy(propvalue, pbuf, sizeof(propvalue));
        }
      }
      pclose(fp);
      memset(&propname[0], 0, sizeof(propname)); /* clear propname to avoid unpredictable behavior */
      if(strlen(propvalue) < 1)
        strcpy(propvalue, "8.8.8.8"); /* use a fallback nameserver since we still fail to get the property correctly */
    }

    if (parsesockaddr(propvalue,&_diet_res.nsaddr_list[_diet_res.nscount]))
      if (_diet_res.nscount<DIET_MAXNS)
        ++_diet_res.nscount;
  }
#else
  /* testing */
  parsesockaddr("127.0.0.1",&_diet_res.nsaddr_list[_diet_res.nscount]);
  ++_diet_res.nscount;
#endif
}

/* return length of decoded data or -1 */
int __dns_decodename(const unsigned char *packet,unsigned int offset,unsigned char *dest,
		     unsigned int maxlen,const unsigned char* behindpacket) {
  const unsigned char *tmp;
  const unsigned char *max=dest+maxlen;
  const unsigned char *after=packet+offset;
  int ok=0;
  for (tmp=after; maxlen>0&&*tmp; ) {
    if (tmp>=behindpacket) return -1;
    if ((*tmp>>6)==3) {		/* goofy DNS decompression */
      unsigned int ofs=((unsigned int)(*tmp&0x3f)<<8)|*(tmp+1);
      if (ofs>=(unsigned int)offset) return -1;	/* RFC1035: "pointer to a _prior_ occurrance" */
      if (after<tmp+2) after=tmp+2;
      tmp=packet+ofs;
      ok=0;
    } else {
      unsigned int duh;
      if (dest+*tmp+1>max) return -1;
      if (tmp+*tmp+1>=behindpacket) return -1;
      for (duh=*tmp; duh>0; --duh)
	*dest++=*++tmp;
      *dest++='.'; ok=1;
      ++tmp;
      if (tmp>after) { after=tmp; if (!*tmp) ++after; }
    }
  }
  if (ok) --dest;
  *dest=0;
  return after-packet;
}
