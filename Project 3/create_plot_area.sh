BENCHMARKS="astar"

for bench in $BENCHMARKS; do
	./plot_area.py outputs/$bench/$bench.*
done
