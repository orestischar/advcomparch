#!/bin/bash
G="GRAIN_001 GRAIN_010 GRAIN_100"
MODES="DTAS_CAS DTAS_TS DTTAS_CAS DTTAS_TS MUTEX"
THREADS="01 02 04 08 16"
g md th
for g in $G; do
for md in $MODES; do
for th in $THREADS; do

    /home/orestis/Downloads/sniper-6.1/tools/advcomparch_mcpat.py -d /part1/$g/$md.NTHREADS_$th-NTHREADS_$th-$g.out -t total -o /part1/$g/$md.NTHREADS_$th-NTHREADS_$th-$g.out/info > /part1/$g/$md.NTHREADS_$th-NTHREADS_$th-$g.out/power.total.out

done
done
done

#---------------------------------------

LOCKS="DTAS_CAS DTAS_TS DTTAS_CAS DTTAS_TS MUTEX"
SHARING="SHARE_ALL SHARE_L3 SHARE_NOTHING"

for locks in $LOCKS; do
for sh in $SHARING; do

    /home/orestis/Downloads/sniper-6.1/tools/advcomparch_mcpat.py -d /home/orestis/Desktop/advcomparch-2018-19-ex4-helpcode/outputs/$locks/$sh-NTHREADS_04-GRAIN_SIZE_001.out -t total -o /home/orestis/Downloads/advcomparch-2018-19-ex4-helpcode/outputs/$locks/$sh-NTHREADS_04-GRAIN_SIZE_001.out/info > /home/orestis/Downloads/advcomparch-2018-19-ex4-helpcode/outputs/$locks/$sh-NTHREADS_04-GRAIN_SIZE_001.out/power.total.out

done
done
