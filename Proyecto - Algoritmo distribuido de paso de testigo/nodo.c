#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>


int N=3;
int NumMinodo=0;
int NumNodos=0;
int NumProcesos=0;
int id_minodo;
int id_global=0;
int id_emisor_minitoken=0;

int id_nodos[100];
int vector_atendidas[20][5];

// Cada proceso tiene una posicion y su valor es la prioridad
int vector_peticiones_internas[100] = {0};
// Uno por prioridad
int vector_peticiones_externas[20][5];

int testigo=0;
int dentro=0;
int ContHaciaN=0;
int arrayExternas[2];
int contadorNumeroPets=0;


// Variables lectores
int acabaronMisConsultas=0;
int contadorConsultasDentro=0;
int MiniToken=0;

int modoConsultas=0;
int contadorMiniTestigos=0;


// Semaforos
sem_t sem_vector_peticiones_internas[20],sem_vector_peticiones_externas[20],sem_vector_atendidas[20];
sem_t sem_testigo[20],sem_dentro[20],sem_ContHaciaN[20],sem_contadorNumeroPets[20],sem_arrayExternas[20];
sem_t sem_contadorConsultasDentro[20],sem_modoConsultas[20],sem_contadorMinitestigos[20],sem_MiniToken[20];


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


void * receptor(int * num);
void * proceso(int * parametros);
void * consultas(int * param);


void copiarVectores(int i){

    sem_wait(&sem_vector_atendidas[NumMinodo]);    
    vector_atendidas[i][1]=vector_peticiones_externas[i][1];
    sem_post(&sem_vector_atendidas[NumMinodo]);

    return;
}



int maxPrioridadInterna(){

    sem_wait(&sem_vector_peticiones_internas[NumMinodo]);
    int prioMasAlta=-1;

    for (int h=0;h<NumProcesos;h++){
        if(prioMasAlta<vector_peticiones_internas[h]){
            prioMasAlta=vector_peticiones_internas[h];
        }
    }

    sem_post(&sem_vector_peticiones_internas[NumMinodo]);    
    return prioMasAlta;
}



void eligeNodoSiguiente(){

    arrayExternas[0]=-1;
    arrayExternas[1]=-1;
    int id_aux=-1;
    int id_destino=-1;
    int maximoExternas=-1;
    int x=0;
    
    for(int j=5;j>0;j--){         

        for(int i=0;i<NumNodos-1;i++){ 

            id_aux=id_nodos[i];
            
            sem_wait(&sem_vector_atendidas[NumMinodo]);
            sem_wait(&sem_vector_peticiones_externas[NumMinodo]);
            
            if(vector_peticiones_externas[id_aux][j]>vector_atendidas[id_aux][j] && x!=1){ 
            
                sem_post(&sem_vector_atendidas[NumMinodo]);
                sem_post(&sem_vector_peticiones_externas[NumMinodo]);
                
                maximoExternas=j;
                id_destino=id_aux;            
                x=1;

            } else{sem_post(&sem_vector_atendidas[NumMinodo]);

                sem_post(&sem_vector_peticiones_externas[NumMinodo]);
            }
        } 

        sem_wait(&sem_arrayExternas[NumMinodo]);
        arrayExternas[0]=id_destino;
        arrayExternas[1]=maximoExternas;
        sem_post(&sem_arrayExternas[NumMinodo]);
    }
   
    return;
}



int aumentarContadorGlobal(){

    struct msgbuff contador;
    msgrcv(id_global,&contador,sizeof(contador),200,0);
    struct msgbuff contador2;
    contador2.mtype=200;
    contador2.texto.cabecera[0]=contador.texto.cabecera[0]+1;
    msgsnd(id_global,&contador2,sizeof(contador2),0);
    return contador2.texto.cabecera[0];
}



int MAX (int x,int y) {
    if(x==NULL){
        x=0;
    }
    if(x<y){
        return y;
    }else{
        return x;
    }
}



void send_token(int id_destino){
    
    struct msgbuff testigoEnviado;

    // Para mandarlo a otro nodo
    testigoEnviado.mtype=99;
    testigoEnviado.texto.cabecera[0]=2;
    sem_wait(&sem_vector_atendidas[NumMinodo]);

    for(int i=0;i<20;i++){
        for(int j=0;j<5;j++){
            testigoEnviado.texto.peticiones_atendidas[i][j]=vector_atendidas[i][j];
        }
    }

    sem_post(&sem_vector_atendidas[NumMinodo]);
    // Se envia el token al que lo pidio
    msgsnd(id_destino,&testigoEnviado,sizeof(testigoEnviado),0);
    return;
}



