#include "interpreter.h"
#include "scheduler.h"

void readExec()
{
	int shmid_prog = shmget(1000, MAX_PROG * TAM * sizeof(char), IPC_CREAT | S_IRUSR | S_IWUSR);
	char* pProg = (char*) shmat(shmid_prog, 0, 0);// array of program's name

	int shmid_priority = shmget(1001, MAX_PROG * sizeof(int), IPC_CREAT | S_IRUSR | S_IWUSR);
	int* pPriority = (int*) shmat(shmid_priority, 0, 0);

	int shmid_iniRT = shmget(1002, MAX_PROG * sizeof(int), IPC_CREAT | S_IRUSR | S_IWUSR);
	int* pIniRT = (int*) shmat(shmid_iniRT, 0, 0);
	
	int shmid_durationRT = shmget(1003, MAX_PROG * sizeof(int), IPC_CREAT | S_IRUSR | S_IWUSR);
	int* pDurationRT = (int*) shmat(shmid_durationRT, 0, 0);
	
	int shmid_numProg = shmget(1004, sizeof(int), IPC_CREAT | S_IRUSR | S_IWUSR);
	int* pNumProg = (int*) shmat(shmid_numProg, 0, 0);
	
	
    FILE* fp;
    int len = 40; // max length of line in exec.txt
    char line[40];
    char* splitLine;
    int numProg = 0; // number of current program

    fp = fopen("exec.txt", "r");
    if(fp == NULL)
    {
        printf("Error! File not found!");
        exit(-1);
    }

    while(fgets(line, len, fp) != NULL)
    {
		
        splitLine = strtok(line, " =\n");
        splitLine = strtok(NULL, " =\n");
        strcpy(pProg+numProg*TAM, splitLine);
        splitLine = strtok(NULL, " =\n");
        pPriority[numProg] = -1;
        pIniRT[numProg] = -1;
        pDurationRT[numProg] = -1;
        if (splitLine == NULL)
        {
        }
        else
        {
            if(splitLine[0] == 'P')
            {
                int priorityNumber;
                splitLine = strtok(NULL, " =\n");
                priorityNumber = strtol(splitLine, &splitLine, 10);
                if(priorityNumber < 1 || priorityNumber > 7)
                {
                    printf("Insert a priority number between 1 and 7\n");
                    continue;
                }
                pPriority[numProg] = priorityNumber;
            }
            else
            {
                int init, duration;
                splitLine = strtok(NULL, " =\n");
                init = strtol(splitLine, &splitLine, 10);

                splitLine = strtok(NULL, " =\n");
                splitLine = strtok(NULL, " =\n");
                duration = strtol(splitLine, &splitLine, 10);

                pIniRT[numProg] = init;
                pDurationRT[numProg] = duration;
            }
        }
        numProg++;
    }
    
    *pNumProg = numProg;

    fclose(fp);
    
    shmdt(pProg);
    shmdt(pPriority);
    shmdt(pIniRT);
    shmdt(pDurationRT);
    shmdt(pNumProg);
}

int main()
{
	readExec();
	return 0;
}

