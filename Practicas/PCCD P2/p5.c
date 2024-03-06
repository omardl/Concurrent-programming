#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int contador = 0;
int estado;
int pid_Jordi, pid_Thiago_Silva;
int acaba = 0;

int main() {

    printf("De Gea ");

    if(fork() == 0) {

        printf("Ramos ");
        exit(EXIT_SUCCESS);

    } else {

        if(fork() == 0) {

            printf("Piqué ");
            exit(EXIT_SUCCESS);

        } else {

            if((pid_Jordi = fork()) == 0) {

                printf("Jordi Alba ");
                exit(EXIT_SUCCESS);

            } else {

                printf("Carvajal ");

                while(contador < 3) {

                    int pid = wait(&estado);

                    if (pid == pid_Jordi) {

                        if((pid_Thiago_Silva = fork()) == 0) {

                            printf("Thiago ");
                            printf("Silva ");
                            exit(EXIT_SUCCESS);
                
                        }

                    } 

                    if (pid != pid_Thiago_Silva) {

                        contador++;
                    
                    } else {

                        acaba++;

                    }

                }

                printf("Busquets ");
                printf("Isco ");
                printf("Aspas ");

                if(!acaba) {
                    wait(&estado);
                }

                printf("Morata\n");

            }
        }
    }

    return 0;
    
}
