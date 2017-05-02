#include "interpreter.h"
#include "scheduler.h"

void readExec()
{
    FILE* fp;
    int len = 40; // max length of line in exec.txt
    char line[40];
    char* splitLine;
    char prog[MAX_PROG][TAM]; // array of program's name
    int priority[MAX_PROG], iniRT[MAX_PROG], durationRT[MAX_PROG];
    char* progIni; // pointer to the initial position of array returned by shmat
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
        strcpy(prog[numProg], splitLine);
        splitLine = strtok(NULL, " =\n");
        priority[numProg] = -1;
        iniRT[numProg] = -1;
        durationRT[numProg] = -1;
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
                priority[numProg] = priorityNumber;
            }
            else
            {
                int init, duration;
                splitLine = strtok(NULL, " =\n");
                init = strtol(splitLine, &splitLine, 10);

                splitLine = strtok(NULL, " =\n");
                splitLine = strtok(NULL, " =\n");
                duration = strtol(splitLine, &splitLine, 10);

                iniRT[numProg] = init;
                durationRT[numProg] = duration;
            }
        }
        numProg++;
    }

    scheduler(prog, priority, iniRT, durationRT, numProg);

    fclose(fp);
}