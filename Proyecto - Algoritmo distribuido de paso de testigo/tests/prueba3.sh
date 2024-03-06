#!/bin/bash 
# prueba3.sh 
echo 'prueba3 lanzada'

for((i=0;i<5;i++))
do
    for((j=0;j<6;j++))
    do  
        ./entrador 6 $j 1
    done
done

sleep 2

./entrador 6 0 5

sleep 1

./entrador 6 1 3 
./entrador 6 2 2


for((i=0;i<10;i++))
do
    for((j=0;j<6;j++))
    do  
        ./entrador 6 $j 1
    done
done

for((i=0;i<4;i++))
do
    for((j=0;j<6;j++))
    do  
        ./entrador 6 $j 1
    done
done

sleep 2

./entrador 6 4 5
sleep 1
./entrador 6 5 4
./entrador 6 3 4
