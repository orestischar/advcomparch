#!/usr/bin/env python3

import sys
import numpy as np

## We need matplotlib:
## $ apt-get install python-matplotlib
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

x_Axis = []
ipc_Axis = []
mpki_l1_Axis = []
mpki_l2_Axis = []

for outFile in sys.argv[1:]:
	fp = open(outFile)
	line = fp.readline()
	while line:
		tokens = line.split()
		if (line.startswith("Total Instructions: ")):
			total_instructions = int(tokens[2])
		elif (line.startswith("IPC:")):
			ipc = float(tokens[1])
		elif (line.startswith("  L1-Data Cache")):
			sizeLine1 = fp.readline()
			l1_size = sizeLine1.split()[1]
			bsizeLine1 = fp.readline()
			l1_bsize = bsizeLine1.split()[2]
			assocLine1 = fp.readline()
			l1_assoc = assocLine1.split()[1]
		elif (line.startswith("L1-Total-Misses")):
			l1_total_misses = int(tokens[1])
			l1_miss_rate = float(tokens[2].split('%')[0])
			mpki_l1 = l1_total_misses / (total_instructions / 1000.0)
		elif (line.startswith("  L2-Data Cache")):
			sizeLine2 = fp.readline()
			l2_size = sizeLine2.split()[1]
			bsizeLine2 = fp.readline()
			l2_bsize = bsizeLine2.split()[2]
			assocLine2 = fp.readline()
			l2_assoc = assocLine2.split()[1]
		elif (line.startswith("L2-Total-Misses")):
			l2_total_misses = int(tokens[1])
			l2_miss_rate = float(tokens[2].split('%')[0])
			mpki_l2 = l2_total_misses / (total_instructions / 1000.0)
		elif (line.startswith("L2_prefetching")):
			pref = fp.readline()
			l2_prefetch_lines = int(tokens[3].split(')')[0])




		line = fp.readline()

	fp.close()

	#l2ConfigStr = '{}'.format(l2_prefetch_lines)
	print (l2_prefetch_lines)
	x_Axis.append(l2_prefetch_lines)
	ipc_Axis.append(ipc)
	mpki_l1_Axis.append(mpki_l1)
	mpki_l2_Axis.append(mpki_l2)

print (x_Axis)
print (ipc_Axis)
print (mpki_l1_Axis)
print (mpki_l2_Axis)

fig, ax1 = plt.subplots()
ax1.grid(True)
ax1.set_xlabel("Prefetched Blocks")

xAx = np.arange(len(x_Axis))
ax1.xaxis.set_ticks(np.arange(0, len(x_Axis), 1))
ax1.set_xticklabels(x_Axis, rotation=45)
ax1.set_xlim(-0.5, len(x_Axis) - 0.5)
ax1.set_ylim(min(ipc_Axis) - 0.05 * min(ipc_Axis), max(ipc_Axis) + 0.05 * max(ipc_Axis))
ax1.set_ylabel("$IPC$")
line1 = ax1.plot(ipc_Axis, label="ipc", color="red",marker='x')

ax2 = ax1.twinx()
ax2.xaxis.set_ticks(np.arange(0, len(x_Axis), 1))
ax2.tick_params(axis='y', colors='green')
ax2.set_xticklabels(x_Axis, rotation=45)
ax2.set_xlim(-0.5, len(x_Axis) - 0.5)
ax2.set_ylim(min(mpki_l1_Axis) - 0.05 * min(mpki_l1_Axis), max(mpki_l1_Axis) + 0.05 * max(mpki_l1_Axis))
ax2.set_ylabel("$MPKI$")
line2 = ax2.plot(mpki_l1_Axis, label="L1D_mpki", color="green",marker='o')

ax3 = ax1.twinx()
ax3.xaxis.set_ticks(np.arange(0, len(x_Axis), 1))
ax3.tick_params(axis='y', colors='blue')
ax3.set_xticklabels(x_Axis, rotation=45)
ax3.set_xlim(-0.5, len(x_Axis) - 0.5)
ax3.set_ylim(min(mpki_l2_Axis) - 0.05 * min(mpki_l2_Axis), max(mpki_l2_Axis) + 0.05 * max(mpki_l2_Axis))
line3 = ax3.plot(mpki_l2_Axis, label="L2D_mpki", color="blue",marker='o')

lns = line1 + line2 + line3
labs = [l.get_label() for l in lns]

plt.title("IPC vs MPKI_l1 vs MPKI_l2")
lgd = plt.legend(lns, labs)
lgd.draw_frame(False)
plt.savefig("Prefetching.png",bbox_inches="tight")
