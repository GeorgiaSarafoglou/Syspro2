#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"queue.h"

void InitQueue(queue** head, int maxsize){
    (*head) = (queue*)malloc(sizeof(queue));
    (*head)->next = NULL;
    (*head)->maxsize = maxsize;
    (*head)->count = 0;
    return;
}

void InsertQueue(queue** head, char* filename, int socket, int size){

    //create new queue node.
    (*head)->count++;
    queue_node* new = (queue_node*)malloc(sizeof(queue_node));
    new->filename = (char*)malloc(strlen(filename));
    new->socket = socket;
    new->size = size;
    strcpy(new->filename,filename);
    new->next = NULL;

    //attach new node to end of queue.
    queue_node* current = (*head)->next;
    if(current == NULL){
        (*head)->next = new;
        return;
    }

    while(current->next != NULL){
        current = current->next;
    }
    current->next = new;

    return;
}

char* PopQueue(queue** head, int* socket, int* size){
    queue_node* temp = (*head)->next;
    (*head)->next = temp->next;
    char* filename =(char*)malloc(strlen(temp->filename)* sizeof(char));
    strcpy(filename, temp->filename);
    *socket = temp->socket;
    *size = temp->size;
    free(temp->filename);
    free(temp);
    (*head)->count--;
    return filename;
}

void DeleteQueue(queue** head){
    queue_node* current = (*head)->next;
    while(current != NULL){
        queue_node* temp = current;
        current = current->next;
        free(temp->filename);
        free(temp);
    }
}

int IsEmpty(queue* head){
    if(head->count == 0){
        return 1;
    }else{
        return 0;
    }
}

int IsFull(queue* head){
    if(head->count == head->maxsize){
        return 1;
    }else{
        return 0;
    }
}
