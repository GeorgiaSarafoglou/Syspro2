#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>


void perror_exit(char* message);
void makefile(char *);


void main(int argc, char* argv[]){
    int port;
    int sock;
    int i;
    char* ip;
    char* directory;
    char folder[524];
    char temp[524];
    char* foldertoken;

    struct sockaddr_in server;
    struct sockaddr *serverptr = (struct sockaddr*)&server;
    struct hostent *rem;


    //command line arguments.
    for(i=1;i<=5;i+=2){
        if(strcmp(argv[i],"-i")==0){
            ip = (char*)malloc(sizeof(char)*strlen(argv[i+1])+1);
            strcpy(ip,argv[i+1]);
        }else if(strcmp(argv[i],"-p")==0){
            port = atoi(argv[i+1]);
        }else if(strcmp(argv[i],"-d")==0){
            directory = (char*)malloc(sizeof(char)*strlen(argv[i+1])+1);
            strcpy(directory,argv[i+1]);
        }   
    }
    //split folder from server path.
    strcpy(temp,directory);
    foldertoken = strtok(temp,"/");
    while(foldertoken!=NULL){
        strcpy(folder,"/");
        strcat(folder,foldertoken);
        foldertoken = strtok(NULL, "/");
    }
    //folder for copy is folder.

    //create socket
    if ((sock = socket(AF_INET , SOCK_STREAM , 0)) < 0){
        perror_exit("socket");
    }
    //find server address
    if((rem = gethostbyname(ip)) == NULL){
        herror("gethostbyname");
        exit(1);
    }
    server.sin_family = AF_INET;
    memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
    server.sin_port = htons(port);
    //initiate connection
    printf("try to connect\n");
    if(connect(sock,serverptr,sizeof(server)) < 0){
        perror_exit("connect");
    }
    printf("Connecting to %s port %d\n", argv[1], port);

    //write to socket the path that was given.
    if(write(sock, directory, strlen(directory)) < 0){
       perror_exit("write");
    }
    //read number of files.
    int count = 0;
    if(read(sock,&count,sizeof(count))<0){
        perror_exit("read count");
    }
    count = ntohl(count);
    int size, bytes;
    int len;     //stores length of filename and its size.
    for(int k = 0; k<count;k++){
        read(sock,&len,sizeof(len));
        len = ntohl(len);
        //read len number of bytes.
     
        char* file = malloc(sizeof(char)* len +1);
    
        read(sock, file, len);
        *(file+len)='\0';
        printf("Received: %s\n",file);
        //read file size.
        read(sock,&size,sizeof(size));
        size = ntohl(size);

        //remove server path and add client output path.
        char* tempfilename = strstr(file, folder);
    
        char* filename = (char*)malloc(sizeof(char)*(strlen(tempfilename)+strlen("./output")+1));
        strcpy(filename, "./output");
        strcat(filename, tempfilename);

        makefile(filename);
        
        //read contents of file.
        bytes = 0;
        char* buf = (char*)malloc(512);
        char* contents = (char*)malloc(sizeof(char)*size+1);
        contents[0]='\0';
        do{
            bytes += read(sock,buf,512);
            strcat(contents,buf);
            memset(buf,'\0',512);
        }while(bytes < size);
        *(contents+size)='\0';

        //write file contents to file.
        
        int fd = open(filename, O_WRONLY);
        if(fd<0){
            perror_exit("opening file");
        }
        if(write(fd,contents,size)<0){
            perror_exit("write");
        }
        
        free(file);
        free(contents);
        free(filename);
        free(buf);
        if(k == count-1){
            write(sock,"end", 3);
        }else{
            write(sock,"ok!",3);     //send message to server to receive next file.
        }
    }
    
    free(ip);
    free(directory);
    close(sock);
}

void perror_exit(char* message){
    perror(message);
    exit(EXIT_FAILURE);
}

void makefile(char* filename){
    char *file = (char*)malloc(sizeof(char)*strlen(filename)+1);
    char *temp = (char*)malloc(sizeof(char)*strlen(filename)+1);
    strcpy(file, filename);
    strcpy(temp,filename);
    char* token;
    char* path = (char*)malloc(sizeof(char)*strlen(filename)+1);
    path[0] = '\0';
    //make necessary directories and add file
    token = strtok(temp, "/");
    while(token != NULL){
        strcat(path,"/");
        
        if(strlen(file) == (strlen(path) + strlen(token))){
            //token is a file.
            strcat(path,token);
            if(access(path,F_OK)==0){
                //file exists.
                //delete file before creating new.
                if(remove(path)!=0){
                    perror_exit("deleting file");
                }
            }
            //file doesnt exists, create it
            creat(path,0666);
        }else{
            strcat(path,token);
            if(!strcmp(path, "/./output")){
                strcpy(path, "./output");
            }
            //token is a directory.
            if(mkdir(path, 0777) && errno != EEXIST){
                printf("error while trying to create %s\n", path);
                exit(0);
            }      
        }
        token = strtok(NULL, "/");
    }
        free(temp);
        free(file);
        free(path);
}