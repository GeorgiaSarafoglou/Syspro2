#include <stdio.h>
#include <sys/wait.h> /* sockets */
#include <sys/types.h> /* sockets */
#include <sys/socket.h> /* sockets */
#include <netinet/in.h> /* internet sockets */
#include <netdb.h> /* ge th os tb ya dd r */
#include <unistd.h> /* fork */
#include <stdlib.h> /* exit */
#include <ctype.h> /* toupper */
#include <signal.h> /* signal */
#include <pthread.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include "queue.h"

void *communication_thread(void*);
void *worker_thread(void*);
void perror_exit(char *);
void readdirect(char*, int);
void countfiles(char*, int *);

//mutexes and condition variables.
pthread_mutex_t Qmutex;     //mutex for queue
pthread_mutex_t client_th;  //mutex for client socket
pthread_cond_t Qcond;       //condition variable for queue

queue *Q;           //global queue.

void main(int argc, char *argv[]){

    //initialize mutexes and condition variables
    pthread_mutex_init(&Qmutex, NULL);
    pthread_cond_init(&Qcond, NULL);
    pthread_mutex_init(&client_th, NULL);
    int port,sock,newsock,thread_pool_size,queue_size,block_size;
    struct sockaddr_in server, client;
    socklen_t clientlen;

    struct sockaddr *serverptr=(struct sockaddr*)&server;
    struct sockaddr *clientptr=(struct sockaddr*)&client;
    struct hostent *rem;

    //command line arguments.
    for(int i=1;i<=7;i+=2){
        if(strcmp(argv[i],"-p")==0){
            port = atoi(argv[i+1]);
        }else if(strcmp(argv[i],"-s")==0){
            thread_pool_size = atoi(argv[i+1]);
        }else if(strcmp(argv[i],"-q")==0){
            queue_size = atoi(argv[i+1]);
        }else if(strcmp(argv[i],"-b")==0){
            block_size = atoi(argv[i+1]);
        }
    }

    
    //initialize queue of size queue_size.
    InitQueue(&Q, queue_size);
    
    //create worker thread pool.
    pthread_t workers[thread_pool_size];
    //create worker threads.
    for(int i=0; i<thread_pool_size; i++){
        if (pthread_create(&workers[i], NULL, worker_thread, &block_size) != 0){
            perror_exit("worker thread");
        }
    }

    //Create socket 
    if((sock = socket(AF_INET, SOCK_STREAM, 0))<0){
        perror_exit("socket");
    }

    server.sin_family = AF_INET; /* Internet domain */
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port); /* The given port */
    /* Bind socket to address */
    if (bind(sock, serverptr, sizeof(server)) < 0){
        perror_exit("bind") ;
    }

    //Listen for connections
    
    if(listen(sock, 10) < 0){
        perror_exit("listen");
    }
    printf("Listening for connections to port %d\n", port);

    while(1){
        //accept connection.
        if((newsock = accept(sock, clientptr, &clientlen)) < 0){
            perror_exit("accept");
        }
    
       printf("Accepted connection\n");
        //create communication thread for serving client.
        pthread_t c_th;
        int* client_socket = malloc(sizeof(int));
        client_socket = &newsock;
        //pass client socket to communication thread function.
        pthread_create(&c_th, NULL, &communication_thread, client_socket);
    }
}

void *communication_thread(void* newsock){
    char buf[524];
    int count = 0;
    //read directory from client.
    if(read(*(int*)newsock, buf, 524) < 0){
       perror_exit("server didnt read directory");
    }
    printf("directory read: %s\n",buf);
    //count number of files client needs to read.
    countfiles(buf,&count);
    int convert = htonl(count);
    write(*(int*)newsock,&convert,sizeof(convert));
    //read files from directory received and insert them to queue.
    readdirect(buf,*(int*)newsock);
}


void perror_exit(char *message){
    perror(message);
    exit(EXIT_FAILURE);
}

