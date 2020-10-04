#!/bin/bash
SNIPER_EXE=/home/orestis/Downloads/sniper-6.1/run-sniper
CONFIG=/home/orestis/Desktop/advcomparch-2018-19-ex4-helpcode/ask4.cfg
OUTPUT_DIR_BASE=/home/orestis/Desktop/advcomparch-2018-19-ex3-helpcode/part1      
VERSIONS="-DTAS_CAS -DTAS_TS -DTTAS_CAS -DTTAS_TS -DMUTEX"
type="-DSNIPER"


for grain_size in 1 10 100; do
	GSoutDir=$(printf "GS_%03d" $grain_size)
	benchOutDir=${OUTPUT_DIR_BASE}/${GSoutDir}
	
	for mode in $VERSIONS; do

		make clean
		make LFLAG=$mode IMPLFLAG=$type

		threads=1
		outDir=$(printf "%s.Cores_%02d-GS_%03d.out" $LFLAG $cores $grain_size)
		outDir="${benchOutDir}/${outDir}"

		sniper_cmd="${SNIPER_EXE} -c ${CONFIG} -n ${cores} --roi -g --perf_model/l2_cache/shared_cores=1 -g --perf_model/l3_cache/shared_cores=1 -d ${outDir} -- ./locks ${cores} 1000 ${grain_size}"
		echo \"$sniper_cmd\"
		/bin/bash -c "$sniper_cmd"

		threads=2
		outDir=$(printf "%s.Cores_%02d-GS_%03d.out" $LFLAG $cores $grain_size)
		outDir="${benchOutDir}/${outDir}"

		sniper_cmd="${SNIPER_EXE} -c ${CONFIG} -n ${cores} --roi -g --perf_model/l2_cache/shared_cores=2 -g --perf_model/l3_cache/shared_cores=2 -d ${outDir} -- ./locks ${cores} 1000 ${grain_size}"
		echo \"$sniper_cmd\"
		/bin/bash -c "$sniper_cmd"

		threads=4
		outDir=$(printf "%s.Cores_%02d-GS_%03d.out" $LFLAG $cores $grain_size)
		outDir="${benchOutDir}/${outDir}"

		sniper_cmd="${SNIPER_EXE} -c ${CONFIG} -n ${cores} --roi -g --perf_model/l2_cache/shared_cores=4 -g --perf_model/l3_cache/shared_cores=4 -d ${outDir} -- ./locks ${cores} 1000 ${grain_size}"
		echo \"$sniper_cmd\"
		/bin/bash -c "$sniper_cmd"

		threads=8
		outDir=$(printf "%s.Cores_%02d-GS_%03d.out" $LFLAG $cores $grain_size)
		outDir="${benchOutDir}/${outDir}"

		sniper_cmd="${SNIPER_EXE} -c ${CONFIG} -n ${cores} --roi -g --perf_model/l2_cache/shared_cores=4 -g --perf_model/l3_cache/shared_cores=8 -d ${outDir} -- ./locks ${cores} 1000 ${grain_size}"
		echo \"$sniper_cmd\"
		/bin/bash -c "$sniper_cmd"

		threads=16
		outDir=$(printf "%s.Cores_%02d-GS_%03d.out" $LFLAG $cores $grain_size)
		outDir="${benchOutDir}/${outDir}"

		sniper_cmd="${SNIPER_EXE} -c ${CONFIG} -n ${cores} --roi -g --perf_model/l2_cache/shared_cores=1 -g --perf_model/l3_cache/shared_cores=8 -d ${outDir} -- ./locks ${cores} 1000 ${grain_size}"
		echo \"$sniper_cmd\"
		/bin/bash -c "$sniper_cmd"

done
done
