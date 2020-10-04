#!/usr/bin/env python3

import sys
import numpy as np
import random

## We need matplotlib:
## $ apt-get install python-matplotlib
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

combinations = int(sys.argv[1]) #different combinations

ipc_Axis=np.ones(combinations)
mpki_Axis=np.ones(combinations)
#mpki2_Axis=np.ones(combinations)


x_Axis = []
ipc_temp = []
mpki_temp = []
#mpki2_temp = []
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
			l1_size = sizeLine.split()[1]
			bsizeLine = fp.readline()
			l1_bsize = bsizeLine.split()[2]
			assocLine = fp.readline()
			l1_assoc = assocLine.split()[1]
		elif (line.startswith("L1-Total-Misses")):
			l1_total_misses = int(tokens[1])
			l1_miss_rate = float(tokens[2].split('%')[0])
			mpki = l1_total_misses / (total_instructions / 1000.0)

		line = fp.readline()

	fp.close()
	if (flag==1):
		l1ConfigStr = '{}K.{}.{}B'.format(l1_size,l1_assoc,l1_bsize)
		x_Axis.append(l1ConfigStr)


	ipc_Axis.append(ipc)
	mpki_Axis.append(mpki)


	#mpki2_temp.append(mpki_l2)

	if (counter==combinations):
		ipc_Axis = np.multiply(ipc_Axis, ipc_temp)
		mpki_Axis= np.multiply(mpki_Axis, mpki_temp)
		#mpki2_Axis= np.multiply(mpki2_Axis, mpki2_temp)
		counter=0
		ipc_temp = []
		mpki_temp = []
		flag=0

for i in range(len(ipc_Axis)):
	ipc_Axis[i] = ipc_Axis[i]**(1.0/10)
	mpki_Axis[i] = mpki_Axis[i]**(1.0/10)
	#mpki2_Axis = [i] = mpki2_Axis[i]**(1.0/combinations)

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
plt.savefig("L1.png",bbox_inches="tight")