/*
read directory contents recursively and insert them to queue if there is space.
*/
void readdirect(char* dir, int socket){
    struct stat st;
    DIR *directory;
    struct dirent *file;

    if((directory = opendir(dir)) == 0){
        printf("no directory\n");
        return;
    }

    //read recursively files.
    while((file = readdir(directory)) != NULL){
        if(file->d_type == DT_DIR){
            //file we read was a directory
            if(strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0){
                continue;
            }
            char newdir[524];           //path of new directory.
            strcpy(newdir, dir);
            strcat(newdir, "/");
            strcat(newdir, file->d_name);
            readdirect(newdir,socket);         //call function again for new directory.
        }else{
            //print file name
            char* filename = malloc(strlen(dir) + 1 + strlen(file->d_name));
            strcpy(filename,dir);
            strcat(filename,"/");
            strcat(filename, file->d_name);
           // printf("file name: %s\n", filename);
            //add to queue if there is space.
            pthread_mutex_lock(&Qmutex);
            while(IsFull(Q)){
                //if Queue is full wait.
                printf("Waiting for Queue to empty...\n");
                sleep(1);
                pthread_cond_wait(&Qcond, &Qmutex);
            }
            //when queue has space insert filename.
            stat(filename, &st);                //get the size of the file.
            printf("Thread %ld: adding file %s to the queue\n",pthread_self(), filename);
            InsertQueue(&Q,filename,socket,(int)st.st_size);
            pthread_cond_signal(&Qcond);        //signal worker thread that a file was inserted to Queue.
            pthread_mutex_unlock(&Qmutex);
            
            free(filename);
        }
    }
    closedir(directory);
    return;
}

void countfiles(char* dir,int* count){
    struct stat st;
    DIR *directory;
    struct dirent *file;

    if((directory = opendir(dir)) == 0){
        printf("no directory\n");
        return;
    }

    //read recursively files.
    while((file = readdir(directory)) != NULL){
        if(file->d_type == DT_DIR){
            //file we read was a directory
            if(strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0){
                continue;
            }
            char newdir[524];           //path of new directory.
            strcpy(newdir, dir);
            strcat(newdir, "/");
            strcat(newdir, file->d_name);
            countfiles(newdir, count);        //call function again for new directory.
        }else{
           (*count)++;
        }
    }
    closedir(directory);
    return;
}

void* worker_thread(void* arg){
    int socketnum;
    int block_size = *(int*)arg;
    int size,num;
    char buffer[block_size];
    char buf[3];
    while(1){
        pthread_mutex_lock(&Qmutex);
        while(IsEmpty(Q)){
            //if queue is empty then wait.
            printf("Waiting for file...\n");
            sleep(1);
            pthread_cond_wait(&Qcond, &Qmutex);
        }
        char *filename = PopQueue(&Q, &socketnum, &size);
        

        num = htonl(size);

        int length = strlen(filename);
        length = htonl(length);
        
        printf("Thread %ld: Received task: %s\n",pthread_self(), filename);
        pthread_cond_signal(&Qcond);        //signal communication thread that queue has space now.
        pthread_mutex_unlock(&Qmutex);

        //file pointer for reading file.
        FILE *fp;

        fp = fopen(filename,"r");
        if(fp == NULL){
            perror_exit("opening file");
        }
        //write to client socket the filename in path form.
        //lock client mutex so only one thread writes to socket at a time.
        pthread_mutex_lock(&client_th);
        printf("Thread %ld: writing %s to socket %d\n",pthread_self(), filename, socketnum);
        write(socketnum, &length, sizeof(length));
        if(write(socketnum, filename, strlen(filename)) < 0){
            perror_exit("write from worker");
        }
        //write file contents
        write(socketnum, &num,sizeof(num));
        while(fgets(buffer,block_size,fp)!=NULL){
            if(write(socketnum,buffer, strlen(buffer))<0){
                perror_exit("write from worker");
            }
        }
        fclose(fp);
        read(socketnum,buf,3);
        if(strcmp(buf,"end")==0){
            printf("closing connection\n");
            close(socketnum);
        }
        pthread_mutex_unlock(&client_th);

        free(filename);
    }
}





