#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> 


int main() {

    int id_segmento;
    
    int *numero;
    
    int salida = 0;

    //sleep(10); //Si p3 se ejecuta aqui, no puede obtener el ID del segmento y sale

    id_segmento = shmget(321, sizeof(int), 0);

    //Una vez obtenido el ID, aunque ejecutemos p3, el programa no fallara y obtendra el numero compartido.
    //Si cerramos p3-1 (que tambien destruye el segmento), habiendo ejecutado p3 o no, y sigue abierto p3-2; éste sigue vinculado al segmento y mostrará el ultimo valor que p3-1 compartió, el segmento se destruye al desvincular p3-2
    //A pesar de no usar shmdt en p3-2, se desvincula el proceso al terminar el mismo (el sistema detecta que el proceso que estaba vinculado al segmento finalizo).

    if(id_segmento == -1) {
        
        printf("Error al obtener el segmento.");
        return 0;
        
    }

    printf("Presiona enter para leer el numero compartido por p3-1 o s para salir: ");

    while(!salida) {

        char tecla = getchar();

        if((tecla != 's') && (tecla != 'S')) {

            numero = (int *) shmat(id_segmento, NULL, 0);

            //sleep(30);

            printf("Numero compartido por p1: %d\n", *numero);

        } else {

            salida++;

        }
    }
    
    return 0;
}