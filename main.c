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
#include <math.h>

#define BUFFER_SIZE 16192

//Global variables.

int printResource = 0;
char ip[20];
int port;
char requestedFile[100];
int n, k, nk;
char requestString[200] = "GET ";

struct sockaddr_in serverAddr;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
double * arr;
int globalIndex = 0;
double average_time, variance;

// Function in charge of making the request to the server.
void* doRequest() {

    int socketfd;

    for(int i = 0; i < k; i++) {

        //Request a new socket.
        if((socketfd = socket(AF_INET, SOCK_STREAM,0)) == -1) {
            printf("Fallo al devolver el descriptor del socket.\n\n");
        }
        if(connect(socketfd, (struct sockaddr *) &serverAddr,  sizeof(struct sockaddr)) == -1) {
            printf("Fallo al conectarse al servidor.\n\n");
        }

        // Calcualtes the time it took to write each request.
        clock_t begin, end;
        double time_spent;
        begin = clock();
        write(socketfd, requestString, strlen(requestString));
        end = clock();
        time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

        // Registers the time spent in the array.
        pthread_mutex_lock(&mutex);
        arr[globalIndex++] = time_spent;
        pthread_mutex_unlock(&mutex);

        // Writes what was read to the 1st standard output (console).
        char buffer[BUFFER_SIZE];
        int readBytes = 0;

        while((readBytes = read(socketfd, buffer, BUFFER_SIZE) ) > 0) {
            if(printResource) write(1,buffer,readBytes);
        }

        close(socketfd);
    }
    return NULL;
}

// Returns the average for an array of numbers.
double average() {
    double result = 0;
    for(int i = 0; i < nk; i++) {
        result += arr[i];
    }
    result /= (nk);
    return result;
}

// Returns the variance for an array of doubles.
double calculateVariance() {
    double result;
    for(int i = 0; i < nk; i++) {
        result += pow(average_time - arr[i], 2);
    }
    result /= (nk);
    return result;
}

int main(int argc, char ** argv) {
    if(argc != 6 && argc != 7) printf("Número inválido de argumentos.\n\n");
    else {

        //Assign arguments.
        strcpy(ip, argv[1]);
        port = atoi(argv[2]);
        strcpy(requestedFile, argv[3]);
        n = atoi(argv[4]);
        k = atoi(argv[5]);
        nk = n * k;
        arr = (double *) malloc(nk*sizeof(double));

        if(argc == 7) printResource = 1;

        //Assign server information.
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = inet_addr(ip);
        serverAddr.sin_port = htons(port);

        //Build the request string.
        strcat(requestString, requestedFile);
        char requestString2[] = " HTTP/1.1";
        strcat(requestString, requestString2);

        // Creates n threads.
        pthread_t threads[n];
        for(int i = 0; i < n; i++) {
            pthread_create(&threads[i], NULL, &doRequest, NULL);
        }
        for(int i = 0; i < n; i++) {
            pthread_join(threads[i], NULL);
        }

        printf("\n\n----------Tiempos----------\n\n");
        for (int i = 0; i < nk; i++) {
            printf("%lf  ", arr[i]);
        }
        printf("\n\n");

        // Calculates & prints time average and variance.
        average_time = average();
        variance = calculateVariance();
        printf("Tiempo promedio: %lf segundos\nVarianza: %.10lf segundos\n\n", average_time, variance);
    }
    return 0;
}

