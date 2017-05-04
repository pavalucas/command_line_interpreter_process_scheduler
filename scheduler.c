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

void heap_init(struct heap *h)
{
    h->count = 0;
    h->size = 1;
    h->heaparr = (int *) malloc(sizeof(int));
    if(!h->heaparr) {
        printf("Error allocatinga memory...\n");
        exit(-1);
    }

}

void max_heapify(int *data, int loc, int count) {
    int left, right, largest, temp;
    left = 2*(loc) + 1;
    right = left + 1;
    largest = loc;
    

    if (left <= count && data[left] > data[largest]) {
        largest = left;
    } 
    if (right <= count && data[right] > data[largest]) {
        largest = right;
    } 
    
    if(largest != loc) {
        temp = data[loc];
        data[loc] = data[largest];
        data[largest] = temp;
        max_heapify(data, largest, count);
    }

}

void heap_push(struct heap *h, int value)
{
    int index, parent;
 
    // Resize the heap if it is too small to hold all the data
    if (h->count == h->size)
    {
        h->size += 1;
        h->heaparr = realloc(h->heaparr, sizeof(int) * h->size);
        if (!h->heaparr) exit(-1); // Exit if the memory allocation fails
    }
    
    index = h->count++; // First insert at last of array

    // Find out where to put the element and put it
    for(;index; index = parent)
    {
        parent = (index - 1) / 2;
        if (h->heaparr[parent] >= value) break;
        h->heaparr[index] = h->heaparr[parent];
    }
    h->heaparr[index] = value;
}

void heap_display(struct heap *h) {
    int i;
    for(i=0; i<h->count; ++i) {
        printf("|%d|", h->heaparr[i]);
    }
    printf("\n");
}

int heap_delete(struct heap *h)
{
    int removed;
    int temp = h->heaparr[--h->count];
    
    
    if ((h->count <= (h->size + 2)) && (h->size > 0))
    {
        h->size -= 1;
        h->heaparr = realloc(h->heaparr, sizeof(int) * h->size);
        if (!h->heaparr) exit(-1); // Exit if the memory allocation fails
    }
    removed = h->heaparr[0];
    h->heaparr[0] = temp;
    max_heapify(h->heaparr, 0, h->count);
    return removed;
}


int emptyPQ(struct heap *pq) {
    int i;
    while(pq->count != 0) {
        printf("<<%d", heap_delete(pq));
    }
}

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
	int quantum = 3;
	Queue* roundRobin = ConstructQueue(qtdProg);
	Queue* priorityProc[7];
	NODE *curProgNode;

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
	}

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

    j = 0;
	curProgNode = nextProcess(priorityProc, roundRobin);
	curProg = curProgNode->data.info;
    while(1)
    {

        if(allProgramsFinished(finished, qtdProg))
			break;
        if(!finished[curProg])
        {
			kill(pid[curProg], SIGCONT);
			fflush(stdout);
			pid_atual = pid[curProg];
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
				printf("Terminou  P%d\n", curProg+1);
				fflush(stdout);
				finished[curProg] =1;
				free(curProgNode);
				curProgNode = nextProcess(priorityProc, roundRobin);
				if(curProgNode != NULL)
				{
					curProg = curProgNode->data.info;
				}
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

