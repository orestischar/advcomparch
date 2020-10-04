BENCHMARKS="blackscholes bodytrack canneal facesim ferret fluidanimate freqmine rtview streamcluster swaptions"

for bench in $BENCHMARKS; do
	./plot_7_2.sh outputs/$bench.*

	mv 7_2.png ./img/7_2/$bench.png

done
