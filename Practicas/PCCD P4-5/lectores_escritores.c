#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>

sem_t *paso_lectura, *paso_deja_lectura, *paso_escritura, *paso_deja_escritura;

sem_t paso_entre_lectores;

sem_t escritores, lectores;

sem_t contienda_lector_escritor;

sem_t memoria_compartida;

void *escritor(int *id_escritor);
void *lector(int *id_lector);

int salida = 0;

int *numero;

int id_segmento;

int N1, N2, N3;

int main(int argc, char *argv[]) {

    N1 = atoi(argv[1]);
    N2 = atoi(argv[2]);
    N3 = atoi(argv[3]);

    if (N2 >= N1) {

        printf("El número total de lectores posibles (N1) debe ser mayor que el máximo número de lectores que pueden leer simultáneamente (N2)\n");
        return 0;

    }

    int id_lector[N1];
    int id_escritor[N3];

    paso_lectura = (sem_t *)malloc((N1)*sizeof(sem_t));
    paso_deja_lectura = (sem_t *)malloc((N1)*sizeof(sem_t));
    paso_escritura = (sem_t *)malloc((N3)*sizeof(sem_t));
    paso_deja_escritura = (sem_t *)malloc((N3)*sizeof(sem_t));

    for (int i = 0; i < N1; i++) {

        id_lector[i] = i+1;

        pthread_t hilo_lector;

        if((sem_init(&paso_lectura[i], 0, 0) != 0) || (sem_init(&paso_deja_lectura[i], 0, 0) != 0) ) {

            printf("Error en la creación de un semáforo.\n");
            return 0;

        }

        if (pthread_create(&hilo_lector, NULL, (void *) lector, &id_lector[i]) != 0) {

            printf("Error creando los lectores.\n");
            return 0;

        }
    }


    for (int i = 0; i < N3; i++) {

        id_escritor[i] = i+1;

        pthread_t hilo_escritor;

        if((sem_init(&paso_escritura[i], 0, 0) != 0) || (sem_init(&paso_deja_escritura[i], 0, 0) != 0)) {

            printf("Error en la creación de un semáforo.\n");
            return 0;

        }

        if (pthread_create(&hilo_escritor, NULL, (void *) escritor, &id_escritor[i]) != 0) {

            printf("Error creando los escritores.\n");
            return 0;

        }
    }


    if (sem_init(&contienda_lector_escritor, 0, 1) != 0) {

        printf("Error en la creación de un semáforo.\n");
        return 0;

    }

    if(sem_init(&memoria_compartida, 0, 1) != 0) {

        printf("Erro en la creación de un semáforo.\n");
        return 0;
        
    }


    if (sem_init(&escritores, 0, 1) != 0) {
        
        printf("Error en la creación de un semáforo.\n");
        return 0;

    } 

    if (sem_init(&lectores, 0, N2) != 0) {

        printf("Error en la creación de un semáforo.\n");
        return 0;

    }

    if (sem_init(&paso_entre_lectores, 0, 0) != 0) {

        printf("Error en la creación de un semáforo.\n");
        return 0;

    }

    id_segmento = shmget(265, sizeof(int), IPC_EXCL|IPC_CREAT|0777);

    numero = (int *) shmat(id_segmento, NULL, 0);

    numero[0] = 0; //Contador lectores

    numero[1] = 0; //Lectores en espera

    numero[2] = 0; //Si la contienda la ganan los lectores

    numero[3] = 0; //Contador escritores


    while(!salida) {

        printf("1. Intentar lectura.\n");
        printf("2. Finalizar lectura.\n");
        printf("3. Intentar escritura.\n");
        printf("4. Finalizar escritura.\n");
        printf("5. Salir.\n");

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
            int manda_no_leer;
            scanf("%d", &manda_no_leer);
            sem_post(&paso_deja_lectura[manda_no_leer - 1]);
            break;

            case 3:
            printf("Introduzca el número del escritor (de 1 a %d)  ", N3);
            int manda_escribir;
            scanf("%d", &manda_escribir);
            sem_post(&paso_escritura[manda_escribir - 1]);
            break;

            case 4:
            printf("Introduzca el número del escritor (de 1 a %d)  ", N3);
            int manda_no_escribir;
            scanf("%d", &manda_no_escribir);
            sem_post(&paso_deja_escritura[manda_no_escribir - 1]);
            break;

            case 5:
            salida++;
            break;

        }

    }

    return 0;

}



void *lector(int *id_lector) {

    int mi_numero, lectores_esperando, toca_leer;
    
    while(!salida) {

        printf("[Lector %d] -> Esperando a intentar leer...\n", *id_lector);

        sem_wait(&paso_lectura[*id_lector - 1]);
        
        printf("[Lector %d] -> Intentando leer... \n", *id_lector);

        sem_wait(&memoria_compartida);
        mi_numero = numero[0];
        numero[0]++;
        sem_post(&memoria_compartida);


        if (mi_numero == 0) {

            sem_wait(&contienda_lector_escritor);

            sem_wait(&memoria_compartida);
            numero[2]++;
            sem_post(&memoria_compartida);

        } else {

            sem_wait(&memoria_compartida);
            toca_leer = numero[2];
            sem_post(&memoria_compartida);

            if (toca_leer == 0) {

                sem_wait(&memoria_compartida);
                numero[1]++;
                sem_post(&memoria_compartida);

                sem_wait(&paso_entre_lectores);

            }
        
        }     

        sem_wait(&memoria_compartida);
        lectores_esperando = numero[1];
        sem_post(&memoria_compartida);

        if (lectores_esperando != 0) {

            sem_post(&paso_entre_lectores);

        }

        sem_wait(&lectores);

        printf("[Lector %d] -> Leyendo...\n", *id_lector);

        sem_wait(&paso_deja_lectura[*id_lector - 1]);

        sem_post(&lectores);

        sem_wait(&memoria_compartida);
        mi_numero = numero[0];
        numero[0]--;
        sem_post(&memoria_compartida);


        if (mi_numero == 1) {

            sem_post(&contienda_lector_escritor); 

            sem_wait(&memoria_compartida);
            numero[2]--;
            sem_post(&memoria_compartida);

        }    

        printf("[Lector %d] -> Fin lectura.\n", *id_lector);
        
    }

    pthread_exit(NULL);

}



void *escritor(int *id_escritor) {

    int contador;

    while(!salida) {

        printf("[Escritor %d] -> Esperando a intentar escribir...\n", *id_escritor);
        
        sem_wait(&paso_escritura[*id_escritor - 1]);

        printf("[Escritor %d] -> Intentando escribir... \n", *id_escritor);

        sem_wait(&contienda_lector_escritor);

        sem_wait(&escritores);

        sem_wait(&memoria_compartida);
        numero[3]++;
        sem_post(&memoria_compartida);

        printf("[Escritor %d] -> Escribiendo...\n", *id_escritor);

        sem_wait(&paso_deja_escritura[*id_escritor - 1]);

        sem_post(&escritores);

        sem_wait(&memoria_compartida);
        numero[3]--;
        contador = numero[3];
        sem_post(&memoria_compartida);

        if(contador == 0) {

            sem_post(&contienda_lector_escritor);

        }

        printf("[Escritor %d] -> Fin escritura.\n", *id_escritor);
        
    }

    pthread_exit(NULL);

}