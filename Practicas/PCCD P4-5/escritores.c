#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>

sem_t *paso_escritura, *paso_deja_escritura, escritores;

void *escritor(int *id_escritor);

int salida = 0;

int main(int argc, char *argv[]) {

    int N3 = atoi(argv[1]);

    int id[N3];

    paso_escritura = (sem_t *)malloc(N3*sizeof(sem_t));
    paso_deja_escritura = (sem_t *)malloc(N3*sizeof(sem_t));


    for (int i = 0; i < N3; i++) {

        id[i] = i+1;

        pthread_t hilo_escritor;

        if((sem_init(&paso_escritura[i], 0, 0) != 0) || (sem_init(&paso_deja_escritura[i], 0, 0) != 0)) {

            printf("Error en la creación de un semáforo.\n");
            return 0;

        }

        if (pthread_create(&hilo_escritor, NULL, (void *) escritor, &id[i]) != 0) {

            printf("Error creando los escritores.\n");
            return 0;

        }

    }

    if (sem_init(&escritores, 0, 1) != 0) {
        
        printf("Error en la creación de un semáforo.\n");
        return 0;

    } 

    while(!salida) {

        printf("1. Intentar escritura.\n");
        printf("2. Finalizar escritura.\n");
        printf("3. Salir.\n");

        int teclado;

        scanf("%d", &teclado);
   
        switch(teclado) {

            case 1:
            printf("Introduzca el número del escritor (de 1 a %d)  ", N3);
            int manda_escribir;
            scanf("%d", &manda_escribir);
            sem_post(&paso_escritura[manda_escribir - 1]);
            break;

            case 2:
            printf("Introduzca el número del escritor (de 1 a %d)  ", N3);
            int manda_dejar;
            scanf("%d", &manda_dejar);
            sem_post(&paso_deja_escritura[manda_dejar - 1]);
            break;

            case 3:
            salida++;
            break;

        }

    }

    return 0;

}



void *escritor(int *id_escritor) {

    while(!salida) {

        printf("[Escritor %d] -> Esperando a intentar escribir...\n", *id_escritor);

        sem_wait(&paso_escritura[*id_escritor - 1]);

        printf("[Escritor %d] -> Intentando escribir... \n", *id_escritor);

        sem_wait(&escritores);

        printf("[Escritor %d] -> Escribiendo...\n", *id_escritor);

        sem_wait(&paso_deja_escritura[*id_escritor - 1]);

        sem_post(&escritores);

        printf("[Escritor %d] -> Fin escritura.\n", *id_escritor);
        
    }

    pthread_exit(NULL);

}