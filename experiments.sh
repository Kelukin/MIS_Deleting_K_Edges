#!/bin/bash
Edges=(50 100 300 500 1000)
Query_id=(0 1 2 3 4  5 6 7 8 9)
dirs=(Data/C125 Data/C250 Data/C500)
Program=./MIS_Deleting_K_Edges
for edge_num  in ${Edges[@]}
do
	echo $edge_num
done

for file in ${dirs[@]}
do
	python3 Data/generate_query_set.py ${file} 10 
	echo 'NearLinear_100edges_train'
	echo ''
	$Program n $file 100 1 1 300
	for q_id in ${Query_id[@]}
	do

		cp "${file}_given_set${q_id}.bin" "${file}_given_set.bin"
		for edge_num in ${Edges[@]}
		do
			echo "$file $edge_num $q_id"
			echo ''
			echo 'Greedy'
			echo ''
			$Program greedy $file $edge_num  

			#echo 'NearLinear_Base_train'
			#echo ''
			#$Program n $file $edge_num 1 1 200 

			echo ''
			echo 'NearLinear_real'
			echo ''
			$Program n $file $edge_num 0 
			
			#echo ''	
			#echo 'P&T'
			#echo ''
			#$Program p $file $edge_num 0 1

			echo ''	
			echo 'P&T_edge_estimation'
			echo ''
			$Program p $file $edge_num 0 2

			
			#echo 'NearLinear_500edges_train'
			#echo ''
			#$Program n $file 500 1 1 200

			#echo ''	
			#echo 'P&T_diff_learn_estimation'
			#echo ''
			#$Program p $file $edge_num 0 2


			#echo ''
			#echo 'Long Time Train'
			#echo ''
			#$Program n $file $edge_num 1 1 500


			#echo ''	
			#echo 'P&T_long_log_learning'
			#echo ''
			#$Program p $file $edge_num 0 2

			echo ''
			echo ''
			echo ''
		done
	done
done
