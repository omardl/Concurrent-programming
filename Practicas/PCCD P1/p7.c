#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h> 

void handler_subida(int signal);
void handler_bajada(int signal);
void handler_salida(int signal);

//Como no se especifica T_PISO, usamos, p.e., 2seg
int T_PISO = 2;
int piso_actual = 1;
int numero_plantas;
int fin = 0;
int flag_subida, flag_bajada;

//RECIBE EL NUMERO DE PISOS EN LA LLAMADA
int main(int argc, char *argv[]) {

    numero_plantas = atoi(argv[1]);
    int pid_ascensor;

    printf("\nIntroduce el PID del ascensor: ");
    scanf("%d", &pid_ascensor);

    struct sigaction subida;
    subida.sa_handler = handler_subida;

    struct sigaction bajada;
    bajada.sa_handler = handler_bajada;

    struct sigaction salida;
    salida.sa_handler = handler_salida;

    sigaction(3, &salida, NULL);
    sigaction(10, &subida, NULL);
    sigaction(12, &bajada, NULL);

    flag_subida = 0;
    flag_bajada = 0;

    while(!fin) {

        pause();
        
        if(flag_subida == 1) {

            if (piso_actual < numero_plantas) {

                system("echo ...Subiendo...");
                piso_actual++;
                sleep(T_PISO);
                char salida[30] = "echo Has llegado a la planta ";
                char planta[5];
                sprintf(planta, "%d", piso_actual);
                strcat(salida, planta);
                system(salida);

            } else {

                system("echo Ya estás en el último piso.");

            }

            flag_subida = 0;
 
        }

        if (flag_bajada == 1) {

            if (piso_actual > 1) {

                system("echo ...Bajando...");
                piso_actual--;
                sleep(T_PISO);
                sleep(T_PISO);
                char salida[30] = "echo Has llegado a la planta ";
                char planta[5];
                sprintf(planta, "%d", piso_actual);
                strcat(salida, planta);
                system(salida);

            } else {

                system("echo Ya estás en el primer piso.");

            }

            flag_bajada = 0;

        }

    }

    return 0;
}

void handler_subida(int signal) {

    flag_subida = 1;

}

void handler_bajada(int signal) {

    flag_bajada = 1;

}

//EL SENSOR DEBERIA ESTAR EN PERMANENTE FUNCIONAMIENTO, 
//UNICAMENTE SE IMPLEMENTA LA OPCION PARA CERRAR EL PROCESO DE MANERA SEGURA
void handler_salida(int signal) {
    
    fin = 1;

}