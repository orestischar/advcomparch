#!/usr/bin/env python3

import sys
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np

## For nbit predictors
predictors_to_plot = ["  Conditional-Taken-Branches", "  Conditional-NotTaken-Branches", "  Unconditional-Branches", "  Calls", "  Returns"]

x_Axis = []
y_Axis = []

fp = open(sys.argv[1])
line = fp.readline()
while line:
	tokens = line.split()
	if (line.startswith("  Total-Branches:")):
		total_br = int(tokens[1])
	else:
		for pred_prefix in predictors_to_plot:
			if line.startswith(pred_prefix):
				predictor_string = tokens[0].split(':')[0]
				type_of_branch = float(tokens[1])
				x_Axis.append(predictor_string)
				y_Axis.append(100.0 * type_of_branch / total_br)
				#print(predictor_string, " ", type_of_branch / total_br)

	line = fp.readline()

fig, ax1 = plt.subplots()
ax1.grid(True)

xAx = np.arange(len(x_Axis))
ax1.bar(xAx, y_Axis, align='center', alpha=0.5)

ax1.xaxis.set_ticks(np.arange(0, len(x_Axis), 1))
ax1.set_xticklabels(x_Axis, rotation=45)
ax1.set_ylabel("$percentage _of_branches$")
#line1 = ax1.plot(y_Axis, label="mpki", color="",marker='x')

plt.title("Branch Statistics for the " + str (total_br) + " branches")
plt.savefig(("plot"),bbox_inches="tight")
