#include "scheduler.h"

Queue *ConstructQueue(int limit) {
    Queue *queue = (Queue*) malloc(sizeof (Queue));
    if (queue == NULL) {
        return NULL;
    }
    if (limit <= 0) {
        limit = 65535;
    }
    queue->limit = limit;
    queue->size = 0;
    queue->head = NULL;
    queue->tail = NULL;

    return queue;
}

void DestructQueue(Queue *queue) {
    NODE *pN;
    while (!isEmpty(queue)) {
        pN = Dequeue(queue);
        free(pN);
    }
    free(queue);
}

int Enqueue(Queue *pQueue, NODE *item) {
    /* Bad parameter */
    if ((pQueue == NULL) || (item == NULL)) {
        return FALSE;
    }
    // if(pQueue->limit != 0)
    if (pQueue->size >= pQueue->limit) {
        return FALSE;
    }
    /*the queue is empty*/
    item->prev = NULL;
    if (pQueue->size == 0) {
        pQueue->head = item;
        pQueue->tail = item;

    } else {
        /*adding item to the end of the queue*/
        pQueue->tail->prev = item;
        pQueue->tail = item;
    }
    pQueue->size++;
    return TRUE;
}

NODE * Dequeue(Queue *pQueue) {
    /*the queue is empty or bad param*/
    NODE *item;
    if (isEmpty(pQueue))
        return NULL;
    item = pQueue->head;
    pQueue->head = (pQueue->head)->prev;
    pQueue->size--;
    return item;
}

int isEmpty(Queue* pQueue) {
    if (pQueue == NULL) {
        return FALSE;
    }
    if (pQueue->size == 0) {
        return TRUE;
    } else {
        return FALSE;
    }
}

// Tests if all programs have ended
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

// Tests if there is only one program executing
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

NODE* nextProcess(Queue* priorProc[], Queue* roundRobin)
{
	int i;
	NODE* curProgNode;
	for(i = 1; i <= 7; i++)
	{
		if(priorProc[i] != NULL && !isEmpty(priorProc[i]))
		{
			return Dequeue(priorProc[i]);
		}
	}
	if(roundRobin != NULL && !isEmpty(roundRobin))
	{
		return Dequeue(roundRobin);
	}
	return NULL;
}

void putProcessQueue(Queue* priorProc[], Queue* roundRobin, int priority[], NODE* progNode)
{
	int curProg = progNode->data.info;
	if(priority[curProg] == -1)
		Enqueue(roundRobin, progNode);
	else
		Enqueue(priorProc[priority[curProg]], progNode);
}

int checkRealTimeConflict(RT* realTime, int qtRT, int ini, int dur)
{
    for(int i = 0; i < qtRT; i++)
    {
        int iniAt = realTime[i].ini;
        int durAt = realTime[i].dur;
        if((iniAt >= ini && iniAt + durAt <= ini + dur) || (ini >= iniAt && ini + dur <= iniAt + durAt))
            return 0;
        if((ini <= iniAt && ini + dur >= iniAt) || (iniAt <= ini && iniAt + durAt >= ini))
            return 0;
    }
    return 1;
}

int checkRealTimeStart(RT* realTime, int qtRT, int qtSec, int finished[])
{
    //printf("qtSecFunction %ld\n", qtSec);
    for(int i = 0; i < qtRT; i++)
    {
        int numProg = realTime[i].numProg;
        //printf("ini %ld\n", realTime[i].ini);
        if(realTime[i].ini == qtSec && !finished[numProg])
        {
            //printf("entrei aqui\n");
            return numProg;
        }
    }
    return -1;
}

