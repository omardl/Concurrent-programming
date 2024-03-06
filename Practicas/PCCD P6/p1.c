#include <sys/msg.h>
#include <stdio.h>

#include <errno.h>
#include <string.h> 

int main() {

    /*int msgget(key_t key, int msgflg) --> Devuelve el identificador de una cola de mensajes asociada a la key y ya creada. Sucede cuando msgflg = 0 y la key no tenia el valor IPC_PRIVATE al crearla. 
                                        --> Se crea una nueva cola si la key tiene el valor IPC_PRIVATE, cuando no lo tiene pero no hay una cola asociada a la key e IPC_CREAT se especifica en msgflg
                                        --> Devuelve el ID, o -1 si hay error.
                                        --> Al crear se definen en msgflg los permisos
    */

    int ID_cola = msgget(123, IPC_CREAT|0777);

    if (ID_cola == -1) {
        
        printf("Error creando la cola con ID %d.\n", ID_cola);
        printf("ERROR: %s.\n", strerror(errno));

    } else {

        printf("Se ha creado con éxito la cola con ID %d.\n", ID_cola);

    }

    return 0;

}