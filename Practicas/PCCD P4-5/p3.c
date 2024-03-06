#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

//CON 5 PROCESOS

void *Carvajal();
void *Ramos();
void *Pique();
void *Thiago();


int main() {

    printf("De Gea ");

    pthread_t hilo1, hilo2, hilo3, hilo4;

    if((pthread_create(&hilo1, NULL, Carvajal, NULL) == 0) && (pthread_create(&hilo2, NULL, Ramos, NULL) == 0) && (pthread_create(&hilo3, NULL, Pique, NULL) == 0)) {

        printf("Jordi Alba ");

        if(pthread_create(&hilo4, NULL, Thiago, NULL) == 0) {

            if((pthread_join(hilo1, NULL) == 0) && (pthread_join(hilo2, NULL) == 0) && (pthread_join(hilo3, NULL) == 0)) {

                printf("Busquets ");
                printf("Isco ");
                printf("Aspas ");
                
                if(pthread_join(hilo4, NULL) == 0) {

                    printf("Morata\n");

                } else {

                    printf("Error en la espera de hilos.\n");

                }
                
            } else {

                printf("Error en la espera de hilos.\n");

            }

        } else {

            printf("Error en la cracion de hilos.\n");

        }

    } else {

        printf("Error en la creacion de hilos.\n");

    }

    return 0;

}

void *Carvajal() {

    printf("Carvajal ");

    pthread_exit(NULL);

}


void *Ramos() {

    printf("Ramos ");

    pthread_exit(NULL);

}

void *Pique() {

    printf("Pique ");

    pthread_exit(NULL);

}

void *Thiago() {

    printf("Thiago ");
    printf("Silva ");

    pthread_exit(NULL);

}