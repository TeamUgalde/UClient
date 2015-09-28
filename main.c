#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "netinet/in.h"
#include <fcntl.h>
#include "signal.h"
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 8096

//Global variables.

char ip[20];
int port;
char requestedFile[100];
int n, k;
char requestString[200] = "GET ";

struct sockaddr_in serverAddr;

void* doRequest() {
    int socketfd;

    for(int i = 0; i < k; i++) {

        if((socketfd = socket(AF_INET, SOCK_STREAM,0)) == -1) {
            printf("Fallo al devolver el descriptor del socket.\n\n");
        }

        if(connect(socketfd, (struct sockaddr *) &serverAddr,  sizeof(struct sockaddr)) == -1) {
            printf("Fallo al conectarse al servidor.\n\n");
        }

        write(socketfd, requestString, strlen(requestString));

        char buffer[BUFFER_SIZE];
        int readBytes = 0;

        while( (readBytes = read(socketfd, buffer, BUFFER_SIZE) ) > 0) {
            write(1,buffer,readBytes);
        }

        close(socketfd);
    }
    return NULL;
}

int main(int argc, char ** argv) {
    if(argc != 6) printf("Número inválido de argumentos.\n\n");
    else {
        //Assign arguments.
        strcpy(ip, argv[1]);
        port = atoi(argv[2]);
        strcpy(requestedFile, argv[3]);
        n = atoi(argv[4]);
        k = atoi(argv[5]);

        //Assign server information.
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = inet_addr(ip);
        serverAddr.sin_port = htons(port);

        //Build the request string.
        strcat(requestString, requestedFile);
        char requestString2[] = " HTTP/1.1";
        strcat(requestString, requestString2);

        pthread_t threads[n];
        for(int i = 0; i < n; i++) {
            pthread_create(&threads[i], NULL, &doRequest, NULL);
        }

        for(int i = 0; i < n; i++) {
            pthread_join(threads[i], NULL);
        }

    }
    return 0;
}

