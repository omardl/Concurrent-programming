#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>


int  vector_atendidas[20][6]={0};


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

    int id_nodos[100000];
    key_t NodosExternos[10000];
    key_t key_minodo= ftok("/bin/ls",50); 
    int id_global=msgget(key_minodo,0777 | IPC_CREAT); 

    for(int v=0;v<NumNodos;v++){
          NodosExternos[v]= ftok("/bin/ls",v);
          id_nodos[v]=msgget(NodosExternos[v],0777 | IPC_CREAT);           
  	      printf("Id obtenido, nodo %d--> id %d\n",v,id_nodos[v]);     
    }

    for(int a=0;a<8;a++){
        for(int i=0;i<NumNodos;i++){
            for(int v=0;v<1000;v++){
                struct msgbuff salida;
                msgrcv(id_nodos[i],&salida,sizeof(salida),v,IPC_NOWAIT);  
            }
        }

        for(int v=0;v<1000;v++){
            struct msgbuff salida;
            msgrcv(id_global,&salida,sizeof(salida),v,IPC_NOWAIT);  
        }
    }
}