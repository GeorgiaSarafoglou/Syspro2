typedef struct queue_node{
    char* filename;
    int socket;
    int size;
    struct queue_node* next;
}queue_node;

typedef struct queue{
    int maxsize;
    int count;
    struct queue_node* next;
}queue;

void InitQueue(queue**, int);

void InsertQueue(queue**, char*, int, int);


/*
returns filename from the first queue node and socket number.
filename needs to be freed.
*/
char* PopQueue(queue**, int*, int*);

void DeleteQueue(queue**);

/*
Returns 1 if queue is empty and 0 if it is not.
*/
int IsEmpty(queue*);


/*
returns 1 if queueu is full and 0 if it is not.
*/
int IsFull(queue*);