#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main() {

    int id_segmento;

    printf("\nDeben ejecutarse SÓLO 2 instancias del programa CON LA MISMA CLAVE para evitar errores, ya que a partir de la primera ejecución se abre Q.\n");

    //SI ES LA PRIMERA INSTANCIA DE P7, SERA LA INSTANCIA 0 (PROCESO P), SI ES LA SEGUNDA, SERÁ LA 1 (PROCESO Q)    
    int instancia = 0;

    int *quiere;

    id_segmento = shmget(371, sizeof(int)*2, IPC_EXCL|IPC_CREAT|0777);

    if(id_segmento == -1) {
        
        //Si da error porque el segmento ya existe, accedemos a el (segunda instancia de p5)
        id_segmento = shmget(371, sizeof(int),0);
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

        getchar();

        printf("Dentro del pasillo.\n");

        getchar();

        if(instancia == 0) {

            quiere[0] = 1;
            printf("He accionado el pulsador.\n");

            while(!sigue_P) {

                getchar();
                printf("Intentando acceder a la Sección Crítica...");

                if(quiere[1] == 0) {

                    printf("Dentro de mi Sección Crítica.\n");
                    sigue_P++;
                    
                    getchar();
                    printf("He salido de mi Sección Crítica.\n");
                    
                    getchar();
                    quiere[0] = 0;
                    printf("He accionado el pulsador.\n");
                
                } else {

                    printf("Puerta cerrada.\n");

                }

            }

            sigue_P--;

        } else {

            quiere[1] = 1;
            printf("He accionado el pulsador.\n");

            while(!sigue_Q) {

                getchar();
                printf("Intentando acceder a la Sección Crítica...");

                if(quiere[0] == 0) {

                    printf("Dentro de mi Sección Crítica.\n");
                    sigue_Q++;
                    
                    getchar();
                    printf("He salido de mi Sección Crítica.\n");
                    
                    getchar();
                    quiere[1] = 0;
                    printf("He accionado el pulsador.\n");
                
                } else {

                    printf("Puerta cerrada.\n");

                }

            }

            sigue_Q--;

        }

    }
    

    //NOTA: Al tener que detener el proceso con ctrl+C, no se puede desvincular y eliminar el segmento, si se detiene y se vuelve a ejecutar  más de dos veces (con la misma clave) estará todo el rato sin poder volver a la instancia 0; 
    //Deberá cambiarse la clave para crear un nuevo segmento de memoria compartida, y el anterior quedará sin liberar
    

    //No se viola la EXCLUSIÓN MUTUA: Si un proceso va a entrar y acciona el pulsador, en ningun momento podrá entrar en la Sección Crítica el otro proceso hasta que el primero vuelva a accionar el puslador al salir.

    //No se produce INANICIÓN: Cuando un proceso (Q) solicita entrar, en cuanto el otro (P) salga de su Sección Crítica podrá entrar (Q) sin problemas. Y el que ha salido (P) no  podrá entrar hasta que haya entrado y salido el primero (Q).

    //INTERBLOQUEO: Cuando el primer proceso acciona el pulsador y el segundo lo acciona también sin que el primero llegase a la puerta, ámbos se encontrarán las puertas cerradas (NO PUEDEN PROSEGUIR SU EJECUCIÓN).


}