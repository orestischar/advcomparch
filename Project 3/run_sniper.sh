#!/bin/bash

## Example of usage: ./run_sniper.sh gcc
## Modify the following paths appropriately
## CAUTION: use only absolute paths below!!!
SNIPER_EXE=/home/orestis/Downloads/sniper-6.1/run-sniper
SPEC_PINBALLS_DIR=/home/orestis/Downloads/cpu2006_pinballs/
SNIPER_CONFIG=gainestown
OUTPUT_DIR_BASE="./outputs"

for BENCH in $@; do
	benchOutDir=${OUTPUT_DIR_BASE}/$BENCH
	mkdir -p $benchOutDir

for dw in 1 2 4 8 16 32; do
for ws in 96 192 384; do
	outDir=$(printf "%s.DW_%02d-WS_%03d.out" $BENCH $dw $ws)
	outDir="${benchOutDir}/${outDir}"

	pinball="$(ls $SPEC_PINBALLS_DIR/$BENCH/pinball_short.pp/*.address)"
	sniper_cmd="${SNIPER_EXE} -c ${SNIPER_CONFIG} -d ${outDir} -g --perf_model/core/interval_timer/dispatch_width=$dw -g --perf_model/core/interval_timer/window_size=$ws --pinballs=${pinball}"
	echo \"$sniper_cmd\"
	/bin/bash -c "$sniper_cmd"
done
done

done