#BENCHMARKS="astar cactusADM gcc GemsFDTD gobmk hmmer mcf omnetpp sjeng soplex xalancbmk zeusmp"
BENCHMARKS="astar gcc omnetpp"
for bench in $BENCHMARKS; do
	./plot_energy_delay.py outputs/$bench/$bench.*
done
