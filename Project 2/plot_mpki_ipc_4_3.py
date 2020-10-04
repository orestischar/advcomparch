#!/usr/bin/env python3

import sys
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np

## For nbit predictors
predictors_to_plot = [ "  BTB-512-1", "  BTB-512-2", "  BTB-256-2", "  BTB-256-4", "  BTB-128-4", "  BTB-64-8"]

x_Axis = []
mpki_Axis = []
correct_Axis = []
target_correct_Axis = []

fp = open(sys.argv[1])
line = fp.readline()
while line:
	tokens = line.split()
	if (line.startswith("Total Instructions:")):
		total_ins = float(tokens[2])
	else:
		for pred_prefix in predictors_to_plot:
			if line.startswith(pred_prefix):
				predictor_string = tokens[0].split(':')[0]
				correct_predictions = float(tokens[1])
				incorrect_predictions = float(tokens[2])
				target_correct_predictions = float(tokens[3])
				x_Axis.append(predictor_string)
				mpki_Axis.append(incorrect_predictions / (total_ins / 1000.0))
				correct_Axis.append(correct_predictions / (total_ins / 1000.0))
				target_correct_Axis.append(target_correct_predictions / (total_ins / 1000.0))

	line = fp.readline()

fig, ax1 = plt.subplots()
ax1.grid(True)

xAx = np.arange(len(x_Axis))
ax1.xaxis.set_ticks(np.arange(0, len(x_Axis), 1))
ax1.tick_params(axis='y', colors='red')
ax1.set_xticklabels(x_Axis, rotation=45)
ax1.set_xlim(-0.5, len(x_Axis) - 0.5)
ax1.set_ylim(min(mpki_Axis) - 0.05, max(mpki_Axis) + 0.05)
ax1.set_ylabel("$MPKI$")
line1 = ax1.plot(mpki_Axis, label="mpki", color="red",marker='x')

ax2 = ax1.twinx()
ax2.xaxis.set_ticks(np.arange(0, len(x_Axis), 1))
ax2.tick_params(axis='y', colors='chocolate')
ax2.set_xticklabels(x_Axis, rotation=45)
ax2.set_xlim(-0.5, len(x_Axis) - 0.5)
ax2.set_ylim(min(correct_Axis) - 0.05, max(correct_Axis) + 0.05)
ax2.set_ylabel("correct prediction PKI")
line2 = ax2.plot(correct_Axis, label="correct prediction PKI", color="chocolate",marker='o')

ax3 = ax1.twinx()
ax3.xaxis.set_ticks(np.arange(0, len(x_Axis), 1))
ax3.tick_params(axis='y', colors='darkmagenta')
ax3.set_xticklabels(x_Axis, rotation=45)
ax3.set_xlim(-0.5, len(x_Axis) - 0.5)
ax3.set_ylim(min(target_correct_Axis) - 0.05, max(target_correct_Axis) + 0.05)
#ax3.set_ylabel("$target correct prediction PKI$")
line3 = ax3.plot(target_correct_Axis, label="target correct prediction PKI", color="darkmagenta",marker='x')

lns = line1 + line2 + line3
labs = [l.get_label() for l in lns]
lgd = plt.legend(lns, labs)
lgd.draw_frame(False)
plt.title("BTB")
plt.savefig(("plot"),bbox_inches="tight")
