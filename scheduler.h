#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#define MAX_PROG 10
#define TAM 100
#define TRUE  1
#define FALSE   0

/* a link in the queue, holds the info and point to the next Node*/
typedef struct {
    int info;
} DATA;

typedef struct Node_t {
    DATA data;
    struct Node_t *prev;
} NODE;

/* the HEAD of the Queue, hold the amount of node's that are in the queue*/
typedef struct Queue {
    NODE *head;
    NODE *tail;
    int size;
    int limit;
} Queue;

// Queue implementation for round robin
Queue *ConstructQueue(int limit);
void DestructQueue(Queue *queue);
int Enqueue(Queue *pQueue, NODE *item);
NODE *Dequeue(Queue *pQueue);
int isEmpty(Queue* pQueue);

struct heap {
	int size;
	int count;
	int *heaparr;
};

void heap_init(struct heap *h);
void max_heapify(int *data, int loc, int count);
void heap_push(struct heap *h, int value);
void heap_display(struct heap *h);
int heap_delete(struct heap *h);
int emptyPQ(struct heap *pq);

void scheduler(char prog[][TAM], int priority[], int iniRT[], int durationRT[], int qtdProgs);