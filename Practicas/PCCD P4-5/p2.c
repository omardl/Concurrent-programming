#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct parametros {

    int id_parametro;
    char *parametro;

};

void *funcion_hilo(struct parametros *param);

int contador = 0;

int main(int argc, char *argv[]) {

    contador = argc - 1;

    struct parametros parametro_n[argc - 1];

    for (int i = 1; i < argc; i++) {

        pthread_t hilo;
        
        parametro_n[i-1].id_parametro = i;
        parametro_n[i-1].parametro = argv[i];



        if(pthread_create(&hilo, NULL, (void *) funcion_hilo, (void *) &parametro_n[i-1]) != 0) {

            printf("Error creando el hilo.\n");
            return 0; 
        
        }      
    }

    //Esperamos a que los threads acaben, ya que no nos indica el uso de pthread_join hasta el siguiente ejercicio, implementamos un contador
    while(contador != 0) {
        sleep(1); //Esto no se debe hacer (espera activa) 
    }
    
    return 0;

}

void *funcion_hilo(struct parametros *param) {

    printf("Parametro número %d: %s\n", param -> id_parametro, param -> parametro);
    
    contador--;

    pthread_exit(NULL);

}