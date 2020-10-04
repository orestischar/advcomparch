BENCHMARKS="gcc mcf zeusmp cactusADM gobmk soplex hmmer sjeng GemsFDTD omnetpp astar xalancbmk"

for bench in $BENCHMARKS; do
    ./plot_ipc.py outputs/$bench/$bench.*


done
