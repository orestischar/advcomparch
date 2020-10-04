#!/usr/bin/env python3

import sys
import numpy as np
import random
from math import log2

## We need matplotlib:
## $ apt-get install python-matplotlib
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

combinations = int(sys.argv[1]) #different combinations

ipc_Axis=np.ones(combinations)
mpki_Axis=np.ones(combinations)


x_Axis = []
ipc_temp = []
mpki_temp = []
initial_l1_size = 32.0
initial_l1_assoc = 4.0
counter=0
flag=1
for outFile in sys.argv[2:]:
	counter+=1
	fp = open(outFile)
	line = fp.readline()
	while line:
		tokens = line.split()
		if (line.startswith("Total Instructions: ")):
			total_instructions = int(tokens[2])
		elif (line.startswith("IPC:")):
			ipc = float(tokens[1])
		elif (line.startswith("  L1-Data Cache")):
			sizeLine = fp.readline()
			l1_size = float(sizeLine.split()[1])
			bsizeLine = fp.readline()
			l1_bsize = float(bsizeLine.split()[2])
			assocLine = fp.readline()
			l1_assoc = float(assocLine.split()[1])
		elif (line.startswith("L1-Total-Misses")):
			l1_total_misses = int(tokens[1])
			l1_miss_rate = float(tokens[2].split('%')[0])
			mpki = l1_total_misses / (total_instructions / 1000.0)


		line = fp.readline()

	fp.close()


	size_doublings = int(log2(l1_size / initial_l1_size))
	for i in range(size_doublings):
		ipc *= 0.90

	assoc_doublings = int(log2(l1_assoc / initial_l1_assoc))
	for i in range(assoc_doublings):
		ipc *= 0.95

	if (flag==1):
		l1ConfigStr = '{}K.{}.{}B'.format(int(l1_size),int(l1_assoc),int(l1_bsize))
		x_Axis.append(l1ConfigStr)

	ipc_temp.append(ipc)
	mpki_temp.append(mpki)



	if (counter==combinations):
		ipc_Axis = np.multiply(ipc_Axis, ipc_temp)
		mpki_Axis= np.multiply(mpki_Axis, mpki_temp)
		counter=0
		ipc_temp = []
		mpki_temp = []
		flag=0

for i in range(len(ipc_Axis)):
	ipc_Axis[i] = ipc_Axis[i]**(1.0/10)
	mpki_Axis[i] = mpki_Axis[i]**(1.0/10)

print (x_Axis)
print (ipc_Axis)
print (mpki_Axis)

fig, ax1 = plt.subplots()
ax1.grid(True)
ax1.set_xlabel("CacheSize.Assoc.BlockSize")

xAx = np.arange(len(x_Axis))
ax1.xaxis.set_ticks(np.arange(0, len(x_Axis), 1))
ax1.set_xticklabels(x_Axis, rotation=45)
ax1.set_xlim(-0.5, len(x_Axis) - 0.5)
ax1.set_ylim(min(ipc_Axis) - 0.05 * min(ipc_Axis), max(ipc_Axis) + 0.05 * max(ipc_Axis))
ax1.set_ylabel("$IPC$")
line1 = ax1.plot(ipc_Axis, label="ipc", color="red",marker='x')

ax2 = ax1.twinx()
ax2.xaxis.set_ticks(np.arange(0, len(x_Axis), 1))
ax2.set_xticklabels(x_Axis, rotation=45)
ax2.set_xlim(-0.5, len(x_Axis) - 0.5)
ax2.set_ylim(min(mpki_Axis) - 0.05 * min(mpki_Axis), max(mpki_Axis) + 0.05 * max(mpki_Axis))
ax2.set_ylabel("$MPKI$")
line2 = ax2.plot(mpki_Axis, label="L1D_mpki", color="green",marker='o')

lns = line1 + line2
labs = [l.get_label() for l in lns]

plt.title("Mean Values: IPC vs MPKI")
lgd = plt.legend(lns, labs)
lgd.draw_frame(False)
plt.savefig("L1_b.png",bbox_inches="tight")