int main(int argc, char * argv[]){
    
    NumNodos=atoi(argv[1]);
    NumMinodo=atoi(argv[2]);

    // Creacion de semaforos
    sem_init(&sem_vector_peticiones_internas[NumMinodo],0,1);
    sem_init(&sem_vector_peticiones_externas[NumMinodo],0,1);
    sem_init(&sem_vector_atendidas[NumMinodo],0,1);
    sem_init(&sem_testigo[NumMinodo],0,1);
    sem_init(&sem_dentro[NumMinodo],0,1);
    sem_init(&sem_ContHaciaN[NumMinodo],0,1);
    sem_init(&sem_contadorNumeroPets[NumMinodo],0,1);
    sem_init(&sem_arrayExternas[NumMinodo],0,1);
    sem_init(&sem_contadorConsultasDentro[NumMinodo],0,1);
    sem_init(&sem_modoConsultas[NumMinodo],0,1);
    sem_init(&sem_contadorMinitestigos[NumMinodo],0,1);
    sem_init(&sem_MiniToken[NumMinodo],0,1);
  
    // ID del nodo
    key_t key=ftok("/bin/ls",NumMinodo);
    id_minodo=msgget(key,0777 | IPC_CREAT);


    int i=0;
    pthread_t pthtoken;

    // Cracion del hilo donde se recibirá el testigo, peticiones....    
    pthread_create(&pthtoken,NULL,(void *)receptor,NULL);

    // Se obtiene el id del resto de nodos
    key_t NodosExternos[10000];
    int j=0;
    for(int v=0;v<NumNodos;v++){
        if(v!=NumMinodo){
            NodosExternos[j]= ftok("/bin/ls",v); 
            id_nodos[j]=msgget(NodosExternos[j],0777 | IPC_CREAT);        
            j++;
        }
    }

    // Creacion del contador global
    key_t global= ftok("/bin/ls",50); 
    id_global=msgget(global,0777 | IPC_CREAT); 
    
    int x=1;
    while(x){
        struct msgbuff lanzar; 
        msgrcv(id_minodo,&lanzar,sizeof(lanzar),100,0);
        pthread_t pth;
        int parametros[2];
        parametros[0]=NumProcesos;
        parametros[1]=lanzar.texto.cabecera[0];

        if(parametros[1]==100){
            x=0;
        }
        
        if(parametros[1]==1){
            pthread_create(&pth,NULL,(void *)consultas,parametros);
        } else{
            pthread_create(&pth,NULL,(void *)proceso,parametros);
        }

        NumProcesos++;
    }
}



