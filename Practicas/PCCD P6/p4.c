#include <stdio.h> 
#include <stdlib.h>
#include <sys/msg.h>

#include <string.h>
#include <errno.h>


int main(int argc, char *argv[]) {

    int ID_cola = atoi(argv[1]);

    struct mensaje {  

        long mstype;
        char mstxt[50];    

    } nuevo_mensaje;
    /* ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg)  
        --> Devolverá el número de bytes copiados al array de la estructura, o -1 en caso de error.
        --> Por defecto es BLOQUEANTE: espera hasta que llega un mensaje del tipo deseado, se elimina la cola o recibe una señal.
        --> Para que se comporte como NO BLOQUEANTE debe indicarse IPC_NOWAIT en msgflg.

        --> msgsz indica la longitud de mstxt de la struct, que puede ser distinta en el origen. El flag MSG_NOERROR permite truncar
            el mensaje si la longitud es excedida, en caso contrario dará error.
    */

    //NOTA: si hay mas de un mensaje se lee el que primero llega a netrar en la cola

     

    if (msgrcv(ID_cola, &nuevo_mensaje, sizeof(char[50]), 8, 0) < 0) {

        printf("ERROR: %s.", strerror(errno));

    } else {

        printf("Se ha recibido el mensaje < %s > de la cola %d.\n", nuevo_mensaje.mstxt, ID_cola);
        
    }
    

    return 0;



}