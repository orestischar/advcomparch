BENCHMARKS="blackscholes bodytrack canneal facesim ferret fluidanimate freqmine rtview streamcluster swaptions"

for bench in $BENCHMARKS; do
	./plot_l2.sh outputs/L2/$bench.*

	mv L2.png ./img/L2/$bench.png

done
