#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> 
#include <string.h>


int main() {

    int id_segmento;
    
    int *numero;
    char parametro[5] = "0";
    
    int salida = 0;

    //Si se invoca p3 antes de crear el segmento, al ser p3-1 el padre, p3 da error al intentar acceder al mismo

    id_segmento = shmget(321, sizeof(int), IPC_EXCL|IPC_CREAT|0777);

    //Si se invoca p3 a partir de este punto, el unico momento en el que p3-1 dara error será al tratar de eliminar el segmento (mas bien, marcarlo para su borrado), ya que lo habrá eliminado antes p3.
    //p3 marca el segmento para su eliminación, pero ésta no ocurre hasta que todos los procesos se desvinculen del segmento, y sólo podrá hacerlo el proceso creador, propietario o con permisos para ello.

    if(id_segmento == -1) {
        
        printf("Error creando el segmento.");
        return 0;
        
    }


    numero = (int *) shmat(id_segmento, NULL, 0);

    while(!salida) {

        printf("\nIntroduce un numero (máximo 5 cifras) a compartir con p3-2, o pulsa s para salir del programa: ");
        scanf("%s", parametro);
        
        if ((strcmp(parametro,"s") != 0)&&(strcmp(parametro,"S") != 0)) {

            *numero = atoi(parametro);

        } else {

            salida++;
        
        }

    } 
    
    if(shmdt((void *)numero) == -1) {

        printf("\nError desvinculando la variable asignada al segmento.");
        return 0;

    } 

    if(shmctl(id_segmento, IPC_RMID, NULL) == -1) {

        printf("\nError eliminando el segmento.");
        return 0;

    }

    return 0;

}