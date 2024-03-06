
#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>


/*
  Peticion: tipo = 1
  Testigo: tipo = 2
  Minitestigo: tipo = 3
*/
struct text{
    // Cabecera [tipo, id_origen, prioridad, Nº peticion]
    int cabecera[4];
    int peticiones_atendidas[20][5];
};


struct msgbuff{
    long mtype;
    struct text texto;
};


int main(int argc, char * argv[]){

    int NumNodos=atoi(argv[1]);
    int NumMinodo= atoi(argv[2]);
    int prioridad=atoi(argv[3]);

    // Se obtiene el id del resto de nodos
    int id_nodos[100000];
    key_t NodosExternos[10000];

    for(int v=0;v<NumNodos;v++){
        NodosExternos[v]= ftok("/bin/ls",v); 
        id_nodos[v]=msgget(NodosExternos[v],0777 | IPC_CREAT);         
  	    printf("Id obtenido, nodo %d--> id %d\n",v,id_nodos[v]);
    }

    struct msgbuff lanzar;
    lanzar.mtype=100;//para mandarlo a otro nodo
    lanzar.texto.cabecera[0]=prioridad;
      
    printf("Se lanza en %d un proceso de prio %d\n",id_nodos[NumMinodo],lanzar.texto.cabecera[0]);

    msgsnd(id_nodos[NumMinodo],&lanzar,sizeof(lanzar),0);
}