void * receptor(int * num){

    // Espera de peticiones y testigo
    while(1){

        struct msgbuff buffer;
        msgrcv(id_minodo,&buffer,sizeof(buffer),99,0);    
            
        // Peticion
        if(buffer.texto.cabecera[0]==1){

            int id_origen=buffer.texto.cabecera[1];
            int prio_peticion=buffer.texto.cabecera[2];
            int num_peticion=buffer.texto.cabecera[3];

            // Se comprueba la mayor peticion almacenada
            sem_wait(&sem_vector_peticiones_externas[NumMinodo]);
            vector_peticiones_externas[id_origen][prio_peticion]=MAX(vector_peticiones_externas[id_origen][prio_peticion],num_peticion);
            sem_post(&sem_vector_peticiones_externas[NumMinodo]);

            sem_wait(&sem_modoConsultas[NumMinodo]);

            if(modoConsultas==1 && prio_peticion>1){
                modoConsultas=0;
            }
            
            sem_post(&sem_modoConsultas[NumMinodo]);
            sem_wait(&sem_modoConsultas[NumMinodo]);
            sem_wait(&sem_testigo[NumMinodo]);

            // Se dan paso a las consultas
            if(modoConsultas==1 && prio_peticion==1 && testigo==1){
            
                sem_post(&sem_modoConsultas[NumMinodo]);
                sem_post(&sem_testigo[NumMinodo]);

                struct msgbuff mini_token;
                mini_token.mtype=99;
                mini_token.texto.cabecera[0]=3;
                mini_token.texto.cabecera[1]=id_minodo;
                copiarVectores(id_origen);
                msgsnd(id_origen,&mini_token,sizeof(mini_token),0);

                sem_wait(&sem_contadorMinitestigos[NumMinodo]);
                contadorMiniTestigos++;
                sem_post(&sem_contadorMinitestigos[NumMinodo]);

            } else{sem_post(&sem_modoConsultas[NumMinodo]);
                sem_post(&sem_testigo[NumMinodo]);
            }


            // Recepcion de una peticion cuando el nodo tiene le token pero ya no lo esta usando
            sem_wait(&sem_testigo[NumMinodo]);
            sem_wait(&sem_dentro[NumMinodo]);
            sem_wait(&sem_vector_peticiones_externas[NumMinodo]);
            sem_wait(&sem_vector_atendidas[NumMinodo]);

            if(testigo==1 && dentro==0 && vector_peticiones_externas[id_origen][prio_peticion] > vector_atendidas[id_origen][prio_peticion]){
                
                sem_post(&sem_vector_peticiones_externas[NumMinodo]);
                sem_post(&sem_testigo[NumMinodo]);
                sem_post(&sem_dentro[NumMinodo]);
                sem_post(&sem_vector_atendidas[NumMinodo]);
                
                sem_wait(&sem_arrayExternas[NumMinodo]);
                send_token(id_origen);
                sem_post(&sem_arrayExternas[NumMinodo]);

                sem_wait(&sem_testigo[NumMinodo]);
                testigo=0; 
                sem_post(&sem_testigo[NumMinodo]);  

            } else{

                sem_post(&sem_vector_peticiones_externas[NumMinodo]);
                sem_post(&sem_vector_atendidas[NumMinodo]);
                sem_post(&sem_testigo[NumMinodo]);
                sem_post(&sem_dentro[NumMinodo]);

            }     
        }
        
        

        // Recepcion del testigo
        if(buffer.texto.cabecera[0]==2){ 

            sem_wait(&sem_testigo[NumMinodo]);
            testigo=1;
            sem_post(&sem_testigo[NumMinodo]);
            printf("++++TESTIGO RECIBIDO++++\n");
            
            // Anotacion de las peticiones atendidas que que se reciben con el token como informacion
            sem_wait(&sem_vector_atendidas[NumMinodo]);
             for(int i=0; i<10;i++){
                for(int j=0;j<5;j++){
                    vector_atendidas[i][j]=buffer.texto.peticiones_atendidas[i][j];
                }
            }    
            sem_post(&sem_vector_atendidas[NumMinodo]);

            // Buscar en el vector de peticiones internas cual es el mas prioritario, y darle el paso 
            int prioMasAlta=maxPrioridadInterna();

            if(prioMasAlta>0){

                struct msgbuff darPaso;
                darPaso.mtype=prioMasAlta;

                if(prioMasAlta==1){

                    sem_wait(&sem_modoConsultas[NumMinodo]);
                    modoConsultas=1;
                    sem_post(&sem_modoConsultas[NumMinodo]);
                
                } else{
                
                    sem_wait(&sem_modoConsultas[NumMinodo]);
                    modoConsultas=0;
                    sem_post(&sem_modoConsultas[NumMinodo]);
        
                }
    
                msgsnd(id_minodo,&darPaso,sizeof(darPaso),0);
            }
        }



        sem_wait(&sem_testigo[NumMinodo]);

        // Si se recibe un minitestigo, todos los nodos pueden realizar el proceso (no es concurrente)
        if(testigo==0 && buffer.texto.cabecera[0]==3){

            sem_post(&sem_testigo[NumMinodo]);

            sem_wait(&sem_modoConsultas[NumMinodo]);
            modoConsultas=1;
            sem_post(&sem_modoConsultas[NumMinodo]);

            printf("++++MINITESTIGO RECIBIDO++++\n");
            id_emisor_minitoken=buffer.texto.cabecera[1];

            sem_wait(&sem_MiniToken[NumMinodo]);
            MiniToken=1;
            sem_post(&sem_MiniToken[NumMinodo]);
            

            struct msgbuff darPaso;
            darPaso.mtype=1;
            msgsnd(id_minodo,&darPaso,sizeof(darPaso),0);           

        } else{
            sem_post(&sem_testigo[NumMinodo]);
        }

        
        
        sem_wait(&sem_testigo[NumMinodo]);
        sem_wait(&sem_dentro[NumMinodo]);
        
        // Testigo en desuso
        if(testigo==1 && buffer.texto.cabecera[0]==27 && dentro==0){

            sem_post(&sem_testigo[NumMinodo]);
            sem_post(&sem_dentro[NumMinodo]);

            int prioMasAlta=maxPrioridadInterna();

            if(prioMasAlta>0){

                if(prioMasAlta==1){
                    sem_wait(&sem_modoConsultas[NumMinodo]);
                    modoConsultas=1;
                    sem_post(&sem_modoConsultas[NumMinodo]);
                }

                struct msgbuff darPaso;
                darPaso.mtype=prioMasAlta;
                msgsnd(id_minodo,&darPaso,sizeof(darPaso),0);
            }
        } else{
            sem_post(&sem_testigo[NumMinodo]);
            sem_post(&sem_dentro[NumMinodo]);
        }       


        
        sem_wait(&sem_testigo[NumMinodo]);      
        
        // Finalizan los procesos no concurrentes (proceso de consultas)

        if(testigo==1 && buffer.texto.cabecera[0]==4){
            
            printf("++++LAS CONSULTAS EN ESTE NODO HAN ACABADO++\n");
            sem_post(&sem_testigo[NumMinodo]);
            acabaronMisConsultas=1;
            
            if(contadorMiniTestigos==0 && acabaronMisConsultas==1){   
                        
                eligeNodoSiguiente();                
                int prioExternas=arrayExternas[1];
                
                int prioInternas=maxPrioridadInterna();
               
                // Peticion externa con mayor prioridad
                if(prioExternas>prioInternas){
                
                    if(prioInternas>0){

                        sem_wait(&sem_contadorNumeroPets[NumMinodo]);
                        contadorNumeroPets++;
                        sem_post(&sem_contadorNumeroPets[NumMinodo]);

                        struct msgbuff peticion;
                        peticion.mtype=99;
                        peticion.texto.cabecera[0]=1;
                        peticion.texto.cabecera[1]=id_minodo;
                        peticion.texto.cabecera[2]=prioInternas;
                        peticion.texto.cabecera[3]=contadorNumeroPets;
                        
                        for(int a=0;a<NumNodos-1;a++){               
                                int x= msgsnd(id_nodos[a],&peticion,sizeof(peticion),0);       
                        }
                    } 

                    sem_wait(&sem_arrayExternas[NumMinodo]);
                    send_token(arrayExternas[0]);
                    sem_post(&sem_arrayExternas[NumMinodo]);

                    sem_wait(&sem_testigo[NumMinodo]);
                    testigo=0;
                    sem_post(&sem_testigo[NumMinodo]);

                    acabaronMisConsultas=0;
                } 

                if(prioExternas>0 && prioInternas>0){
                    if(prioInternas>=prioExternas){
                        sem_wait(&sem_modoConsultas[NumMinodo]);
                        modoConsultas=0;
                        sem_post(&sem_modoConsultas[NumMinodo]);

                        struct msgbuff darPaso;
                        darPaso.mtype=prioInternas;
                        msgsnd(id_minodo,&darPaso,sizeof(darPaso),0);
                    }
                }    
            }

        } else{
            sem_post(&sem_testigo[NumMinodo]);
        }     



        // Recopilacion de minitestigos enviados por el nodo
        sem_wait(&sem_testigo[NumMinodo]);
        
        if(testigo==1 && buffer.texto.cabecera[0]==5){
            printf("++++MINITESTIGO RECOPILADO++++\n");
            sem_post(&sem_testigo[NumMinodo]);
            
            sem_wait(&sem_contadorMinitestigos[NumMinodo]);
            contadorMiniTestigos--;
            sem_post(&sem_contadorMinitestigos[NumMinodo]);
            
            if(contadorMiniTestigos==0 && acabaronMisConsultas==1){
                
                eligeNodoSiguiente();
                int prioExternas=arrayExternas[1];
                int id_destino=arrayExternas[0];

                int prioInternas=maxPrioridadInterna();
                
                if(prioExternas>prioInternas){
                                        
                    if(prioInternas>0){
                        sem_wait(&sem_contadorNumeroPets[NumMinodo]);
                        contadorNumeroPets++;
                        sem_post(&sem_contadorNumeroPets[NumMinodo]);

                        struct msgbuff peticion;
                        peticion.mtype=99;
                        peticion.texto.cabecera[0]=1;
                        peticion.texto.cabecera[1]=id_minodo;
                        peticion.texto.cabecera[2]=prioInternas;
                        peticion.texto.cabecera[3]=contadorNumeroPets;                    


                        for(int a=0;a<NumNodos-1;a++){               
                                int x= msgsnd(id_nodos[a],&peticion,sizeof(peticion),0);                            
                        }
                    }

                    //Envio del testigo
                    send_token(id_destino);

                    sem_wait(&sem_testigo[NumMinodo]);
                    testigo=0;
                    sem_post(&sem_testigo[NumMinodo]);
                    acabaronMisConsultas=0;    
                        
                }

                if(prioExternas>0 && prioInternas>0){
                    if(prioInternas>=prioExternas){
                        sem_wait(&sem_modoConsultas[NumMinodo]);
                        modoConsultas=0;
                        sem_post(&sem_modoConsultas[NumMinodo]);

                        struct msgbuff darPaso;
                        darPaso.mtype=prioInternas;
                        msgsnd(id_minodo,&darPaso,sizeof(darPaso),0);//despierto a un proceso interno
                    }
                }    

            }
        }
        else{
            sem_post(&sem_testigo[NumMinodo]);
        }
    }   
}



