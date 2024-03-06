#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>

int main() {

    int estado_hijo;
    int pid_hijo;

    for(int i = 0; i < 3; i++) {

        if(fork() == 0) {

            sleep(1);
            exit(EXIT_SUCCESS);

        } else {

            pid_hijo = wait(&estado_hijo);
        
        }

        printf("\nHa finalizado el hijo con el pid %d y el codigo %d \n", pid_hijo, estado_hijo);

    }

    return 0;

}