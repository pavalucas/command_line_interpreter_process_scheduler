#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#define MAX_PROG 10
#define TAM 100
#define TRUE  1
#define FALSE   0

typedef struct {
    int numProg;
    time_t ini;
    time_t dur;
} RT;

// Queue implementation
typedef struct {
    int info;
} DATA;

typedef struct Node_t {
    DATA data;
    struct Node_t *prev;
} NODE;

typedef struct Queue {
    NODE *head;
    NODE *tail;
    int size;
    int limit;
} Queue;

Queue *ConstructQueue(int limit);
void DestructQueue(Queue *queue);
int Enqueue(Queue *pQueue, NODE *item);
NODE *Dequeue(Queue *pQueue);
int isEmpty(Queue* pQueue);

void scheduler();