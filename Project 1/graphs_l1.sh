BENCHMARKS="blackscholes bodytrack canneal facesim ferret fluidanimate freqmine rtview streamcluster swaptions"

for bench in $BENCHMARKS; do
	./plot_l1.sh outputs/L1/$bench.*

	mv L1.png ./img/L1/$bench.png

done
