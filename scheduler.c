#include "scheduler.h"

int index_prog_atual;
int pid_atual;


// Testa se todos programas ja terminaram
int allProgramsFinished(int finished[],int n)
{
    int i;
    for(i = 0; i < n ; i++)
    {
        if(finished[i] == 0)
            return 0;
    }

    return 1;
}

// Testa se tem apenas um unico programa rodando
int onlyProgram(int finished[], int n)
{
    int i;
    int progs = 0;
    for(i = 0; i < n ; i++)
    {
        if(finished[i] == 0)
            progs++;

        if(progs > 1)
            return 0;
    }

    return 1;

}

void scheduler(char prog[][TAM], int priority[], int iniRT[], int durationRT[], int qtdProg)
{

    // if(priority[i] == -1 && iniRT[i] == -1 && durationRT[i] == -1)
    // {
        // ROUND ROBIN
        int i, pid[MAX_PROG], n_pid, j;
        int finished[MAX_PROG];
        int result, status;
        int quantum = 4;
        for(i = 0; i < qtdProg; i++)
            finished[i] = 0;

        for(i = 0; i < qtdProg; i++)
        {
            n_pid = fork();
            if(n_pid != 0)
            {
                pid[i] = n_pid;
                kill(n_pid, SIGSTOP);
            }
            else
            {
                sleep(1);
                execve(prog[i], NULL, NULL);
            }
        }

        i = j = 0;
        while(1)
        {
            if(allProgramsFinished(finished, qtdProg))
                    break;
            if(!finished[i])
            {
                kill(pid[i], SIGCONT);
                fflush(stdout);
                pid_atual = pid[i];
                sleep(1);
                result = waitpid(pid[i], &status, WNOHANG);
                if (result == 0)
                {
                  j++;
                  if(j == quantum)
                  {

                    if(!onlyProgram(finished, qtdProg))
                    {
                        kill(pid[i], SIGSTOP);
                        i++;
                    }
                    j = 0;
                  }

                }
                else
                {
                  printf("Terminou  P%d\n", i+1);
                  fflush(stdout);
                  finished[i] =1;
                  j = 0;
                  i++;
                }
            }else{
                i++;
            }

            i %= qtdProg;

            fflush(stdout);
        }
    //}
    // else if(priority[i] != -1)
    // {
    //     printf("PRIORITY %d\n", priority[i]);
    // }
    // else
    // {
    //     printf("REAL TIME\n");
    //     printf("INITIAL: %d\n", iniRT[i]);
    //     printf("DURATION: %d\n", durationRT[i]);
    // }
}