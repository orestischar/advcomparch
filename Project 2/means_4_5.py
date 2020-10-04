import sys
from scipy import stats
import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt


x_Axis = []
dir_mpki=[ [],[],[],[],[],[],[],[],[],[],[],[],[],[],[] ]


outpath = "./outputs/4_5/"

predictors_to_plot = ["StaticTakenPredictor", "BTFNTPredictor", "Nbit-16K-2", "Pentium-M", "Local_1","Local_2","Global_1","Global_2","Global_3","Global_4","Alpha","Tournament_1","Tournament_2","Tournament_3","Tournament_4"]

benchmarks = ["403.gcc", "429.mcf", "434.zeusmp", "436.cactusADM", "445.gobmk", "450.soplex", "456.hmmer", "458.sjeng", "459.GemsFDTD", "471.omnetpp", "473.astar","483.xalancbmk"]


for bench in benchmarks:
	benchfile = outpath + bench + ".cslab_branch_predictors.out"
	fp = open(benchfile)
	line = fp.readline()
	print("now processing " + bench +" file")
	while line:
		if line.startswith("Total Instructions:"):
			tokens = line.split()
			total_instr = float(tokens[2])
		elif line.startswith("Branch Predictors:"):
			for i in range(15):
				line = fp.readline()
				tokens = line.split()
				correct = float(tokens[1])
				incorrect = float(tokens[2])
				#targetCorrect = float(tokens[3])
				mpki = incorrect / (total_instr/1000.0)
				dir_mpki[i].append(mpki)
			break
		line = fp.readline()
	fp.close()
	

fig, ax1 = plt.subplots()

ax1.grid(True)
#ax1.set_xlabel("BTB Predictors (entries, associativity)")

xAx = np.arange(len(predictors_to_plot))
ax1.xaxis.set_ticks(np.arange(0, len(predictors_to_plot), 1))
ax1.set_xticklabels(predictors_to_plot, rotation=45)
#ax1.set_xlim(-1, len(predictors_to_plot) + 1)

#ax1.set_ylim(0, max(total_branches_axis))
ax1.set_ylabel("$MPKI$")


mpki_axis = []
for i in range(15):
	#change to np.mean(dir_mpki[i]) for arithmetic mean
	#change to stats.mstats.gmean(dir_mpki[i]) for geometric mean
	mpki_axis.append(np.mean(dir_mpki[i]))
print("LALALALLA")
print(mpki_axis)
	

ax1.bar(xAx, mpki_axis, align='center', alpha=0.5, color='saddlebrown')



plt.title("Mean PKI - Different Predictors")


plt.savefig("4.5 mean.png",bbox_inches="tight")