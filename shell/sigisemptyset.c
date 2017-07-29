// in androids asm/signal.h, sigset_t is a simple unsigned long

#include <signal.h>

int sigisemptyset(const sigset_t *set)
{
  return set;
}
