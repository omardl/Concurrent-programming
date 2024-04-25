# Prácticas y proyecto realizados durante la asignatura de Programacion Concurrente y Distribuida (3º) del GETT de la UVigo

En esta asignatura se abordaron los principales conceptos y problemas de la programación concurrente, en conjunto con una serie de prácticas y un proyecto en los que se aplicaron diferentes herramientas para su gestión.

## Prácticas
La temática de cada práctica está enfocada al uso de una o varias herramientas específicas para la resolución de problemas de concurrencia. Empezando por ejercicios sencillos para conocer cómo funciona cada herramienta y acabando, algunos de ellos, con la resolución de problemas clásicos de la programación concurrente como el problema de la cena de los filósofos, los lectores-escritores, el agente y los fumadores, etc.

### Práctica 1
Sincronización de procesos concurrentes mediante señales.

### Práctica 2
Sincronización entre procesos padre e hijo y resolución de grafos de procedencia.

### Práctica 3
Uso de memoria compartida. Algoritmos de Decker y Peterson.

### Prácticas 4 y 5
Sincronización de procesos mediante semáforos. Resolución del problema de lectores y escritores.

### Práctica 6
Sincronización de procesos mediante colas de mensajes. Resolución del problema de la cena de los filósofos y el problema del agente y los fumadores.



## Proyecto: Elaboración de un algoritmo de paso de testigo en sistemas distribuídos
El contexto de este proyecto busca desarrollar un algoritmo de paso de testigo en un sistema distribuído que simula una "taquilla virtual", en la que se realizarían la compraventa de entradas para diferentes eventos. 
Para ello, inicialmente se parte de la base de que en cada nodo se podrán realizar una serie de procesos, que podrán tener mayor o menor prioridad en el sistema. Dichos procesos, en función de su prioridad, son:
- Mantenimiento, por parte de los administradores de la plataforma (prioridad 5)
- Pagos, por parte de los clientes (prioridad 4)
- Administración, por parte de las empresas que realizan los eventos (prioridad 4)
- Anulación, por parte de los clientes (prioridad 3)
- Reservas, por parte de los clientes (prioridad 2)
- Consultas, por parte de los clientes (prioridad 1)


A mayores de seguir el orden de prioridad, todos los procesos serán concurrentes y por tanto, no podrán realizarse a la vez, tanto con procesos de otras prioridades como con otros procesos de igual prioridad. Para ello, el sistema distribuido tendrá un testigo que se irá pasando y solicitando entre los diferentes nodos, y sólo cuando un nodo disponga de éste podrá realizar sus procesos en orden de prioridad y uno por uno. En el testigo, también se envían que procesos de qué prioridad están en espera en otros nodos, para que nodo con el testigo sepa a quién lo debe enviar una vez sus procesos prioritarios acaben. Además, si surge un proceso más prioritario en otro nodo, informará a todos los nodos (no sabiendo cuál tiene el testigo) para que actualice dicha información en el testigo.


A excepción del resto, los procesos de consultas no serán concurrentes y por tanto todos los nodos podrán realizarlos a la vez. Para ello, el nodo con el testigo enviará una serie de minitestigos que indicarán que sí pueden realizar procesos de consultas (cuando sólo estén pendientes procesos de consultas en el mismo y no haya otros de mayor prioridad en los demás nodos), hasta que un proceso de mayor prioridad aparezca en el sistema y el nodo con el testigo solicite la recuperación de minitestigos y continúe el proceso.


Además, para evitar problemas de inanición, cada nodo estará limitado a realizar un número máximo de procesos de igual prioridad si hay otros nodos que también están a la espera para ello, evitando así una situación de bloqueo.

# Diagramas de flujo
## Diagrama de procesos de prioridad 2 o mayor
![diagrama1](https://github.com/omardl/Concurrent-programming--PCCD-GET-UVigo/assets/105445540/c658f433-b749-47b8-b1d0-6affac5b0399)

## Diagrama de procesos de prioridad 1
![diagrama2](https://github.com/omardl/Concurrent-programming--PCCD-GET-UVigo/assets/105445540/fc7c90ee-6b9e-4835-9c60-c9d8398fdccf)

## Diagrama del proceso de recepción en el nodo
![diagrama3](https://github.com/omardl/Concurrent-programming--PCCD-GET-UVigo/assets/105445540/235f160a-0675-484f-bd7e-8debb461016c)

### Autores
- Omar Delgado
- María Boado
- Julen Beiro
- Adrián González

