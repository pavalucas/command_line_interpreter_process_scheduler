#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#define TAM 100
#define MAX_PROG 10

void test2(key_t key)
{
    int segmento;
    char* pMensagem;
    char mensagem[TAM];
    int i;

    segmento = shmget(key, TAM * MAX_PROG * sizeof(char), S_IRUSR | S_IWUSR);
    if(segmento == -1)
    {
        printf("Erro ao alocar espaço em memória.\n");
        exit(-1);
    }


    pMensagem = (char*)shmat(segmento, 0, 0);
    if(pMensagem == -1)
    {
        printf("Erro ao se anexar com segmento já criado.\n");
        exit(-2);
    }

    for(i = 0; i < 3; i++)
    {
        strcpy(mensagem, pMensagem);
        printf("%s\n", pMensagem);
        pMensagem += 100;
    }

    shmdt(pMensagem);
    shmctl (segmento, IPC_RMID, 0);

    return;
}