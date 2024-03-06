#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main() {

    int id_segmento;

    printf("\nDeben ejecutarse SÓLO 2 instancias del programa CON LA MISMA CLAVE para evitar errores, ya que a partir de la primera ejecución se abre Q.\n");

    //SI ES LA PRIMERA INSTANCIA DE P6 SERA LA INSTANCIA 0 (PROCESO P), SI ES LA SEGUNDA, SERÁ LA 1 (PROCESO Q)    
    int instancia = 0;

    int *quiere;

    id_segmento = shmget(361, sizeof(int)*2, IPC_EXCL|IPC_CREAT|0777);

    if(id_segmento == -1) {
        
        //Si da error porque el segmento ya existe, accedemos a el (segunda instancia de p5)
        id_segmento = shmget(361, sizeof(int),0);
        instancia = 1;
        printf("\nPROCESO Q\n\n");

        if (id_segmento == -1) {

            printf("Error al crear/obtener el ID del segmento.");
            return 0;

        }
    } else {
        printf("\nPROCESO P\n\n");
    }

    quiere = (int *) shmat(id_segmento, NULL, 0);

    //QUIERE[0] == QUIERE_P
    //QUIERE[1] == QUIERE_Q
    quiere[0] = 0;
    quiere[1] = 0;


    int sigue_P = 0;
    int sigue_Q = 0;
    
    while(1) {

        printf("Caminando por mi habitación.\n");

        if(instancia == 0) {

            while(!sigue_P) {

                getchar();

                printf("Intentando acceder al pasillo de entrada a la Sección Crítica...");

                if(quiere[1] == 0) {

                    printf("Dentro del pasillo.\n");
                    
                    getchar();

                    quiere[0] = 1;
                    printf("He accionado el pulsador.\n");
                    printf("Dentro de mi Sección Crítica.\n");
                    
                    getchar();
                    printf("He salido de mi Sección Crítica.\n");

                    getchar();
                    quiere[0] = 0;
                    printf("He accionado el pulsador.\n");
                    sigue_P++;
                
                } else {

                    printf("Puerta cerrada.\n");

                }

            }

            sigue_P--;

        } else {

            while(!sigue_Q) {

                getchar();

                printf("Intentando acceder al pasillo de entrada a la Sección Crítica...");

                if(quiere[0] == 0) {

                    printf("Dentro del pasillo.\n");
                    
                    getchar();

                    quiere[1] = 1;
                    printf("He accionado el pulsador.\n");
                    printf("Dentro de mi Sección Crítica.\n");
                    
                    getchar();
                    printf("He salido de mi Sección Crítica.\n");

                    getchar();
                    quiere[1] = 0;
                    printf("He accionado el pulsador.\n");
                    sigue_Q++;
                
                } else {

                    printf("Puerta cerrada.\n");

                }

            }

            sigue_Q--;

        }

    }
    

    //NOTA: Al tener que detener el proceso con ctrl+C, no se puede desvincular y eliminar el segmento, si se detiene y se vuelve a ejecutar  más de dos veces (con la misma clave) estará todo el rato sin poder volver a la instancia 0; 
    //Deberá cambiarse la clave para crear un nuevo segmento de memoria compartida, y el anterior quedará sin liberar

    //VIOLACIÖN DE EXCLUSIÖN MUTUA: Cuando ámbos acceden al pasillo antes de que alguno de ellos pulse, entran ámbos en la Sección Crítica.

    //No hay ninguna posible situación en la que ámbos procesos queden suspendidos sin poder avanzar en su ejecución, es decir, no hay INTERBLOQUEO.

    //INANICIÓN: Un proceso puede salir de su Sección Crítica y, en el mismo cuanto, volver a entrar antes de que el otro pueda decir que quiere hacerlo, de manera indefinida. 

}