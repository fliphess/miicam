#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>


int main(int argc, char *argv[])
{
    FILE *fd;
    fd = fopen("/dev/shm/rtspd_snapshot", "w");

    if(!fd)
        return 1;

    fprintf(fd, "snap\n");
    fclose(fd);

    usleep(800);

    if (access("/dev/shm/rtspd_snapshot", F_OK ) == -1 ) {
        char dirstring[40];
        struct tm *sTm;
        time_t now = time(0);
        sTm = gmtime(&now);
        strftime(dirstring, sizeof(dirstring), "/tmp/sd/RECORDED_IMAGES/%Y/%m/%d", sTm);
        printf("Snapshot created in %s\n", dirstring);
    }
    else {
        printf("ERROR: No snapshot created! Is rtspd running?\n");
    }

    return 0;
}

