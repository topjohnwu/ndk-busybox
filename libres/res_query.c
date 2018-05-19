#include <resolv.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/nameser.h>
#include <fcntl.h>
#include <sys/time.h>
#include "dietdns.h"

/* needs:
 * __dns_fd
 * __dns_make_fd
 * __dns_readstartfiles
 * h_errno
 * diet_res_mkquery
 * _res
 */

extern void __dns_make_fd(void);
extern int __dns_fd;

extern void __dns_readstartfiles(void);

int diet_res_query(const char *dname, int class, int type, unsigned char *answer, int anslen) {
  unsigned char packet[512];
  int size;
  struct pollfd duh[2];
  __dns_make_fd();

  __dns_readstartfiles();
  if ((size=diet_res_mkquery(DIET_QUERY,dname,class,type,0,0,0,(char*)packet,512))<0) { diet_h_errno=NO_RECOVERY; return -1; }
  {
    {
      int i;	/* current server */
      int j;	/* timeout count down */
      struct timeval last,now;

      i=0;
      duh[0].events=POLLIN;
      duh[0].fd=0;
      last.tv_sec=0;
      for (j=20; j>0; --j) {
	gettimeofday(&now,0);
	if (now.tv_sec-last.tv_sec>10) {
	  duh[0].fd=__dns_fd;
	  sendto(__dns_fd,packet,size,0,(struct sockaddr*)&(_diet_res.nsaddr_list[i]),sizeof(struct sockaddr));
	  last=now;
	}
	if (++i >= _diet_res.nscount) i=0;
	if (poll(duh,1,1000) == 1) {
	  /* read and parse answer */
	  unsigned char inpkg[1500];
	  int len=read(duh[0].fd,inpkg,sizeof(inpkg));
	  /* header, question, answer, authority, additional */
	  if (inpkg[0]!=packet[0] || inpkg[1]!=packet[1]) continue;	/* wrong ID */
	  if ((inpkg[2]&0xf9) != (_diet_res.options&DIET_RES_RECURSE?0x81:0x80)) continue;	/* not answer */
	  if ((inpkg[3]&0x0f) != 0) {
	    diet_h_errno=HOST_NOT_FOUND;
	    return -1;
	  }		/* error */
	  if (len>anslen) {
	    diet_h_errno=NO_RECOVERY;
	    return -1;
	  }
	  memcpy(answer,inpkg,len);
	  return len;
	}
/*kaputt:*/
      }
    }
  }
  diet_h_errno=TRY_AGAIN;
  return -1;
}
