#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> 


//IMPORTANTE: usando el flag IPC_RMID con shmctl, se nos indica lo siguiente:
//el segmento se MARCA PARA SER DESTRUIDO CUANDO EL ULTIMO PROCESO SE DESVINCULA.
//Quien lo llama debe ser el proceso CREADOR, PROPIETARIO O UNO PRIVILEGIADO.

int main(int argc, char *argv[]) {

    int id_segmento;
    
    int *numero;

    id_segmento = shmget(321, sizeof(int), 0);

    if(id_segmento == -1) {
        
        printf("Error accediendo al segmento.");
        return 0;
        
    }

    //Parametros: id del segmento, puntero void si queremos indicarle un espacio libre a donde apuntar, y una serie de flags
    numero = (int *) shmat(id_segmento, NULL, 0);

    printf("direccion (%p)", numero);


    //Parametro: un puntero void.
    if(shmdt((void *)numero) == -1) {

        printf("\nError desvinculando la variable asignada al segmento.");
        return 0;

    } 

    //Parámetros: id del segmento, flags para indicar el tipo de operacion (usamos IPC_RMID para eliminar el segmento) y un puntero a un tipo struct, relacionado con segun que operaciones.
    if(shmctl(id_segmento, IPC_RMID, NULL) == -1) {

        printf("\nError eliminando el segmento.");
        return 0;

    }

    return 0;

}