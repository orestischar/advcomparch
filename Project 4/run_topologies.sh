#!/bin/bash
SNIPER_EXE=/home/orestis/Downloads/sniper-6.1/run-sniper
CONFIG=/home/orestis/Desktop/advcomparch-2018-19-ex4-helpcode/${CONFIG}
OUTPUT_DIR_BASE=/home/orestis/Desktop/advcomparch-2018-19-ex3-helpcode/real      
VERSIONS="-DTAS_CAS -DTAS_TS -DTTAS_CAS -DTTAS_TS -DMUTEX"
type="-DREAL"
  


for mode in $VERSIONS; do

  make clean
  make LFLAG=$mode

      ${SNIPER_EXE} -c ${CONFIG} -n 4  --roi -g --perf_model/l1_icache/shared_cores=1 -g --perf_model/l1_dcache/shared_cores=1 -g --perf_model/l2_cache/shared_cores=4 -g --perf_model/l3_cache/shared_cores=4 -- ./locks 4 1000 1
      ${SNIPER_EXE} -c ${CONFIG} -n 4 --roi -g --perf_model/l1_icache/shared_cores=1 -g --perf_model/l1_dcache/shared_cores=1 -g --perf_model/l2_cache/shared_cores=1 -g --perf_model/l3_cache/shared_cores=4 -- ./locks 4 1000 1
      ${SNIPER_EXE} -c ${CONFIG} -n 4 --roi -g --perf_model/l1_icache/shared_cores=1 -g --perf_model/l1_dcache/shared_cores=1 -g --perf_model/l2_cache/shared_cores=1 -g --perf_model/l3_cache/shared_cores=1 -- ./locks 4 1000 1
done
