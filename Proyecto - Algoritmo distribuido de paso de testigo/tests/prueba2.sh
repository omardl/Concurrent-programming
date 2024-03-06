#!/bin/bash 
# prueba2.sh 
echo 'prueba2 lanzada'
for((j=0;j<4;j++))
do
    for ((i=5;i>0;i--))
    do
        for((k=0;k<6;k++))
        do
        ./entrador 6 $k $i
        done

    done
   

done