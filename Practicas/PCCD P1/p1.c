#include <unistd.h>
#include <stdio.h>

//Proceso p1 que espera a recibir una señal. Se prueba cada señal definida para comprobar la acción por defecto de cada una.

int main()  {

   if (pause() == -1) {

      printf("error");

   } 

   return 0;

}