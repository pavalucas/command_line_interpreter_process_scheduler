#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    int i = 0;
    for(i = 0; i < 4; i++)
    {
        printf("P4 rodando %d\n", i);
        fflush(stdout);
        sleep(1);
    }
}