void * proceso(int * param){
 
    int MiNum=(int) param[0];
    int Miprio=param[1];
    
    sem_wait(&sem_modoConsultas[NumMinodo]);

    // Hay procesos de consultas
    if(modoConsultas==1){

        modoConsultas=0;

        sem_post(&sem_modoConsultas[NumMinodo]);
              
        sem_wait(&sem_testigo[NumMinodo]);

        if(testigo==1){
            sem_post(&sem_testigo[NumMinodo]);

            sem_wait(&sem_contadorNumeroPets[NumMinodo]);

            contadorNumeroPets++;
            sem_post(&sem_contadorNumeroPets[NumMinodo]);

            struct msgbuff peticion;
            peticion.mtype=99;
            peticion.texto.cabecera[0]=1;
            peticion.texto.cabecera[1]=id_minodo;
            peticion.texto.cabecera[2]=Miprio;
            peticion.texto.cabecera[3]=contadorNumeroPets;
            
            for(int a=0;a<NumNodos-1;a++){               
                    int x= msgsnd(id_nodos[a],&peticion,sizeof(peticion),0);    
            }
        } else{
            sem_post(&sem_testigo[NumMinodo]);
        }
    } else {
        sem_post(&sem_modoConsultas[NumMinodo]);
    }

    sem_wait(&sem_testigo[NumMinodo]);

    sem_wait(&sem_dentro[NumMinodo]);

    // Aviso a mi nodo que hago una peticion interna por si esta el testigo en este y nadie en la SC
    if(testigo==1 && dentro==0) {

        sem_post(&sem_testigo[NumMinodo]);
        sem_post(&sem_dentro[NumMinodo]);
        struct msgbuff peticion;
        peticion.mtype=99;
        peticion.texto.cabecera[0]=27;
        msgsnd(id_minodo,&peticion,sizeof(peticion),0);

    } else {
        sem_post(&sem_testigo[NumMinodo]);
        sem_post(&sem_dentro[NumMinodo]);
    } 
    

    // Comprobar si se necesita realizar una peticion a otros nodos por ser este el mas prioritario,
    sem_wait(&sem_testigo[NumMinodo]);

    if(testigo==0){

        sem_post(&sem_testigo[NumMinodo]);
        int prioMasAlta=maxPrioridadInterna();

        if(Miprio>prioMasAlta){ 

            sem_wait(&sem_contadorNumeroPets[NumMinodo]);
            contadorNumeroPets++;
            sem_post(&sem_contadorNumeroPets[NumMinodo]);

            struct msgbuff peticion;
            peticion.mtype=99;
            peticion.texto.cabecera[0]=1;
            peticion.texto.cabecera[1]=id_minodo;
            peticion.texto.cabecera[2]=Miprio;
            peticion.texto.cabecera[3]=contadorNumeroPets;
            
            for(int a=0;a<NumNodos-1;a++){               
                    int x= msgsnd(id_nodos[a],&peticion,sizeof(peticion),0);                
            }
        }
    } else {
        sem_post(&sem_testigo[NumMinodo]);
    }


    sem_wait(&sem_vector_peticiones_internas[NumMinodo]);
    vector_peticiones_internas[MiNum]=Miprio;
    sem_post(&sem_vector_peticiones_internas[NumMinodo]);
    
    
    struct msgbuff meDanPaso;
    msgrcv(id_minodo,&meDanPaso,sizeof(meDanPaso),Miprio,0);

    sem_wait(&sem_dentro[NumMinodo]);
    dentro=1;
    sem_post(&sem_dentro[NumMinodo]);

    sem_wait(&sem_vector_peticiones_internas[NumMinodo]);
    vector_peticiones_internas[MiNum]=0;
    sem_post(&sem_vector_peticiones_internas[NumMinodo]);

    int numero=aumentarContadorGlobal();
   
    printf("Numero %d en entrar. Proceso %i, prioridad %i. En la seccion critica \n",numero,MiNum,Miprio);    

    sleep(1);
    
    // Peticion atendida
    sem_wait(&sem_vector_atendidas[NumMinodo]);
    vector_atendidas[id_minodo][Miprio]=contadorNumeroPets;
    sem_post(&sem_vector_atendidas[NumMinodo]);

    sem_wait(&sem_ContHaciaN[NumMinodo]);

    if (ContHaciaN<(N-1)) { 

        sem_post(&sem_ContHaciaN[NumMinodo]);
        int maximoInternas=maxPrioridadInterna();            
        
        eligeNodoSiguiente();
        int maximoExternas=arrayExternas[1];

        if (maximoExternas>maximoInternas) { 
            
            sem_wait(&sem_vector_peticiones_externas[NumMinodo]);
            vector_peticiones_externas[arrayExternas[0]][arrayExternas[1]]=0;
            sem_post(&sem_vector_peticiones_externas[NumMinodo]);

            // Peticiones internas pendientes
            int maximoInternas=maxPrioridadInterna();
          
            if(maximoInternas>0){

                contadorNumeroPets++;
                struct msgbuff peticion;
                peticion.mtype=99;
                peticion.texto.cabecera[0]=1;
                peticion.texto.cabecera[1]=id_minodo;
                peticion.texto.cabecera[2]=maximoInternas;
                peticion.texto.cabecera[3]=contadorNumeroPets;
                
                for(int a=0;a<NumNodos-1;a++){               
                        int x= msgsnd(id_nodos[a],&peticion,sizeof(peticion),0);                    
                }
            }   

            sem_wait(&sem_arrayExternas[NumMinodo]);
            send_token(arrayExternas[0]);

            sem_post(&sem_arrayExternas[NumMinodo]);

            sem_wait(&sem_testigo[NumMinodo]);
            sem_wait(&sem_dentro[NumMinodo]);
            sem_wait(&sem_ContHaciaN[NumMinodo]);

            testigo=0;
            dentro=0;
            ContHaciaN=0;

            sem_post(&sem_testigo[NumMinodo]);
            sem_post(&sem_dentro[NumMinodo]);
            sem_post(&sem_ContHaciaN[NumMinodo]);

        }

        if(maximoInternas >= maximoExternas){
    
            if(maximoInternas==1){
                
                sem_wait(&sem_modoConsultas[NumMinodo]);
                
                modoConsultas=1;
                sem_post(&sem_modoConsultas[NumMinodo]);
            }

            struct msgbuff darPaso;
            darPaso.mtype=maximoInternas;
            sem_wait(&sem_dentro[NumMinodo]);
            
            dentro=0;
            sem_post(&sem_dentro[NumMinodo]);

            msgsnd(id_minodo,&darPaso,sizeof(darPaso),0);

            sem_wait(&sem_ContHaciaN[NumMinodo]);
            
            ContHaciaN++;
            sem_post(&sem_ContHaciaN[NumMinodo]);
        }

    } else { 
        // Maximo de procesos atendidos nodo, pasamos el testigo para evitar inanicion
        sem_post(&sem_ContHaciaN[NumMinodo]);

        int maximoInternas=maxPrioridadInterna();
        eligeNodoSiguiente();
        
        sem_wait(&sem_vector_peticiones_externas[NumMinodo]);
        sem_wait(&sem_vector_atendidas[NumMinodo]);

        if(arrayExternas[1]>0){
            vector_atendidas[arrayExternas[0]][arrayExternas[1]]=vector_peticiones_externas[arrayExternas[0]][arrayExternas[1]];
        }

        sem_post(&sem_vector_peticiones_externas[NumMinodo]);
        sem_post(&sem_vector_atendidas[NumMinodo]);

        sem_wait(&sem_arrayExternas[NumMinodo]);

        if(arrayExternas[1]>0){            

            if(maximoInternas>0){ 
                sem_wait(&sem_contadorNumeroPets[NumMinodo]);
                contadorNumeroPets++;
                sem_post(&sem_contadorNumeroPets[NumMinodo]);

                struct msgbuff peticion;
                peticion.mtype=99;
                peticion.texto.cabecera[0]=1;
                peticion.texto.cabecera[1]=id_minodo;
                peticion.texto.cabecera[2]=maximoInternas;
                peticion.texto.cabecera[3]=contadorNumeroPets;
                
                for(int a=0;a<NumNodos-1;a++){               
                        int x= msgsnd(id_nodos[a],&peticion,sizeof(peticion),0);                    
                }
            }

            send_token(arrayExternas[0]);
            sem_post(&sem_arrayExternas[NumMinodo]);

            sem_wait(&sem_testigo[NumMinodo]);
            sem_wait(&sem_dentro[NumMinodo]);
            sem_wait(&sem_ContHaciaN[NumMinodo]);

            testigo=0;
            dentro=0;
            ContHaciaN=0;

            sem_post(&sem_testigo[NumMinodo]);
            sem_post(&sem_dentro[NumMinodo]);
            sem_post(&sem_ContHaciaN[NumMinodo]);

        } else { 

            sem_post(&sem_arrayExternas[NumMinodo]);

            if(maximoInternas==1){
                sem_wait(&sem_modoConsultas[NumMinodo]);
                modoConsultas=1;
                sem_post(&sem_modoConsultas[NumMinodo]);
            }
        
            if(maximoInternas>0){
                struct msgbuff darPaso;
                darPaso.mtype=maximoInternas;
                
                sem_wait(&sem_dentro[NumMinodo]);
                dentro=0;
                sem_post(&sem_dentro[NumMinodo]);

                msgsnd(id_minodo,&darPaso,sizeof(darPaso),0);

                sem_wait(&sem_ContHaciaN[NumMinodo]);
                ContHaciaN++;
                sem_post(&sem_ContHaciaN[NumMinodo]);
            }
        }
    }
}



