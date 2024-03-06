#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>

sem_t *paso_lectura, *paso_deja_lectura, lectores;

void *lector(int *id_lector);

int salida = 0;

int main(int argc, char *argv[]) {

    int N1 = atoi(argv[1]);
    int N2 = atoi(argv[2]);

    if (N2 >= N1) {

        printf("El número total de lectores posibles (N1) debe ser mayor que el máximo número de lectores que pueden leer simultáneamente (N2)\n");
        return 0;

    }

    int id[N1];

    paso_lectura = (sem_t *)malloc(N1*sizeof(sem_t));
    paso_deja_lectura = (sem_t *)malloc(N1*sizeof(sem_t));


    for (int i = 0; i < N1; i++) {

        id[i] = i+1;

        pthread_t hilo_lector;

        if((sem_init(&paso_lectura[i], 0, 0) != 0) || (sem_init(&paso_deja_lectura[i], 0, 0) != 0) ) {

            printf("Error en la creación de un semáforo.\n");
            return 0;

        }

        if (pthread_create(&hilo_lector, NULL, (void *) lector, &id[i]) != 0) {

            printf("Error creando los lectores.\n");
            return 0;

        }

    }

    if (sem_init(&lectores, 0, N2) != 0) {
        
        printf("Error en la creación de un semáforo.\n");
        return 0;

    } 

    while(!salida) {

        printf("1. Intentar leer.\n");
        printf("2. Finalizar leer.\n");
        printf("3. Salir.\n");

        int teclado;

        scanf("%d", &teclado);
   
        switch(teclado) {

            case 1:
            printf("Introduzca el número del lector (de 1 a %d)  ", N1);
            int manda_leer;
            scanf("%d", &manda_leer);
            sem_post(&paso_lectura[manda_leer - 1]);
            break;

            case 2:
            printf("Introduzca el número del lector (de 1 a %d)  ", N1);
            int manda_dejar;
            scanf("%d", &manda_dejar);
            sem_post(&paso_deja_lectura[manda_dejar - 1]);
            break;

            case 3:
            salida++;
            break;

        }

    }

    return 0;

}



void *lector(int *id_lector) {

    while(!salida) {

        printf("[Lector %d] -> Esperando a intentar leer...\n", *id_lector);

        sem_wait(&paso_lectura[*id_lector - 1]);

        printf("[Lector %d] -> Intentando leer... \n", *id_lector);

        sem_wait(&lectores);

        printf("[Lector %d] -> Leyendo...\n", *id_lector);

        sem_wait(&paso_deja_lectura[*id_lector - 1]);

        sem_post(&lectores);

        printf("[Lector %d] -> Fin lectura.\n", *id_lector);
        
    }

    pthread_exit(NULL);

}