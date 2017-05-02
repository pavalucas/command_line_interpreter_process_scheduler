#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include "shmgetTest2.c"
#define TAM 100
#define MAX_PROG 10

int main(void)
{
    int segmento;
    int i = 0;
    char* pMensagem;
    char* pMensagemIni;
    char mensagem[TAM];

    FILE* fp;
    char curProg[100]; // name of program being read
    char line[40];
    char* splitLine;
    char* progIni;
    char* prog; // array of program's name
    int len = 40; // max length of line in exec.txt
    int numProg = 0; // number of current program
    int seg1, seg2;
    key_t key1 = 8732; // IPC_PRIVATE does not work apparently
    printf("key: %x\n", key1);

    segmento = shmget(key1, TAM * MAX_PROG * sizeof(char), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    if(segmento == -1)
    {
        printf("Erro ao alocar espaço em memória.\n");
        exit(-1);
    }

    progIni = (char*)shmat(segmento, 0, 0);
    if(prog == -1)
    {
        printf("Erro ao se anexar com segmento já criado.\n");
        exit(-2);
    }


    fp = fopen("exec.txt", "r");
    if(fp == NULL)
    {
        printf("Error! File not found!");
        exit(-3);
    }

    prog = progIni;
    while(fgets(line, len, fp) != NULL)
    {
        splitLine = strtok(line, " =\n");
        splitLine = strtok(NULL, " =\n");
        strcpy(curProg, splitLine);
        printf("Cur prog: %s\n", curProg);
        for(i = 0; i < TAM; i++)
        {
            *prog = curProg[i];
            prog++;
        }
    }

    shmdt(progIni);

    test2(key1);

    return 0;
}