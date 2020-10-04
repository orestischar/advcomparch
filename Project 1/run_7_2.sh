#!/bin/bash

## Modify the following paths appropriately
PARSEC_PATH=/home/orestis/Downloads/parsec-3.0
PIN_EXE=/home/orestis/Downloads/pin-3.6-97554-g31f0a167d-gcc-linux/pin
PIN_TOOL=/home/orestis/Downloads/parsec-3.0/parsec_workspace/pintool/obj-intel64/simulator.so

CMDS_FILE=./cmds_simlarge.txt
outDir="./outputs/"

export LD_LIBRARY_PATH=$PARSEC_PATH/pkgs/libs/hooks/inst/amd64-linux.gcc-serial/lib/

## Triples of <cache_size>_<associativity>_<block_size>
#CONFS="16_4_32_2 16_4_64_2 16_4_128 32_4_32 32_4_64 32_4_128 32_8_64 64_4_32 64_4_64 64_4_128 64_8_64 128_8_64"


L1size=32
L1assoc=8
L1bsize=64
L2size=1024
L2assoc=8
L2bsize=128
TLBe=64
TLBp=4096
TLBa=4
L2prf=0


BENCHMARKS="blackscholes bodytrack canneal facesim ferret fluidanimate freqmine rtview streamcluster swaptions"

for BENCH in $BENCHMARKS; do
	cmd=$(cat ${CMDS_FILE} | grep "$BENCH")

	outFile=$(printf "%s.dcache_cslab.out" $BENCH)
	outFile="$outDir/$outFile"


		pin_cmd="$PIN_EXE -t $PIN_TOOL -o $outFile -L1c ${L1size} -L1a ${L1assoc} -L1b ${L1bsize} -L2c ${L2size} -L2a ${L2assoc} -L2b ${L2bsize} -TLBe ${TLBe} -TLBp ${TLBp} -TLBa ${TLBa} -L2prf ${L2prf} -- $cmd"
		time $pin_cmd
	done

done
