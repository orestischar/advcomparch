BOB="L1 L2" # TLB Prefetching"
COMB="11 12" # 12 7"
arr1=($BOB)
arr2=($COMB)
for ((i=0;i<4;i++)); do
	./means_plot.sh ${arr2[i]} outputs/${arr1[i]}/*
	mv bob.png ./img/${arr1[i]}.png

done
