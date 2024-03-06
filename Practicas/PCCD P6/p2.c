#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <errno.h> 

int main(int argc, char *argv[]) {

    int ID_cola = atoi(argv[1]);

    printf("El parámetro de entrada es %d.\n", ID_cola);

    /*int msgctl(int msqid, int cmd, struct msqid_ds *buf)
        --> Realiza una operacion de control indicada en cmd, sobre la cola con ID msqid
        --> Devuelve 0 si la ejecución ha sido correcta, -1 si no
        
    cmd --> valores posibles:
     - IPC_STAT: Copia info de la estructura de datos del kernel asociada con el msqid en la struct a la que apunta *buf.
     - IPC_SET: escribe valores para algunos parametros dentro de la estructura.
     - IPC_RMID: elimina la cola de mensajes despertando cualquier proceso en espera, que devolverá un error
     - IPC_INFO: Devuelve info sobre algunos parametros de la cola en un struct.
     - etc.
    */

    if(msgctl(ID_cola, IPC_RMID, NULL) == -1) {

        printf("ERROR: %s.\n", strerror(errno));

    } else {

        printf("Se ha eliminado la cola con el ID %d.\n", ID_cola);

    }
    
    return 0;

}