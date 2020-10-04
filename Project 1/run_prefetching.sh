#!/bin/bash

## Modify the following paths appropriately
PARSEC_PATH=/home/orestis/Downloads/parsec-3.0
PIN_EXE=/home/orestis/Downloads/pin-3.6-97554-g31f0a167d-gcc-linux/pin
PIN_TOOL=/home/orestis/Downloads/parsec-3.0/parsec_workspace/pintool/obj-intel64/simulator.so

CMDS_FILE=./cmds_simlarge.txt
outDir="./outputs/"

export LD_LIBRARY_PATH=$PARSEC_PATH/pkgs/libs/hooks/inst/amd64-linux.gcc-serial/lib/

## Triples of <cache_size>_<associativity>_<block_size>
BLOCKS="1 2 4 8 16 32 64"

L1size=32
L1assoc=8
L1bsize=64
L2size=1024
L2assoc=8
L2bsize=128
TLBe=64
TLBp=4096
TLBa=4

BENCHMARKS="blackscholes bodytrack canneal facesim ferret fluidanimate freqmine rtview streamcluster swaptions"

for BENCH in $BENCHMARKS; do
	cmd=$(cat ${CMDS_FILE} | grep "$BENCH")
	
	for n in $BLOCKS; do
		## Get parameters
	    L2prf=$(echo $n)

		outFile=$(printf "%s.dcache_cslab.Prefetching_n=%02d.out" $BENCH ${L2prf})
		outFile="$outDir/$outFile"

		pin_cmd="$PIN_EXE -t $PIN_TOOL -o $outFile -L1c ${L1size} -L1a ${L1assoc} -L1b ${L1bsize} -L2c ${L2size} -L2a ${L2assoc} -L2b ${L2bsize} -TLBe ${TLBe} -TLBp ${TLBp} -TLBa ${TLBa} -L2prf ${L2prf} -- $cmd"
		time $pin_cmd
	done

done
