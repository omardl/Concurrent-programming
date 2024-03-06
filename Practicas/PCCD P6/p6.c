#include <stdio.h>
#include <pthread.h>
#include <sys/msg.h>
#include <unistd.h>


int ID_cola_tenedor1, ID_cola_tenedor2, ID_cola_tenedor3, ID_cola_tenedor4, ID_cola_tenedor5;
int ID_cola_filosofos_intentando_comer;

void *filosofo(int *id_filosofo);

struct mensaje {
        
        long mstype;
        char mstxt[0];

};


int main() {

    ID_cola_tenedor1 = msgget(5432, IPC_CREAT|0777);
    ID_cola_tenedor2 = msgget(6543, IPC_CREAT|0777);
    ID_cola_tenedor3 = msgget(7654, IPC_CREAT|0777);
    ID_cola_tenedor4 = msgget(8765, IPC_CREAT|0777);
    ID_cola_tenedor5 = msgget(9876, IPC_CREAT|0777);

    if ((ID_cola_tenedor1 == -1)||(ID_cola_tenedor2 == -1)||(ID_cola_tenedor3 == -1)||(ID_cola_tenedor4 == -1)||(ID_cola_tenedor5 == -1)) {
        
        printf("Error creando alguna cola de tenedor.\n");
        return 0;

    } 

    ID_cola_filosofos_intentando_comer = msgget(1221, IPC_CREAT|0777);

    if (ID_cola_filosofos_intentando_comer == -1) {

        printf("Error creando la cola con ID %d.\n", ID_cola_filosofos_intentando_comer);
        return 0;

    }


    //Una cola para cada tenedor
    struct mensaje nuevo_mensaje;
    nuevo_mensaje.mstype = 1; 
    if (msgsnd(ID_cola_tenedor1, &nuevo_mensaje, sizeof(char[0]), 0) != 0) {

        printf("Error al enviar un mensaje a la cola con ID %d\n", ID_cola_tenedor1);
        return 0;

    }

    if (msgsnd(ID_cola_tenedor2, &nuevo_mensaje, sizeof(char[0]), 0) != 0) {

        printf("Error al enviar un mensaje a la cola con ID %d\n", ID_cola_tenedor2);
        return 0;

    }

    if (msgsnd(ID_cola_tenedor3, &nuevo_mensaje, sizeof(char[0]), 0) != 0) {

        printf("Error al enviar un mensaje a la cola con ID %d\n", ID_cola_tenedor3);
        return 0;

    }

    if (msgsnd(ID_cola_tenedor4, &nuevo_mensaje, sizeof(char[0]), 0) != 0) {

        printf("Error al enviar un mensaje a la cola con ID %d\n", ID_cola_tenedor4);
        return 0;

    }

    if (msgsnd(ID_cola_tenedor5, &nuevo_mensaje, sizeof(char[0]), 0) != 0) {

        printf("Error al enviar un mensaje a la cola con ID %d\n", ID_cola_tenedor5);
        return 0;

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

        struct mensaje coger_tenedorI, coger_tenedorD;
        struct mensaje seccion_critica;

        msgrcv(ID_cola_filosofos_intentando_comer, &seccion_critica, sizeof(char[0]), 1, 0);

        switch(*id_filosofo) {

            case 1:
            msgrcv(ID_cola_tenedor2, &coger_tenedorI, sizeof(char[0]), 1, 0);
            break;

            case 2:
            msgrcv(ID_cola_tenedor3, &coger_tenedorI, sizeof(char[0]), 1, 0);
            break;

            case 3:
            msgrcv(ID_cola_tenedor4, &coger_tenedorI, sizeof(char[0]), 1, 0);
            break;

            case 4:
            msgrcv(ID_cola_tenedor5, &coger_tenedorI, sizeof(char[0]), 1, 0);
            break;

            case 5:
            msgrcv(ID_cola_tenedor1, &coger_tenedorI, sizeof(char[0]), 1, 0);
            break;

            default:
            printf("Está ocurriendo algún error con los IDs de los filósofos.\n");
            break;
        }

        if (*id_filosofo == 5) {

            printf("Filósofo [%d]: Cogiendo el tenedor (1) de mi izquierda.\n", *id_filosofo);

        } else {

            printf("Filósofo [%d]: Cogiendo el tenedor (%d) de mi izquierda.\n", *id_filosofo, *id_filosofo + 1);

        }
        

        switch(*id_filosofo) {

            case 1:
            msgrcv(ID_cola_tenedor5, &coger_tenedorD, sizeof(char[0]), 1, 0);
            break;

            case 2:
            msgrcv(ID_cola_tenedor1, &coger_tenedorD, sizeof(char[0]), 1, 0);
            break;

            case 3:
            msgrcv(ID_cola_tenedor2, &coger_tenedorD, sizeof(char[0]), 1, 0);
            break;

            case 4:
            msgrcv(ID_cola_tenedor3, &coger_tenedorD, sizeof(char[0]), 1, 0);
            break;

            case 5:
            msgrcv(ID_cola_tenedor4, &coger_tenedorD, sizeof(char[0]), 1, 0);
            break;

            default:
            printf("Está ocurriendo algún error con los IDs de los filósofos.\n");
            break;
        }


        if (*id_filosofo == 1) {

            printf("Filósofo [%d]: Cogiendo el tenedor (5) de mi derecha.\n", *id_filosofo);

        } else {

            printf("Filósofo [%d]: Cogiendo el tenedor (%d) de mi derecha.\n", *id_filosofo, *id_filosofo - 1);

        }

        printf("Filósofo [%d]: Comiendo...\n", *id_filosofo);

        //sleep(3);   //Para poder ejecutarlo e ir leyendo los mensajes para comprobar su correcto funcionamiento

        struct mensaje dejar_tenedorI, dejar_tenedorD;
        dejar_tenedorI.mstype = 1;
        dejar_tenedorD.mstype = 1;

        if (*id_filosofo == 5) {

            printf("Filósofo [%d]: Dejando el tenedor (1) de mi izquierda.\n", *id_filosofo);

        } else {

            printf("Filósofo [%d]: Dejando el tenedor (%d) de mi izquierda.\n", *id_filosofo, *id_filosofo + 1);

        }
    

        switch(*id_filosofo) {

            case 1:
            msgsnd(ID_cola_tenedor2, &dejar_tenedorI, sizeof(char[0]), 0);
            break;

            case 2:
            msgsnd(ID_cola_tenedor3, &dejar_tenedorI, sizeof(char[0]), 0);
            break;

            case 3:
            msgsnd(ID_cola_tenedor4, &dejar_tenedorI, sizeof(char[0]), 0);
            break;

            case 4:
            msgsnd(ID_cola_tenedor5, &dejar_tenedorI, sizeof(char[0]), 0);
            break;

            case 5:
            msgsnd(ID_cola_tenedor1, &dejar_tenedorI, sizeof(char[0]), 0);
            break;

            default:
            printf("Está ocurriendo algún error con los IDs de los filósofos.\n");
            break;

        }

        if (*id_filosofo == 1) {

            printf("Filósofo [%d]: Dejando el tenedor (5) de mi derecha.\n", *id_filosofo);

        } else {

            printf("Filósofo [%d]: Dejando el tenedor (%d) de mi derecha.\n", *id_filosofo, *id_filosofo - 1);

        }

        switch(*id_filosofo) {

            case 1:
            msgsnd(ID_cola_tenedor5, &dejar_tenedorD, sizeof(char[0]), 0);
            break;

            case 2:
            msgsnd(ID_cola_tenedor1, &dejar_tenedorD, sizeof(char[0]), 0);
            break;

            case 3:
            msgsnd(ID_cola_tenedor2, &dejar_tenedorD, sizeof(char[0]), 0);
            break;

            case 4:
            msgsnd(ID_cola_tenedor3, &dejar_tenedorD, sizeof(char[0]), 0);
            break;

            case 5:
            msgsnd(ID_cola_tenedor4, &dejar_tenedorD, sizeof(char[0]), 0);
            break;

            default:
            printf("Está ocurriendo algún error con los IDs de los filósofos.\n");
            break;
        }
        
        msgsnd(ID_cola_filosofos_intentando_comer, &seccion_critica,sizeof(char[0]), 0);

    }

}