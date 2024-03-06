#include <stdio.h>
#include <sys/msg.h> 
#include <stdlib.h>
 
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    
    int ID_cola = atoi(argv[1]);

    /*int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg) 
        --> Devuelve 0 si hay exito, -1 si no.
        --> Realiza una copia del mensaje en la estructura msgp al buzon especificado.
        --> La cola debe tener permisos de escritura.
        --> msgp es una estructura que debe contener un primer tipo int positivo (comunmente long), y un segundo tipo que suele ser una cadena de char, aunque puede variar e incluso tener longitud 0.
        --> La cola tiene una capacidad de bytes predefinida que puede modificarse con msgctl

        --> Es BLOQUEANTE por defecto, si la cola está llena se bloqueará hasta tener espacio suficiente. Para que sea NO BLOQUEANTE deberá especificarse el flag IPC_NOWAIT, en cuyo caso dará directamente error.
        --> Un comportamiento BLOQUEANTE también dará error si la cola es eliminada o recibe una señal.

        --> msgsz es el tamaño del tipo del struct en el que se almacene el mensaje, sin tener en cuenta el long.
    */
    
    struct mensaje {
        
        long mstype;
        char mstxt[50];

    };

    struct mensaje nuevo_mensaje;
    nuevo_mensaje.mstype = 8;
    strcpy(nuevo_mensaje.mstxt, "tercer mensajito."); //Probando el miniejemplo 3 de la practica 6


    if (msgsnd(ID_cola, &nuevo_mensaje, sizeof(char[50]), 0) != 0) {

        printf("Error al enviar un mensaje a la cola con ID %d\n", ID_cola);

        printf("ERROR (%s)\n", strerror(errno));

    } else {

        printf("Se ha enviado el mensaje < %s > al buzón con ID %d.\n", nuevo_mensaje.mstxt, ID_cola);

    }

    return 0;

}