void * consultas(int * param){
    
    int  MiNum=(int) param[0];
    int Miprio=param[1];
    
    sem_wait(&sem_modoConsultas[NumMinodo]);
    
    if(modoConsultas==0){
        
        sem_post(&sem_modoConsultas[NumMinodo]);
        sem_wait(&sem_testigo[NumMinodo]);
        sem_wait(&sem_dentro[NumMinodo]);

        if(testigo==1 && dentro==0){
            
            sem_post(&sem_testigo[NumMinodo]);
            sem_post(&sem_dentro[NumMinodo]);
            
            struct msgbuff peticion;
            peticion.mtype=99;
            peticion.texto.cabecera[0]=27;

            msgsnd(id_minodo,&peticion,sizeof(peticion),0);

        } else {
            sem_post(&sem_testigo[NumMinodo]);sem_post(&sem_dentro[NumMinodo]);
        }

        sem_wait(&sem_testigo[NumMinodo]);

        if(testigo==0){
            
            sem_post(&sem_testigo[NumMinodo]);

            int prioMasAlta=maxPrioridadInterna();

            if(Miprio>prioMasAlta){
                
                sem_wait(&sem_contadorNumeroPets[NumMinodo]);
                contadorNumeroPets++;
                sem_post(&sem_contadorNumeroPets[NumMinodo]);

                struct msgbuff peticion;
                peticion.mtype=99;
                peticion.texto.cabecera[0]=1;
                peticion.texto.cabecera[1]=id_minodo;
                peticion.texto.cabecera[2]=Miprio;
                peticion.texto.cabecera[3]=contadorNumeroPets;
                
                for(int a=0;a<NumNodos-1;a++){               
                        int x= msgsnd(id_nodos[a],&peticion,sizeof(peticion),0);
                }
            }
       
        } else { 
            sem_post(&sem_testigo[NumMinodo]);
        }

        sem_wait(&sem_vector_peticiones_internas[NumMinodo]);
        vector_peticiones_internas[MiNum]=Miprio;
        
        sem_post(&sem_vector_peticiones_internas[NumMinodo]);

        struct msgbuff meDanPaso;
        msgrcv(id_minodo,&meDanPaso,sizeof(meDanPaso),Miprio,0);

    } else {
        sem_post(&sem_modoConsultas[NumMinodo]);
    }

    sem_wait(&sem_modoConsultas[NumMinodo]);

    // El nodo esta en modo consultas y manda los minitestigos para permitir procesos de consultas (no concurrentes) en otros nodos
    if(modoConsultas==1){

        sem_post(&sem_modoConsultas[NumMinodo]);

        sem_wait(&sem_dentro[NumMinodo]);
        
        dentro=1;
        sem_post(&sem_dentro[NumMinodo]);

        sem_wait(&sem_vector_peticiones_internas[NumMinodo]);
        
        vector_peticiones_internas[MiNum]=0;
        sem_post(&sem_vector_peticiones_internas[NumMinodo]);

        sem_wait(&sem_testigo[NumMinodo]);
        sem_wait(&sem_contadorConsultasDentro[NumMinodo]);
        
        if(testigo==1 && contadorConsultasDentro==0){
                       
            sem_post(&sem_testigo[NumMinodo]);
            sem_post(&sem_contadorConsultasDentro[NumMinodo]);
            
            sem_wait(&sem_vector_peticiones_externas[NumMinodo]);
            sem_wait(&sem_vector_atendidas[NumMinodo]);
            
            for(int i=0;i<NumNodos-1;i++){
                
                if(vector_peticiones_externas[id_nodos[i]][1]>vector_atendidas[id_nodos[i]][1]){
                     
                    sem_post(&sem_vector_atendidas[NumMinodo]);
                    struct msgbuff mini_token;
                    mini_token.mtype=99;
                    mini_token.texto.cabecera[0]=3;
                    mini_token.texto.cabecera[1]=id_minodo;
                
                    copiarVectores(i);
                    msgsnd(id_nodos[i],&mini_token,sizeof(mini_token),0);

                    sem_wait(&sem_contadorMinitestigos[NumMinodo]);
                    contadorMiniTestigos++;
                    sem_post(&sem_contadorMinitestigos[NumMinodo]);

                } else{ 
                    sem_post(&sem_vector_atendidas[NumMinodo]);
                }              
            }
            sem_post(&sem_vector_peticiones_externas[NumMinodo]);

        } else {
            sem_post(&sem_testigo[NumMinodo]);
            sem_post(&sem_contadorConsultasDentro[NumMinodo]);
        }

        int x=0;
        sem_wait(&sem_vector_peticiones_internas[NumMinodo]);
        
        for (int h=0;h<NumProcesos;h++){

            if(vector_peticiones_internas[h]==1 && x==0){
                
                struct msgbuff darPaso;
                darPaso.mtype=1;
                msgsnd(id_minodo,&darPaso,sizeof(darPaso),0);
                x=1;

            }
        }
        
        sem_post(&sem_vector_peticiones_internas[NumMinodo]);

        int numero=aumentarContadorGlobal();

        printf("NUMERO %d en entrar. Proceso %i, prioridad %i. En la seccion critica \n",numero,MiNum,Miprio); 

        sleep(1);
        
        sem_wait(&sem_contadorConsultasDentro[NumMinodo]);
        contadorConsultasDentro++;
        sem_post(&sem_contadorConsultasDentro[NumMinodo]);

        
        sem_wait(&sem_contadorConsultasDentro[NumMinodo]);
        contadorConsultasDentro--;
        sem_post(&sem_contadorConsultasDentro[NumMinodo]);


        sem_wait(&sem_contadorNumeroPets[NumMinodo]);
        sem_wait(&sem_vector_atendidas[NumMinodo]);
        vector_atendidas[id_minodo][Miprio]=contadorNumeroPets;
        sem_post(&sem_vector_atendidas[NumMinodo]);
        sem_post(&sem_contadorNumeroPets[NumMinodo]);


        sem_wait(&sem_contadorConsultasDentro[NumMinodo]);

        // Ultima consulta en salir, indica al nodo que debe recopilar minitestigos
        if(contadorConsultasDentro==0){

            sem_post(&sem_contadorConsultasDentro[NumMinodo]);

            sem_wait(&sem_dentro[NumMinodo]);
            dentro=0;
            sem_post(&sem_dentro[NumMinodo]);

            struct msgbuff minitestigo_nodo;
            minitestigo_nodo.mtype=99;
            minitestigo_nodo.texto.cabecera[0]=4;
            msgsnd(id_minodo,&minitestigo_nodo,sizeof(minitestigo_nodo),0);
            
            sem_wait(&sem_testigo[NumMinodo]);
            sem_wait(&sem_MiniToken[NumMinodo]);

            if(testigo==0 && MiniToken==1){ 
                sem_post(&sem_testigo[NumMinodo]);
                sem_post(&sem_MiniToken[NumMinodo]);

                struct msgbuff minitoken;
                minitoken.mtype=99;
                minitoken.texto.cabecera[0]=5;
                msgsnd(id_emisor_minitoken,&minitoken,sizeof(minitoken),0);
                
                sem_wait(&sem_MiniToken[NumMinodo]);
                MiniToken=0;ç

                sem_post(&sem_MiniToken[NumMinodo]);

                sem_wait(&sem_dentro[NumMinodo]);
                dentro=0;
                
                sem_post(&sem_dentro[NumMinodo]);

            } else {
                sem_post(&sem_testigo[NumMinodo]);
                sem_post(&sem_MiniToken[NumMinodo]);
            }
           
            sem_wait(&sem_modoConsultas[NumMinodo]);
            modoConsultas=0;
            sem_post(&sem_modoConsultas[NumMinodo]);

        } else {
            sem_post(&sem_contadorConsultasDentro[NumMinodo]);
        }
    } else { 
        sem_post(&sem_modoConsultas[NumMinodo]);
    }
}