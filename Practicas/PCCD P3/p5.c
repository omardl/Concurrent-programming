#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main() {

    int id_segmento;

    printf("\nDeben ejecutarse SÓLO 2 instancias del programa CON LA MISMA CLAVE para evitar errores, ya que a partir de la primera ejecución se abre Q.\n");

    //SI ES LA PRIMERA INSTANCIA DE P5, SERA LA INSTANCIA 0 (PROCESO P), SI ES LA SEGUNDA, SERÁ LA 1 (PROCESO Q)    
    int instancia = 0;

    //turno 0 = puerta abierta para P, cerrada para Q
    //turno 1 = puerta abierta para Q, cerrada para P
    int *turno = 0;

    id_segmento = shmget(351, sizeof(int), IPC_EXCL|IPC_CREAT|0777);

    if(id_segmento == -1) {
        
        //Si da error porque el segmento ya existe, accedemos a el (segunda instancia de p5)
        id_segmento = shmget(351, sizeof(int),0);
        printf("\nPROCESO Q\n\n");

        instancia++;

        if (instancia > 1)

        if (id_segmento == -1) {

            printf("Error al crear/obtener el ID del segmento.\n");
            return 0;

        }
    } else {

        printf("\nPROCESO P\n\n");
        

    }

    turno = (int *) shmat(id_segmento, NULL, 0);

    int sigue_P = 0;
    int sigue_Q = 0;
    
    while(1) {

        printf("Caminando por mi habitación.\n");

        if(instancia == 0) {

            while(!sigue_P) {

                getchar();

                printf("Intentando entrar a mi Sección Crítica...\n");

                if (*turno == 1) {

                    printf("Puerta cerrada.\n");

                } else {

                    printf("Dentro de mi Sección Crítica.\n");
                    getchar();
                    sigue_P++;
                }
            }

            printf("He salido de mi Sección Crítica.\n");
            getchar();
            *turno = 1;
            printf("He accionado mi pulsador.\n");
            sigue_P--;

        } else {

            while(!sigue_Q) {

                getchar();

                printf("Intentando a mi Sección Crítica...\n");

                if (*turno == 0) {

                    printf("Puerta cerrada.\n");

                } else {

                    printf("Dentro de mi Sección Crítica.\n");
                    getchar();
                    sigue_Q++;
                }
            }

            printf("He salido de mi Sección Crítica.\n");
            getchar();
            *turno = 0;
            printf("He accionado mi pulsador.\n");
            sigue_Q--;

        }

    }

    //NOTA: Al tener que detener el proceso con ctrl+C, no se puede desvincular y eliminar el segmento, si se detiene y se vuelve a ejecutar  más de dos veces (con la misma clave) estará todo el rato sin poder volver a la instancia 0; 
    //Deberá cambiarse la clave para crear un nuevo segmento de memoria compartida, y el anterior quedará sin liberar.

    //Se cumple la EXCLUSIÖN MUTUA y no hay INTERBLOQUEO.

    //Para simular la INANICIÖN basta, por ejemplo, con que el primer proceso nunca quiera ni llegue a entrar en su sección crítica.

}