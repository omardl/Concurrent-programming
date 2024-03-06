#include <signal.h>
#include <unistd.h>
#include <stdio.h>

int fin = 0;
int cont_usr1, cont_usr2;

void handler(int signal);

int main() {

    cont_usr1 = 0;
    cont_usr2 = 0;

    struct sigaction sigaction_param;

    //Creamos una mascara, la ponemos a 1, y dejamos pasar la 10, 12 y 15, que son las que usa nuestro programa.
    sigset_t mascara;
    sigfillset(&mascara);
    sigdelset(&mascara, SIGUSR1);
    sigdelset(&mascara, SIGUSR2);
    sigdelset(&mascara, SIGTERM);

    sigprocmask(SIG_SETMASK, &mascara, NULL);


    //Creamos un handler para las señales 10, 12 y 15 y ponemos el flag SA_RESTART
    sigaction_param.sa_handler = handler;

    //Si es interrumpida y abortada, se vuelve a iniciar.
    sigaction_param.sa_flags = SA_RESTART;

    sigaction(10, &sigaction_param, NULL);
    sigaction(12, &sigaction_param, NULL);
    sigaction(15, &sigaction_param, NULL);
    
    while(!fin) {
        pause();
    }

    printf("\nNúmero de señales SIGUSR1 recibidas: %i", cont_usr1);
    printf("\nNumero de señales SIGUSR2 recibidas: %i \n", cont_usr2);

    return 0;

}


void handler(int signal) {

    switch (signal) {

        //SIGUSR1
        case 10: cont_usr1 ++;
        break;

        //SIGUSR2
        case 12: cont_usr2 ++;
        break;
        
        //SIGTERM
        case 15: fin = 1;
        break;

        default:
        break;
    }
}