#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>


int  vector_atendidas[20][5]={0};


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

  // Obtencion del ID del resto de nodos
  int id_nodos[100000];
  key_t NodosExternos[10000];

  for(int v=0;v<NumNodos;v++){
    NodosExternos[v]= ftok("/bin/ls",v); 
    id_nodos[v]=msgget(NodosExternos[v],0777 | IPC_CREAT);         
    printf("Id obtenido, nodo %d--> id %d\n",v,id_nodos[v]);      
  }
      
  // Creacion del contador global
  key_t key_minodo= ftok("/bin/ls",50); 
  int id_global=msgget(key_minodo,0777 | IPC_CREAT); 

  struct msgbuff token;

  token.mtype=99;//para mandarlo a otro nodo
  token.texto.cabecera[0]=2;

  for(int i=0;i<20;i++){
    for(int j=0;j<5;j++){
      token.texto.peticiones_atendidas[i][j]=vector_atendidas[i][j];
    }
  }

  printf("Mandando la cabecera %d \n",token.texto.cabecera[0]);

  // Se envia el token al que lo pidio
  int x= msgsnd(id_nodos[1],&token,sizeof(token),0);
  struct msgbuff contador;

  contador.mtype=200;
  contador.texto.cabecera[0]=0;

  msgsnd(id_global,&contador,sizeof(contador),0);

  printf("Token enviado al nodo 1 de id %d\n",id_nodos[1]);
  printf("%d\n",x);
}