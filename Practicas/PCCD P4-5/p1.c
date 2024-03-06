#include <pthread.h> 
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

void *hilo_lectura_teclado();

char caracter = '0';
int contador = 0;

int main() {

    //Variable que identifica a un hilo. Permite controlarlo una vez entra en ejecución.
    pthread_t hilo1;

    printf("Introduce los caracteres que desees (máx.:100). Pulsa la tecla q para salir.\n");
    

    /*Crea un nuevo hilo; para COMPILAR debe incluirse -l pthread que incluiría la libreria externa con las funciones y tipos para el uso de hilos
    Parámetros: 
        1 - pthread_t *thread   (direccion de la variable pthread) 

        2 - const pthread_attr_t *attr  (direccion que a punta a una struct pthread_attr_t, que identfica los atributos del nuevo hilo. Se usa la funcion pthread_attr_init() para inicializar la estructura.
        Si es NULL, tendrá los atributos por defecto).

        3 - void *(*start_routine) (void *)   (puntero a la función con la subrutina que ejecuta el hilo)

        4 - void *arg    (dirección de los argumentos a pasar en la llamada al hilo, si los hubiera)

    Valor devuelto: 0 si no hay error.
    */ 
    if (pthread_create(&hilo1, NULL, hilo_lectura_teclado, NULL) != 0) {
   
        printf("Error en la creación del hilo.\n");
        return 0;

    }

    while(caracter != 'q') {   

        sleep(1);

        printf("Se han introducido %d caracteres del teclado.\n", contador); 

        contador = 0;

    }

    

    //pthread_exit(0);

    printf("Se ha pulsado la tecla q.\n Saliendo del programa...\n"); //ESTO NO SE IMPRIME, POR QUË?

    return 0;

}

void *hilo_lectura_teclado() {

    while(caracter != 'q'){


        caracter = getchar();

        //Descontamos la tecla ENTER 
        if (caracter == '\n') {
            contador--;
        }

        contador ++;

    }

    exit(0); //diferencia entre salir con exit o pthread_exit????? no está clara
    
}