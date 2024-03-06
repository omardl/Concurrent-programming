#include <signal.h>
#include <unistd.h>
#include <stdio.h>

void handler(int signal);

int hijos = 0;
int contador = 0;

int main() {

    struct sigaction padre;
    padre.sa_handler = handler;
    sigaction(17, &padre, NULL);


    if(fork() == 0) {

        sleep(3);
        return 0;

    } else {

        if(fork() == 0) {

            sleep(2);
            return 0;

        } else {

            if(fork() == 0) {

                sleep(1);
                return 0;

            } else {

                while(hijos < 3) {

                    pause();

                    printf("Finaliza un hijo.\n");

                }

            }
        }
    }

    printf("Han finalizado %d hijos.\n", hijos);

    return 0;
    
}

void handler(int signal) {

    hijos++;

}