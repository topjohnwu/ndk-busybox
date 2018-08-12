/* Get the system-specific definition of `struct statfs'.  */
#include <sys/vfs.h>

/* Hack for Android API <21 since it does not include sys/statvfs.h */
#define statvfs statfs
