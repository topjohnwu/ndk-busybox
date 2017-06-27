#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int reboot_android(int argc, char *argv[])
{
    int ret;

    const char* restart_cmd = NULL;
    char* prop_value;
    char* prop_cmd;

    restart_cmd = "reboot";
    while (true) {
        int c = getopt(argc, argv, "p");
        if (c == EOF)
            break;
        switch (c) {
        case 'p':
            restart_cmd = "shutdown";
            break;
        }
    }

    if (argc > optind)
        asprintf(&prop_value, "%s,%s", restart_cmd, argv[optind]);
    else
        asprintf(&prop_value, "%s", restart_cmd);

    asprintf(&prop_cmd, "setprop sys.powerctl %s", prop_value);
    ret = execlp("sh", "sh", "-c", prop_cmd, NULL);
    return ret;
}
