BENCHMARKS="blackscholes bodytrack canneal facesim ferret fluidanimate freqmine rtview streamcluster swaptions"

for bench in $BENCHMARKS; do
	./plot_prefetching.sh outputs/Prefetching/$bench.*

	mv Prefetching.png ./img/Prefetching/$bench.png

done
