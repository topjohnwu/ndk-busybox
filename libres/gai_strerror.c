#include <sys/socket.h>
#include <netdb.h>

const char* diet_gai_strerror(int error) {
  switch (error) {
  case EAI_FAMILY: return "family not supported";
  case EAI_SOCKTYPE: return "socket type not supported";
  case EAI_NONAME: return "unknown host";
  case EAI_SERVICE: return "unknown service";
  case EAI_MEMORY: return "memory allocation failure";
  case EAI_AGAIN: return "temporary failure";
  }
  return "DNS error. Sorry.";
}
