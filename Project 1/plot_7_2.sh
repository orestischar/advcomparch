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

for outFile in sys.argv[1:]:
	fp = open(outFile)
	line = fp.readline()
	while line:
		tokens = line.split()
		if (line.startswith("INSTRUCTIONS:")):
			instr = int(tokens[1])
			ipc_instr = float(tokens[4])
			x_Axis.append(instr)
			ipc_Axis.append(ipc_instr)
			if (instr==1): 
				ipc_1 = ipc_instr
			elif (instr==10): 
				ipc_10 = ipc_instr
		elif (line.startswith("IPC:")):
			ipc_final = float(tokens[1])

		line = fp.readline()

	fp.close()

#print (l1ConfigStr)

#print (x_Axis)
#print (ipc_Axis)
#plt.plot(x_Axis, ipc_Axis)
fig, ax1 = plt.subplots()
ax1.grid(True)
ax1.set_xlabel("Instructions * 10^7")

#xAx = np.arange(len(x_Axis))
#ax1.xaxis.set_ticks(np.arange(0, len(x_Axis), 1))
#ax1.set_xticklabels(x_Axis, rotation=45)
ax1.set_xlim(0, len(x_Axis))
ax1.set_ylim(min(ipc_Axis) - 0.00005*min(ipc_Axis) , max(ipc_Axis) + 0.00005*min(ipc_Axis))
ax1.set_ylabel("$IPC$")
line1 = plt.plot(ipc_Axis, label="ipc", color="red")
line2 = plt.axhline(ipc_1, label="IPC 10M", color="green",marker='o')
line3 = plt.axhline(ipc_10, label="IPC 100M", color="blue",marker='o')
line4 = plt.axhline(ipc_final, label="final IPC", color="black",marker='o')


plt.title("IPC in Time")
lgd = plt.legend()
lgd.draw_frame(False)
plt.savefig("7_2.png",bbox_inches="tight")

