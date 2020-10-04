BENCHMARKS="astar cactusADM gcc GemsFDTD gobmk hmmer mcf omnetpp sjeng soplex xalancbmk zeusmp"

DWS="01 02 04 08 16 32"
WINDOW_S="096 192 384"

for bench in $BENCHMARKS; do
	for dw in $DISPATCH_W; do
		for ws in $WINDOW_S; do

		/home/orestis/Downloads/sniper-6.1/tools/advcomparch_mcpat.py -d /outputs/$bench/$bench.DW_$dw-WS_$ws.out -t total -o /outputs/$bench/$bench.DW_$dw-WS_$ws.out/info > /outputs/$bench/$bench.DW_$dw-WS_$ws.out/power.total.out

		done
	done
done

