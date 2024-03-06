#!/bin/bash 
# prueba0.sh 
echo 'prueba0 lanzada'

./entrador 6 1 4


for((i=0;i<10;i++))
do
 ./entrador 6 1 1
done


for((i=0;i<3;i++))
do
    for((j=2;j<=5;j++))
    do  
        ./entrador 6 1 $j
    done
done
