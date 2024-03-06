#include <stdio.h>
#include <pthread.h>
#include <sys/msg.h>
#include <unistd.h>


int ID_cola_tenedores;
int ID_cola_filosofos_intentando_comer;

void *filosofo(int *id_filosofo);

struct mensaje {
        
        long mstype;
        char mstxt[0];

};


int main() {

    ID_cola_tenedores = msgget(4321, IPC_CREAT|0777);

    if (ID_cola_tenedores == -1) {
        
        printf("Error creando la cola con ID %d.\n", ID_cola_tenedores);
        return 0;

    } 

    ID_cola_filosofos_intentando_comer = msgget(1234, IPC_CREAT|0777);

    if (ID_cola_filosofos_intentando_comer == -1) {

        printf("Error creando la cola con ID %d.\n", ID_cola_filosofos_intentando_comer);
        return 0;

    }


    //Cola de mensajes para tenedores, cada tenedor es un tipo diferente de mensaje
    for(int i = 1; i < 6; i++) {

        struct mensaje nuevo_mensaje;
        nuevo_mensaje.mstype = i; 

        if (msgsnd(ID_cola_tenedores, &nuevo_mensaje, sizeof(char[0]), 0) != 0) {

            printf("Error al enviar un mensaje a la cola con ID %d\n", ID_cola_tenedores);
            return 0;

        }

    }


    //4 mensajes para la cola que limita la entrada a la Sección Crítica
    for(int j = 0; j < 4; j++) {

        struct mensaje nuevo_mensaje;
        nuevo_mensaje.mstype = 1;

        if (msgsnd(ID_cola_filosofos_intentando_comer, &nuevo_mensaje, sizeof(char[0]), 0) != 0) {

            printf("Error al enviar un mensaje a la cola con ID %d\n", ID_cola_filosofos_intentando_comer);
            return 0;

        }
    }

    //Se crean los hilos de los 5 filósofos
    int parametros[5];

    for(int i = 1; i < 6; i++) {

        parametros[i-1] = i;

        pthread_t filo;

        if(pthread_create(&filo, NULL, (void *) filosofo, &parametros[i-1]) != 0) {

            printf("Error creando los filósofos.\n");
            return 0;

        }

    }

    while(1) {}

    return 0;

}

void *filosofo(int *id_filosofo) {    

    while(1) {

        printf("Filósofo [%d]: Pensando...\n", *id_filosofo);

        struct mensaje coger_tenedor1, coger_tenedor2;
        struct mensaje seccion_critica;

        msgrcv(ID_cola_filosofos_intentando_comer, &seccion_critica, sizeof(char[0]), 1, 0);

        if (*id_filosofo == 5) {

            msgrcv(ID_cola_tenedores, &coger_tenedor1, sizeof(char[0]), 1, 0);

            printf("Filósofo [%d]: Cogiendo el tenedor (1) de mi izquierda.\n", *id_filosofo);

        } else {

            msgrcv(ID_cola_tenedores, &coger_tenedor1, sizeof(char[0]), *id_filosofo + 1, 0);

            printf("Filósofo [%d]: Cogiendo el tenedor (%d) de mi izquierda.\n", *id_filosofo, *id_filosofo + 1);

        }
        
        if (*id_filosofo == 1) {

            msgrcv(ID_cola_tenedores, &coger_tenedor2, sizeof(char[0]), 5, 0);

            printf("Filósofo [%d]: Cogiendo el tenedor (5) de mi derecha.\n", *id_filosofo);

        } else {

            msgrcv(ID_cola_tenedores, &coger_tenedor2, sizeof(char[0]), *id_filosofo - 1, 0);

            printf("Filósofo [%d]: Cogiendo el tenedor (%d) de mi derecha.\n", *id_filosofo, *id_filosofo - 1);

        }

        printf("Filósofo [%d]: Comiendo...\n", *id_filosofo);

        //sleep(3); //Para poder ejecutarlo e ir leyendo los mensajes para comprobar su correcto funcionamiento

        struct mensaje dejar_tenedor1, dejar_tenedor2;

        if (*id_filosofo == 5) {

            printf("Filósofo [%d]: Dejando el tenedor (1) de mi izquierda.\n", *id_filosofo);

            dejar_tenedor1.mstype = 1;

        } else {

            printf("Filósofo [%d]: Dejando el tenedor (%d) de mi izquierda.\n", *id_filosofo, *id_filosofo + 1);

            dejar_tenedor1.mstype = *id_filosofo + 1;

        }

        msgsnd(ID_cola_tenedores, &dejar_tenedor1, sizeof(char[0]), 0);
        
        if (*id_filosofo == 1) {

            printf("Filósofo [%d]: Dejando el tenedor (5) de mi derecha.\n", *id_filosofo);

            dejar_tenedor2.mstype = 5;

        } else {

            printf("Filósofo [%d]: Dejando el tenedor (%d) de mi derecha.\n", *id_filosofo, *id_filosofo - 1);

            dejar_tenedor2.mstype = *id_filosofo - 1;

        }
        
        msgsnd(ID_cola_tenedores, &dejar_tenedor2, sizeof(char[0]), 0);

        msgsnd(ID_cola_filosofos_intentando_comer, &seccion_critica,sizeof(char[0]), 0);

    }

}