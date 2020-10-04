BENCHMARKS="astar cactusADM gcc GemsFDTD gobmk hmmer mcf omnetpp sjeng soplex xalancbmk zeusmp"

for bench in $BENCHMARKS; do
	./plot_energy.py outputs/$bench/$bench.*
done
