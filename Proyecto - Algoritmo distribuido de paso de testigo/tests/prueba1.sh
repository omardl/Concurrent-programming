#!/bin/bash 
# prueba1.sh 
echo 'prueba1 lanzada'
for ((i=0;i<6;i++))
do
    for((j=0;j<10;j++))
    do
    ./entrador 6 $i 1
    done
done
./entrador 6 1 5
for ((i=0;i<6;i++))
do
    for((j=0;j<10;j++))
    do
    ./entrador 6 $i 1
    done
done