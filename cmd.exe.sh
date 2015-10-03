#!/bin/bash
for ((i = 1; i <= 3; i++))
do
	for ((j = 5; j <= 250; j=j+5))
	do
		mpirun -np "$j" mike > mike."$j"."$i".txt;
		sleep 9;
	done
done




# $ for i in $(seq 5); do mpirun -np "$i" mike > mike."$i".txt; sleep 3; done


# for i in $(seq 3); do for j in $(seq 5); do mpirun -np "$j" mike > mike."$i"."$j".txt; sleep 3; done
# for j in $(seq 5); do mpirun -np "$j" mike > mike."$j".txt; sleep 3; done