void scheduler()
{
	int shmid_prog = shmget(1000, MAX_PROG * TAM * sizeof(char), S_IRUSR | S_IWUSR);
	char* pProg = (char*) shmat(shmid_prog, 0, 0);// array of program's name

	int shmid_priority = shmget(1001, MAX_PROG * sizeof(int), S_IRUSR | S_IWUSR);
	int* pPriority = (int*) shmat(shmid_priority, 0, 0);

	int shmid_iniRT = shmget(1002, MAX_PROG * sizeof(int), S_IRUSR | S_IWUSR);
	int* pIniRT = (int*) shmat(shmid_iniRT, 0, 0);

	int shmid_durationRT = shmget(1003, MAX_PROG * sizeof(int), S_IRUSR | S_IWUSR);
	int* pDurationRT = (int*) shmat(shmid_durationRT, 0, 0);

	int shmid_qtProg = shmget(1004, sizeof(int), S_IRUSR | S_IWUSR);
	int* pQtProg = (int*) shmat(shmid_qtProg, 0, 0);

	int qtdProg = *pQtProg;

	int pid[MAX_PROG], n_pid, i, j, curProg;
	int finished[MAX_PROG];
	int result, status;
	int quantum = 3; // in seconds
	Queue* roundRobin = ConstructQueue(qtdProg);
	Queue* priorityProc[8]; // each queue represents a level of priority
	NODE *curProgNode;
    RT* realTime = (RT*)malloc(sizeof(RT) * qtdProg);
    int countRT = 0; // count how many RT valid processes
    int isRTRunning = 0; // if value is 1 there's a RT process running, otherwise 0
    int procRT, temp;
    struct timeval iniTime;
    struct timeval curTime;
    time_t qtSec;

	for(i = 1; i <= 7; i++)
		priorityProc[i] = ConstructQueue(qtdProg);

	for (i = 0; i < qtdProg; i++)
	{
		finished[i] = 0;
		// ROUND ROBIN
		if(pPriority[i] == -1 && pIniRT[i] == -1 && pDurationRT[i] == -1)
    	{
			curProgNode = (NODE*) malloc(sizeof (NODE));
			curProgNode->data.info = i;
			Enqueue(roundRobin, curProgNode);
		}
		// PRIORITY
		else if(pPriority[i] != -1)
		{

			curProgNode = (NODE*) malloc(sizeof (NODE));
			curProgNode->data.info = i;
			Enqueue(priorityProc[pPriority[i]], curProgNode);
		}
        // REAL TIME
        else
        {
            if(pIniRT[i] + pDurationRT[i] <= 60)
            {
                // Checar se o cara que vou inserir entra em conflito com os já existentes
                if(checkRealTimeConflict(realTime, countRT, pIniRT[i], pDurationRT[i]))
                {
                    realTime[countRT].numProg = i;
                    realTime[countRT].ini = pIniRT[i];
                    realTime[countRT].dur = pDurationRT[i];
                    countRT++;
                }
                else
                {
                    printf("O programa %s entra em conflito com um programa já existente.\n", pProg+i*100);
                    return;
                }
            }
            else
            {
                printf("O programa %s não tem entradas satisfatórias. I+D é maior que 60 segundos.\n", pProg+i*100);
                return;
            }
        }
	}

    // Initializing all process and stopping their execution
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
            execve(pProg+i*100, NULL, NULL);
        }
    }

    gettimeofday(&iniTime, NULL);
    j = 0;
	curProgNode = nextProcess(priorityProc, roundRobin);
    if(curProgNode != NULL)
	   curProg = curProgNode->data.info;
    else
        curProg = -1;
    while(1)
    {

        if(allProgramsFinished(finished, qtdProg))
			break;

        gettimeofday(&curTime, NULL);
        qtSec = curTime.tv_sec - iniTime.tv_sec;
        //sleep(1);
        //printf("qtSec %ld\n", qtSec);
        temp = checkRealTimeStart(realTime, countRT, qtSec, finished);
        if(isRTRunning == 0 && temp >= 0)
        {
            // printf("entrei\n");
            // printf("CurProg %d\n", curProg);
            // printf("ProcRT %d\n", procRT);

            procRT = temp;

            if(curProg != -1)
            {
                kill(pid[curProg], SIGSTOP);
            }

            isRTRunning = 1;
        }
        if(isRTRunning)
        {
            //printf("Entrei RT\n");
            // printf("ProcRT Cont %d\n", procRT);
            kill(pid[procRT], SIGCONT);
            fflush(stdout);
            sleep(1);
            result = waitpid(pid[procRT], &status, WNOHANG);
            //printf("Result: %d\n", result);
            if (result == 0)
            {
                if(pIniRT[procRT] + pDurationRT[procRT] == qtSec)
                {
                    //printf("entrei fim\n");
                    kill(pid[procRT], SIGSTOP);
                    isRTRunning = 0;
                }
            }
            else
            {
                printf("Terminou  %s\n", pProg+procRT*100);
                fflush(stdout);
                finished[procRT] = 1;
                isRTRunning = 0;
            }
        }
        else if(curProg != -1 && !finished[curProg])
        {
			kill(pid[curProg], SIGCONT);
			fflush(stdout);
			sleep(1);
			result = waitpid(pid[curProg], &status, WNOHANG);
			if (result == 0)
			{
			  j++;
			  if(j == quantum)
			  {

				if(!onlyProgram(finished, qtdProg))
				{
					kill(pid[curProg], SIGSTOP);
					putProcessQueue(priorityProc, roundRobin, pPriority, curProgNode);
					curProgNode = nextProcess(priorityProc, roundRobin);
					curProg = curProgNode->data.info;
				}
				j = 0;
			  }
			}
			else
			{
				printf("Terminou  %s\n", pProg+curProg*100);
				fflush(stdout);
				finished[curProg] = 1;
				free(curProgNode);
				curProgNode = nextProcess(priorityProc, roundRobin);
				if(curProgNode != NULL)
				{
					curProg = curProgNode->data.info;
				}
                else
                    curProg = -1;
			  	j = 0;
			}
		}

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
    //     printf("INITIAL: %d\n", pIniRT[i]);
    //     printf("DURATION: %d\n", durationRT[i]);
    // }

	shmdt(pProg);
	shmdt(pPriority);
	shmdt(pIniRT);
	shmdt(pDurationRT);
	shmdt(pQtProg);

	shmctl(shmid_prog, IPC_RMID, 0);
	shmctl(shmid_priority, IPC_RMID, 0);
	shmctl(shmid_iniRT, IPC_RMID, 0);
	shmctl(shmid_durationRT, IPC_RMID, 0);
	shmctl(shmid_qtProg, IPC_RMID, 0);

}
int main()
{
	scheduler();
	return 0;
}

