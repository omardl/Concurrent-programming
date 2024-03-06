#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

//CON 4 PROCESOS

void *Ramos();
void *Pique();
void *Jordi();

int sigue = 0;

int main() {

    printf("De Gea ");

    pthread_t hilo1, hilo2, hilo3;

    if((pthread_create(&hilo1, NULL, Ramos, NULL) == 0) && (pthread_create(&hilo2, NULL, Pique, NULL) == 0) && (pthread_create(&hilo3, NULL, Jordi, NULL) == 0)) {

        printf("Carvajal ");

        if((pthread_join(hilo1, NULL) == 0) && (pthread_join(hilo2, NULL) == 0)) {

            while(!sigue);

            printf("Busquets ");
            printf("Isco ");
            printf("Aspas ");

        } else {

            printf("Error al esperar a un hilo.\n");
            return 0;

        }

    } else  {

        printf("Error en la creación de los hilos.\n");
        return 0;

    }

    if(pthread_join(hilo3, NULL) == 0) {

        printf("Morata\n");
        
    } else {

        printf("Error al esperar a un hilo.\n");

    }

    return 0;

}

void *Ramos() {

    printf("Ramos ");

    pthread_exit(NULL);

}

void *Pique() {

    printf("Pique ");

    pthread_exit(NULL);

}

void *Jordi() {

    printf("Jordi Alba ");

    sigue++;

    printf("Thiago ");
    printf("Silva ");

    pthread_exit(NULL);

}