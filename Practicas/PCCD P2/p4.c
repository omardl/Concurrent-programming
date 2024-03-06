#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>

int main(int argc, char *argv[]) {

    int estado_hijo;

    int pid_hijo;

    //Le decimos al hijo varios parametros para que pase

    for(int i = 0; i < 3; i++) {

        if(fork() == 0) {
            
           execl("p4child", "voy", "a", "aprobar","pccd", (char*) 0);
            
        } else {

            pid_hijo = wait(&estado_hijo);

        }

        printf("\nHa finalizado el hijo con el pid %d y el codigo %d \n", pid_hijo, estado_hijo);

    }

    return 0;

}