#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "netinet/in.h"
#include <fcntl.h>
#include "signal.h"
#include <unistd.h>

#define BACKLOG 64
#define BUFFER_SIZE 8096

//Global variables.

    //Arguments
unsigned long ip;
unsigned short int port;
char requestedFile[100];
int n, k;

    //Socket variables.
int socketfd;
static struct sockaddr_in serverAddr;


void connectToServer() {
    if((socketfd = socket(AF_INET, SOCK_STREAM,0)) == -1) {
        printf("Fallo al devolver el descriptor del socket.\n\n");
    }


	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ip;
	serverAddr.sin_port = port;

	/* Connect tot he socket offered by the web server */
	if(connect(socketfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        printf("Fallo al conectarse al servidor.\n\n");
	}



}

void doRequest() {

}

int main(int argc, char ** argv) {

    if(argc != 6) printf("Número inválido de argumentos.\n\n");
    else {

        //Assign arguments.
        ip = strtol(argv[1], NULL,10);
        port = atoi(argv[2]);
        strcpy(requestedFile, argv[3]);
        n = atoi(argv[4]);
        k = atoi(argv[5]);

        connectToServer();

        char requestString[] = "GET ";
        strcat(requestString, requestedFile);
        strcat(requestString, " HTTP/1.1 \r\n\r\n");



    }
    return 0;
}

