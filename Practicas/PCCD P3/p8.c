#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main() {

    int id_segmento;

    printf("\nDeben ejecutarse SÓLO 2 instancias del programa CON LA MISMA CLAVE para evitar errores, ya que a partir de la primera ejecución se abre Q.\n");

    //SI ES LA PRIMERA INSTANCIA DE P8, SERA LA INSTANCIA 0 (PROCESO P), SI ES LA SEGUNDA, SERÁ LA 1 (PROCESO Q)    
    int instancia = 0;

    int *quiere;

    id_segmento = shmget(381, sizeof(int)*2, IPC_EXCL|IPC_CREAT|0777);

    if(id_segmento == -1) {
        
        //Si da error porque el segmento ya existe, accedemos a el (segunda instancia de p5)
        id_segmento = shmget(381, sizeof(int),0);
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
    
    while(1) {

        printf("Caminando por mi habitación.\n");

        getchar();

        printf("Dentro del pasillo.\n");

        getchar();

        if(instancia == 0) {

            quiere[0] = 1;
            printf("He accionado el pulsador.\n");

            getchar();
            printf("Intentando acceder a la Sección Crítica...");

            if(quiere[1] == 0) {

                printf("Dentro de mi Sección Crítica.\n");
                
                getchar();
                printf("He salido de mi Sección Crítica.\n");
                    
                getchar();
                quiere[0] = 0;
                printf("He accionado el pulsador.\n");
                
            } else {

                printf("Puerta cerrada. Saliendo del pasillo...\n");

                getchar();
                quiere[0] = 0;
                printf("He accionado el pulsador.\n");
                printf("He salido del pasillo.\n");

            }

        } else {

            quiere[1] = 1;
            printf("He accionado el pulsador.\n");

            getchar();
            printf("Intentando acceder a la Sección Crítica...");

            if(quiere[0] == 0) {

                printf("Dentro de mi Sección Crítica.\n");
                    
                getchar();
                printf("He salido de mi Sección Crítica.\n");
                    
                getchar();
                quiere[1] = 0;
                printf("He accionado el pulsador.\n");
                
            } else {

                printf("Puerta cerrada. Saliendo del pasillo...\n");

                getchar();
                quiere[1] = 0;
                printf("He accionado el pulsador.\n");
                printf("He salido del pasillo.\n");

            }

        }

    }
    

    //NOTA: Al tener que detener el proceso con ctrl+C, no se puede desvincular y eliminar el segmento, si se detiene y se vuelve a ejecutar  más de dos veces (con la misma clave) estará todo el rato sin poder volver a la instancia 0; 
    //Deberá cambiarse la clave para crear un nuevo segmento de memoria compartida, y el anterior quedará sin liberar.

    //No se viola la EXCLUSIÓN MUTUA, cuando uno de los procesos indica que quiere entrar, se detecta contienda y ámbos salen del pasillo para volver a intentarlo (ahora o en otro momento).

    //No se produce INTERBLOQUEO, no hay una situación posible en el que los procesos se queden bloqueados sin poder avanzar en su ejecución.

    //INANICIÓN: puede darse en UNO de los procesos cuando el otro accede a su Sección Crítica y vuelve a entrar en ella (en el mismo cuanto) antes de que el primero pueda entrar, de manera indefinida.
    //INANICIÓN: puede darse en ÁMBOS cuando quieren acceder constantemente a su Sección Crítica y se produce contienda (no se ponen de acuerdo para entrar y vuelven a intentarlo) de manera indefinida.

}