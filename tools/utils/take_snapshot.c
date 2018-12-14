#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    FILE *fd;
    fd = fopen("/dev/shm/rtspd_snapshot", "w");

    if(!fd)
        return 1;

    fprintf(fd, "snap\n");
    fclose(fd);
    printf("Snapshot created in /tmp/sd/RECORDED_IMAGES\n");

    return 0;
}
