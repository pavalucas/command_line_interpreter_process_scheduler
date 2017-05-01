#include "interpreter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void readExec()
{
    FILE* fp;
    char ex[5], prog[100], param[20], param2[20];
    char line[40];
    char* splitLine;
    int len = 40;
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
        strcpy(prog, splitLine);
        splitLine = strtok(NULL, " =\n");
        if (splitLine == NULL)
        {
            printf("Program's name: %s\n", prog);
            printf("ROUND ROBIN\n");
        }
        else
        {
            if(splitLine[0] == 'P')
            {
                printf("Program's name: %s\n", prog);
                int priorityNumber;
                splitLine = strtok(NULL, " =\n");
                priorityNumber = strtol(splitLine, &splitLine, 10);
                printf("PRIORITY %d\n", priorityNumber);
                if(priorityNumber < 1 || priorityNumber > 7)
                {
                    printf("Insert a priority number between 1 and 7\n");
                    continue;
                }
            }
            else
            {
                printf("Program's name: %s\n", prog);
                int init, duration;
                printf("REAL TIME\n");
                splitLine = strtok(NULL, " =\n");
                init = strtol(splitLine, &splitLine, 10);
                printf("INITIAL: %d\n", init);

                splitLine = strtok(NULL, " =\n");
                splitLine = strtok(NULL, " =\n");
                duration = strtol(splitLine, &splitLine, 10);
                printf("DURATION: %d\n", duration);
            }
        }
    }

    fclose(fp);
}