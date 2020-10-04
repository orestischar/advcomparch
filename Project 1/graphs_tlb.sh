BENCHMARKS="blackscholes bodytrack canneal facesim ferret fluidanimate freqmine rtview streamcluster swaptions"

for bench in $BENCHMARKS; do
	./plot_tlb.sh outputs/TLB/$bench.*

	mv TLB.png ./img/TLB/$bench.png

done
