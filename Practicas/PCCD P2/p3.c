#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>

int main() {

    int estado_hijo;

    pid_t hijo;

    for(int i = 0; i < 3; i++) {

        if((hijo = fork()) == 0) {

            sleep(1);
            exit(EXIT_SUCCESS);

        } else {

            hijo = waitpid(hijo, &estado_hijo, 0);

        }

        printf("\nHa finalizado el hijo con el pid %d y el codigo %d \n", hijo, estado_hijo);

    }

    return 0;

}