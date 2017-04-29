void readExec()
{
    FILE* fp;
    fp = fopen("exec.txt", "r");
    if(fp == NULL)
    {
        printf("Error! File not found!");
        exit(-1);
    }
}