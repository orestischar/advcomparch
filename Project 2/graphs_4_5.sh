BENCHMARKS="403.gcc 429.mcf 434.zeusmp 436.cactusADM 445.gobmk 450.soplex 456.hmmer 458.sjeng 459.GemsFDTD 471.omnetpp 473.astar 483.xalancbmk"

for bench in $BENCHMARKS; do
    ./plot_mpki_ipc_4_5.py outputs/4_5/$bench.*

    mv plot.png ./img/$bench.png

done
