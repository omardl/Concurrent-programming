#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> 


int main(int argc, char *argv[]) {

    int id_segmento;
    int *numero;
 
    //Clave creada + tamaño + flags para asignar segmento a memoria compartida.
    //Flags: IPC_CREAT para crear, IPC_EXCL para asegurar que no existe ya un ID de segmento asociado con la misma clave, 0777 son los permisos de acceso.
    id_segmento = shmget(311, sizeof(int), IPC_EXCL|IPC_CREAT|0777);

    if(id_segmento == -1) {
        
        printf("Error creando el segmento.");
        return 0;
        
    }

    //shmat nos devuelve un puntero void, que casteamos al tipo que nos interesa.
    //Recibe el id del segmento, un puntero void si queremos indicarle un espacio libre a donde apuntar, y una serie de flags
    numero = (int *) shmat(id_segmento, NULL, 0);

    *numero = atoi(argv[1]);

    printf("\nP1 entrara en suspension medio minuto para que se ejecute P2 y acceda al numero, despues P1 desasignara y liberara el espacio.");

    printf("\nNumero compartido: %d\n", *numero);

    sleep(30);

    //Desasigna el espacio del segmento asociado a la variable, el parametro es un puntero void.
    if(shmdt((void *)numero) == -1) {

        printf("\nError desvinculando la variable asignada al segmento.");
        return 0;

    } 

    //Realiza operaciones de control sobre el segmento de memoria compartida.
    //Parámetros: id del segmento, flags para indicar el tipo de operacion (usamos IPC_RMID para eliminar el segmento) y un puntero a un tipo struct, relacionado con segun que operaciones.
    if(shmctl(id_segmento, IPC_RMID, NULL) == -1) {

        printf("\nError eliminando el segmento.");
        return 0;

    }

    return 0;

}