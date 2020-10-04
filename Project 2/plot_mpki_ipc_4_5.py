#!/usr/bin/env python3

import sys
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np

## For nbit predictors
predictors_to_plot = ["  StaticTakenPredictor", "  BTFNTPredictor", "  Nbit-16K-2", "  Pentium-M", "  Local_1",  "  Local_2",  "  Global_1",  "  Global_2",  "  Global_3",  "  Global_4",  "  Alpha",  "  Tournament_1",  "  Tournament_2",  "  Tournament_3",  "  Tournament_4"]

x_Axis = []
mpki_Axis = []

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
				tcorrect_predictions = float(tokens[1])
				incorrect_predictions = float(tokens[2])
				x_Axis.append(predictor_string)
				mpki_Axis.append(incorrect_predictions / (total_ins / 1000.0))
				#print(predictor_string, " ", type_of_branch / total_br)

	line = fp.readline()

fig, ax1 = plt.subplots()
ax1.grid(True)

xAx = np.arange(len(x_Axis))
ax1.bar(xAx, mpki_Axis, align='center', alpha=0.5, color='maroon')

ax1.xaxis.set_ticks(np.arange(0, len(x_Axis), 1))
ax1.set_xticklabels(x_Axis, rotation=45)
ax1.set_ylabel("$MPKI$")
#line1 = ax1.plot(y_Axis, label="mpki", color="",marker='x')

plt.title("MPKI - Different Predictors")
plt.savefig(("plot"),bbox_inches="tight")
