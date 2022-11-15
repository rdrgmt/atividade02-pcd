// matheus f rodrigues - 93943

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#define NUMTHREADS 4

// globals
int soma = 0;
int request = 0;
int respond = 0;

// headers
void server();          // server process
void critical(int id);  // critical section
void client(int id);    // client process

// funcs
// server
void server(){
    while(1){
        while(request == 0);
        respond = request;

        while(respond != 0);
        request = 0;
    }
}

//critical
void critical(int id){
    int local = soma;
    sleep(rand()%2);
    soma = local + 1;
}

// client
void client(int id){
    while(1){
        // pré protocolo
        while(respond != id) request = id; // comentar esta linha para remover o controle da secao critica!!

        // secao critica
        critical(id);
        printf("ID: %d / SOMA: %d\n", id, soma);

        // pos protocolo
        respond = 0;
    }
}

int main(){
    omp_set_num_threads(NUMTHREADS);

    #pragma omp parallel
    {
        int id = omp_get_thread_num();

        if(id == 0) server();
        else client(id);
    }

    return 0;
}