#include <signal.h>
#include <unistd.h>
#include <stdio.h>

int bits[64];
int fin = 0;
int resultado;

void handler(int signal);

int main() {

    //Se inicializa el vector bits a 0
    for(int i = 0; i < 64; i++) {
        bits[i] = 0;
    }


    //Creamos la estructura sigaction para su funcion y le asignamos nuestro propio handler
    struct sigaction sigaction_param;

    sigaction_param.sa_handler = handler;

    
    for (int j = 0; j < 64; j++) {

        sigaction(j, &sigaction_param, NULL);  //Le asignamos el handler a cada señal (aunque la 9 y la 19 no pueden modificarse realmente)

    }
     
    //Mientras el handler no detecte la señal 15 (SIGTERM) esperaremos por otra señal y cambiaremos el bit correspondiente del vector a 1
    while(!fin) {
        pause();
    }


    printf("\nVector bits: [");
    for (int i = 0; i < 64; i++) {

        printf(" %i ", bits[i]);

    }
    printf("] \n");
     
    return 0;

}
//NOTA: SIGACTION NO FUNCIONA TEORICAMENTE CON SIGKILL (9) NI SIGSTOP (19)
//SIGKILL MATA EL PROCESO Y NO SE IMPRIME EL VECTOR
//SIGSTOP PAUSA EL PROCESO HASTA QUE SE LE ENVIA UNA SEÑAL SIGCONT (18)
void handler(int signal) {

    bits[signal - 1] = 1;

    switch(signal) {
        
        case 15:
        fin = 1;
        break;

        default: 
        break;
    }

}