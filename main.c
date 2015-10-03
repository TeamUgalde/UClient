#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "netinet/in.h"
#include <fcntl.h>
#include "signal.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include "array.h"
#include <math.h>

#define BUFFER_SIZE 8096

//Global variables.

char ip[20];
int port;
char requestedFile[100];
int n, k;
char requestString[200] = "GET ";

struct sockaddr_in serverAddr;

pthread_mutex_t mutex;
Array arr;
double average_time, variance;

void* doRequest() {

    int socketfd;

    for(int i = 0; i < k; i++) {

        if((socketfd = socket(AF_INET, SOCK_STREAM,0)) == -1) {
            printf("Fallo al devolver el descriptor del socket.\n\n");
        }

        if(connect(socketfd, (struct sockaddr *) &serverAddr,  sizeof(struct sockaddr)) == -1) {
            printf("Fallo al conectarse al servidor.\n\n");
        }

        clock_t begin, end;
        double time_spent;
        begin = clock();
        write(socketfd, requestString, strlen(requestString));
        end = clock();
        time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        printf("Tiempo: %lf segundos\n", time_spent);
        pthread_mutex_lock(&mutex);
        insertArray(&arr, time_spent);
        pthread_mutex_unlock(&mutex);

        char buffer[BUFFER_SIZE];
        int readBytes = 0;

        while( (readBytes = read(socketfd, buffer, BUFFER_SIZE) ) > 0) {
            write(1,buffer,readBytes);
        }

        close(socketfd);
    }
    return NULL;
}

double average() {
    double result = 0;
    for(int i = 0; i < n*k; i++) {
        result += arr.array[i];
    }
    result /= (n*k);
    return result;
}

double calculateVariance() {
    double result;
    for(int i = 0; i < n*k; i++) {
        result += pow(average_time - arr.array[i], 2);
    }
    result /= (n*k);
    return result;
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
        initArray(&arr, 1);
        pthread_mutex_init(&mutex, NULL);

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
        average_time = average();
        variance = calculateVariance();
        printf("Tiempo promedio: %lf segundos\nVarianza: %lf segundos\n", average_time, variance);
    }
    return 0;
}

