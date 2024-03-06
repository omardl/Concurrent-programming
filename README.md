# Prácticas y proyecto realizados durante la asignatura de Programacion Concurrente y Distribuida (3º) del GETT de la UVigo

En esta asignatura se abordaron los principales conceptos y problemas de la programación concurrente, en conjunto con una serie de prácticas y un proyecto en los que se aplicaron diferentes herramientas para su gestión.

## Prácticas

## Proyecto: Elaboración de un algoritmo de paso de testigo en sistemas distribuídos
El contexto de este proyecto busca desarrollar un algoritmo de paso de testigo en un sistema distribuído que simula una "taquilla virtual", en la que se realizarían la compraventa de entradas para diferentes eventos. 
Para ello, inicialmente se parte de la base de que en cada nodo se podrán realizar una serie de procesos, que podrán tener mayor o menor prioridad en el sistema. Dichos procesos, en función de su prioridad, son:
- Mantenimiento, por parte de los administradores de la plataforma (prioridad 5)
- Pagos, por parte de los clientes (prioridad 4)
- Administración, por parte de las empresas que realizan los eventos (prioridad 4)
- Anulación, por parte de los clientes (prioridad 3)
- Reservas, por parte de los clientes (prioridad 2)
- Consultas, por parte de los clientes (prioridad 1)
A mayores de seguir el orden de prioridad, todos los procesos serán concurrentes y por tanto, no podrán realizarse a la vez, tanto con procesos de otras prioridades como con otros procesos de igual prioridad. Para ello, el sistema distribuido tendrá un testigo que se irá pasando y solicitando entre los diferentes nodos, y sólo cuando un nodo disponga de éste podrá realizar sus procesos en orden de prioridad y uno por uno.
A excepción del apartado anterior, los procesos de consultas no serán concurrentes y por tanto todos los nodos podrán realizarlos a la vez. Para ello, el nodo con el testigo enviará una serie de minitestigos que indicarán que sí pueden realizar procesos de consultas, hasta que un proceso de mayor prioridad aparezca en el sistema y el nodo con el testigo solicite la recuperación de minitestigos y continúe el proceso.
