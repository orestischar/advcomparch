#!/usr/bin/env python

import sys, os
import itertools, operator
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np
import matplotlib.gridspec as gridspec
from random import choice


def get_params_from_basename(basename):
	tokens = basename.split('.')
	bench = tokens[0]
	input_size = 'ref'
	dw = int(tokens[1].split('-')[0].split('_')[1])
	ws = int(tokens[1].split('-')[1].split('_')[1])
	return (bench, input_size, dw, ws)

def get_energy_delay_from_output_file(output_file):

	EDP_1 = 0
	EDP_2 = 0
	EDP_3 = 0

	fp = open(output_file, "r")
	line = fp.readline()
	while line:
		if 'total' in line:
			#area = float(line.split()[2	].replace(',', ''))
			power = float(line.split()[1])
			if (line.split()[2] == 'kW'):
				power = power*1000

			energy = float(line.split()[3])
			if (line.split()[4] == 'kJ'):
				energy = energy*1000.0

			delay = energy/power

			EDP_1 = energy*delay
			EDP_2 = energy*delay**2
			EDP_3 = energy*delay**3

		line = fp.readline()

	fp.close()
	return (EDP_1, EDP_2, EDP_3)

def tuples_by_dispatch_width(tuples):
	ret = []
	tuples_sorted = sorted(tuples, key=operator.itemgetter(0))
	for key,group in itertools.groupby(tuples_sorted,operator.itemgetter(0)):
		ret.append((key, list(zip(*map(lambda x: x[1:], list(group))))))
	return ret

global_ws = [16,32,64,96,128,192,256,384]
#global_ws = [16,32,64,128,256,512]

if len(sys.argv) < 2:
	print ("usage:", sys.argv[0], "<output_directories>")
	sys.exit(1)


results_tuples = [[] for i in range(3)]

for dirname in sys.argv[1:]:
	if dirname.endswith("/"):
		dirname = dirname[0:-1]
	basename = os.path.basename(dirname)
	output_file = dirname + "/power.total.out"

	(bench, input_size, dispatch_width, window_size) = get_params_from_basename(basename)
	if (window_size < 16): continue
	(EDP_1, EDP_2, EDP_3) = get_energy_delay_from_output_file(output_file)
	results_tuples[0].append((dispatch_width, window_size, EDP_1))
	results_tuples[1].append((dispatch_width, window_size, EDP_2))
	results_tuples[2].append((dispatch_width, window_size, EDP_3))


markers = ['.', 'o', 'v', '*', 'D']

for i in range(3):

	fig = plt.figure()
	plt.grid(True)
	ax = plt.subplot(111)

	ax.set_xlabel("$Window Size$")
	if (i == 0):
		ylabel = 'EDP [ $J \cdot s$ ]'
	else:
		ylabel = 'ED^' + str(i+1) + 'P  [ $J \cdot s$^' + str(i+1) + ']'
	ax.set_ylabel(ylabel)

	j = 0
	tuples_by_dw = tuples_by_dispatch_width(results_tuples[i])
	for tuple in tuples_by_dw:
		dw = tuple[0]
		ws_axis = tuple[1][0]
		area_axis = tuple[1][1]
		x_ticks = np.arange(0, len(global_ws))
		x_labels = map(str, global_ws)
		ax.xaxis.set_ticks(x_ticks)
		ax.xaxis.set_ticklabels(x_labels)

		area_axis = list(area_axis)

		print (x_ticks)
		print (area_axis)
		ax.plot(x_ticks, area_axis, label="DW_"+str(dw), marker=markers[i%len(markers)], markersize=3)
		j = j + 1
	
	if (i == 0):
		plt.title(bench)
	else:
		plt.title(bench)
	lgd = plt.legend(ncol=len(tuples_by_dw), bbox_to_anchor=(0.9, -0.1), prop={'size':8})
	plt.savefig(bench+'ED^'+str(i+1)+'P-'+input_size+'.energy_delay.png', bbox_extra_artists=(lgd,), bbox_to_anchor=(1,1), bbox_inches='tight')

#lgd = plt.legend(ncol=len(tuples_by_dw), bbox_to_anchor=(0.9, -0.1), prop={'size':8})