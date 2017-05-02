#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#define MAX_PROG 10
#define TAM 100

void scheduler(char prog[][TAM], int priority[], int iniRT[], int durationRT[], int qtdProgs);