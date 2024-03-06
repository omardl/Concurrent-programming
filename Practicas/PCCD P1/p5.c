#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void handler_salida(int signal);
void handler_bajada(int signal);
void handler_subida(int signal);

int fin = 0;
int piso_actual = 1;
int ultimo_piso;
int subidas = 0;
int bajadas = 0;
int flag_subida, flag_bajada;


//ASCENSOR (RECIBE EL NUMERO DE PISOS EN LA LLAMADA)
int main(int argc ,char *argv[]) {

    ultimo_piso = atoi(argv[1]);
    
    system("echo ASCENSOR EN MARCHA");

    struct sigaction subida;
    subida.sa_handler = handler_subida;

    struct sigaction bajada;
    bajada.sa_handler = handler_bajada;

    struct sigaction salida; 
    salida.sa_handler = handler_salida;

    sigaction(3, &salida, NULL);
    sigaction(10, &subida, NULL);
    sigaction(12, &bajada, NULL);
    
    flag_bajada = 0;
    flag_subida = 0;
    
    int pid_sensor;
    printf("\nIntroduce el PID del sensor: ");
    scanf("%d", &pid_sensor);

    while (!fin) {

        pause();

        if (flag_subida == 1) {

            kill(pid_sensor, SIGUSR1);

            if (piso_actual < ultimo_piso) {
                
                subidas++;
                piso_actual++;
            
            } 

            flag_subida = 0;

        }

        if (flag_bajada == 1) {

            kill(pid_sensor, SIGUSR2);

            if (piso_actual > 1) {
            
                bajadas++;
                piso_actual--;
            
            } 

            flag_bajada = 0;

        }

    }

    kill(pid_sensor, SIGQUIT);
    
    printf("\nCerrando ascensor...");
    printf("\nEl ascensor ha subido %i veces y ha bajado %i.\n", subidas, bajadas);

    return 0;

}


//NOTA: en la rutina de servicio debe haber el menor codigo posible.
//NOTA 2: Es mejor un handler para cada señal.

void handler_subida(int signal) {

    flag_subida = 1;

}

void handler_bajada(int signal) {

    flag_bajada = 1;

}

void handler_salida(int signal) {

    fin = 1;

}