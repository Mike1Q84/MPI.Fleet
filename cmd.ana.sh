#!/bin/bash
# for ((i = 1; i <= 3; i++))
# do
	for ((j = 5; j <= 250; j=j+5))
	do
		for ((i = 1; i <= 3; i++))
		do
			tail -3 mike."$j"."$i".txt | head -1 >> round."$i".txt
		done
	done
# done



# tail -3 mike.1.1.txt | head -3