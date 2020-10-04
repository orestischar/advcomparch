#!/usr/bin/env python

import sys, os
import itertools, operator
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np

def get_params_from_output_file(output_file):
	results_tuples = []
	time = 0
	fp = open(output_file, "r")
	line = fp.readline()
	while line:
		tokens = line.split('-')
		mechanism = tokens[0].split('.')[0]
		threads = int(tokens[1].split('_')[0])
		grain = int(tokens[2])
		line = fp.readline()
		time = float(line.split()[2])
		results_tuples.append((mechanism, threads, time))
		line = fp.readline()
		line = fp.readline()
	fp.close()
	return (results_tuples, grain)

def tuples_by_mechanism(tuples):
	ret = []
	tuples_sorted = sorted(tuples, key=operator.itemgetter(0))
	for key,group in itertools.groupby(tuples_sorted,operator.itemgetter(0)):
		ret.append((key, tuple(zip(*map(lambda x: x[1:], list(group))))))
	return ret

global_threads = [1, 2, 4, 8]

if len(sys.argv) < 2:
	print ("usage:", sys.argv[0], "<output_directories>")
	sys.exit(1)

for dirname in sys.argv[1:]:
	results = []
	if dirname.endswith("/"):
		dirname = dirname[0:-1]
	basename = os.path.basename(dirname)
	output_file = dirname
	(results, grain) = get_params_from_output_file(output_file)	



markers = ['.', 'v', 's', 'h', '^']
fig = plt.figure()
plt.grid(True)
ax = plt.subplot(111)
ax.set_xlabel("$Threads$")
ax.set_ylabel("$Time(sec)$")

i = 0
tuples_by_mech = tuples_by_mechanism(results)
for tuple in tuples_by_mech:
	mechanism = tuple[0]
	threads_axis = tuple[1][0]
	time_axis = tuple[1][1]
	x_ticks = np.arange(0, len(global_threads))
	x_labels = map(str, global_threads)
	ax.xaxis.set_ticks(x_ticks)
	ax.xaxis.set_ticklabels(x_labels)

	print (x_ticks)
	print (time_axis)
	ax.plot(x_ticks, time_axis, label=str(mechanism), marker=markers[i%len(markers)], markersize=3)
	i = i + 1

plt.title("Grain size = " + str(grain))
lgd = ax.legend(ncol=len(tuples_by_mech), bbox_to_anchor=(0.9, -0.1), prop={'size':8})
plt.savefig('Grain'+str(grain)+'.realtime.png', bbox_extra_artists=(lgd,), bbox_inches='tight')
