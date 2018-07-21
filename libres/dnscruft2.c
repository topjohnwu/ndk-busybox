#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include "dietdns.h"

/* needs:
 * __dns_decodename
 * res_query
 */

extern int __dns_decodename(unsigned char *packet,unsigned int offset,unsigned char *dest,
			    unsigned int maxlen,unsigned char* behindpacket);

/* Oh boy, this interface sucks so badly, there are no words for it.
 * Not one, not two, but _three_ error signalling methods!  (*h_errnop
 * nonzero?  return value nonzero?  *RESULT zero?)  The glibc goons
 * really outdid themselves with this one. */
int __dns_gethostbyx_r(const char* name, struct hostent* result,
			char *buf, size_t buflen,
			struct hostent **RESULT, int *h_errnop, int lookfor) {
  int names,ips;
  char *cur;
  char *max;
  char inpkg[1500];
  char* tmp;
  int size;

  if (lookfor==1) {
    result->h_addrtype=AF_INET;
    result->h_length=4;
  } else {
    result->h_addrtype=AF_INET6;
    result->h_length=16;
  }
  result->h_aliases=(char**)(buf+8*sizeof(char*));
  result->h_addr_list=(char**)buf;
  result->h_aliases[0]=0;

  cur=buf+16*sizeof(char*);
  max=buf+buflen;
  names=ips=0;

  if ((size=diet_res_query(name,DIET_C_IN,lookfor,(unsigned char*)inpkg,512))<0) {
invalidpacket:
    *h_errnop=HOST_NOT_FOUND;
    return -1;
  }
  {
    tmp=inpkg+12;
    {
      unsigned char Name[257];
      unsigned short q=((unsigned short)inpkg[4]<<8)+inpkg[5];
      while (q>0) {
	if (tmp>(char*)inpkg+size) goto invalidpacket;
	while (*tmp) { tmp+=*tmp+1; if (tmp>(char*)inpkg+size) goto invalidpacket; }
	tmp+=5;
	--q;
      }
      if (tmp>inpkg+size) goto invalidpacket;
      q=((unsigned short)inpkg[6]<<8)+inpkg[7];
      if (q<1) goto nodata;
      while (q>0) {
	int decofs=__dns_decodename((unsigned char*)inpkg,(size_t)(tmp-(char*)inpkg),Name,256,(unsigned char*)inpkg+size);
	if (decofs<0) break;
	tmp=inpkg+decofs;
	--q;
	if (tmp[0]!=0 || tmp[1]!=lookfor ||	/* TYPE != A */
	    tmp[2]!=0 || tmp[3]!=1) {		/* CLASS != IN */
	  if (tmp[1]==5) {	/* CNAME */
	    tmp+=10;
	    decofs=__dns_decodename((unsigned char*)inpkg,(size_t)(tmp-(char*)inpkg),Name,256,(unsigned char*)inpkg+size);
	    if (decofs<0) break;
	    tmp=inpkg+decofs;
	  } else
	    break;
	  continue;
	}
	tmp+=10;	/* skip type, class, TTL and length */
	{
	  int slen;
	  if (lookfor==1 || lookfor==28) /* A or AAAA*/ {
	    slen=strlen((char*)Name);
	    if (cur+slen+8+(lookfor==28?12:0)>=max) { *h_errnop=NO_RECOVERY; return -1; }
	  } else {
	    if (lookfor==12) /* PTR */ {
	      decofs=__dns_decodename((unsigned char*)inpkg,(size_t)(tmp-(char*)inpkg),Name,256,(unsigned char*)inpkg+size);
	      if (decofs<0) break;
	      tmp=inpkg+decofs;
	    }
	    slen=strlen((char*)Name);
	  }
	  strcpy(cur,(char*)Name);
	  if (names==0)
	    result->h_name=cur;
	  else
	    result->h_aliases[names-1]=cur;
	  result->h_aliases[names]=0;
	  if (names<8) ++names;
/*		cur+=slen+1; */
	  cur+=(slen|3)+1;
	  result->h_addr_list[ips++] = cur;
	  if (lookfor==1) /* A */ {
	    memcpy(cur,tmp,4);
	    cur+=4; tmp+=4;
	    result->h_addr_list[ips]=0;
	  } else if (lookfor==28) /* AAAA */ {
	    memcpy(cur,tmp,16);
	    cur+=16; tmp+=16;
	    result->h_addr_list[ips]=0;
	  }
	}
/*	      puts(Name); */
      }
    }
  }
  if (!names) {
nodata:
    *h_errnop=NO_DATA;
    return -1;
  }
  *h_errnop=0;
  *RESULT=result;
  return 0;
}
