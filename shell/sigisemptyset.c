#include <signal.h>

int sigisemptyset(const sigset_t *set)
{
    sigset_t empty;
    sigemptyset(&empty);
    return memcmp(&empty, set, sizeof(sigset_t)) == 0;
}
