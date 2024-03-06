#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <stdio.h>

int main() {

    //Para acceder a un segmento creado, IPC_CREAT a 0 (accede si tiene permiso).
    int ID_segmento = shmget(311, sizeof(int), 0);

    if(ID_segmento == -1) {
        
        printf("Error al obtener el segmento.");
        return 0;
        
    }

    int *numero;
    numero = (int *) shmat(ID_segmento, NULL, 0);

    printf("Numero compartido por p1: %d\n", *numero);

    return 0;

}