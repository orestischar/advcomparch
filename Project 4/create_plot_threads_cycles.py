#!/usr/bin/env python

import sys, os
import itertools, operator
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np

def get_params_from_basename(basename):
	tokens = basename.split('.')
	mechanism = tokens[0]
	threads = int(tokens[1].split('-')[0].split('_')[1])
	grain = int(tokens[1].split('-')[1].split('_')[1])
	return (mechanism, threads, grain)

def get_cycles_from_output_file(output_file):
	cycles = 0
	fp = open(output_file, "r")
	line = fp.readline()
	while line:
		if "Cycles" in line:
			cycles = float(line.split()[2])
		line = fp.readline()
	fp.close()
	return cycles

def tuples_by_mechanism(tuples):
	ret = []
	tuples_sorted = sorted(tuples, key=operator.itemgetter(0))
	for key,group in itertools.groupby(tuples_sorted,operator.itemgetter(0)):
		ret.append((key, tuple(zip(*map(lambda x: x[1:], list(group))))))
	return ret

global_threads = [1, 2, 4, 8, 16]

if len(sys.argv) < 2:
	print ("usage:", sys.argv[0], "<output_directories>")
	sys.exit(1)

results_tuples = []

for dirname in sys.argv[1:]:
	if dirname.endswith("/"):
		dirname = dirname[0:-1]
	basename = os.path.basename(dirname)
	output_file = dirname + "/sim.out"

	(mechanism, threads, grain) = get_params_from_basename(basename)
	cycles = get_cycles_from_output_file(output_file)
	results_tuples.append((mechanism, threads, cycles))


markers = ['.', 'v', 's', 'h', '^']
fig = plt.figure()
plt.grid(True)
ax = plt.subplot(111)
ax.set_xlabel("$Threads$")
ax.set_ylabel("$Cycles$")

i = 0
tuples_by_mech = tuples_by_mechanism(results_tuples)
for tuple in tuples_by_mech:
	mechanism = tuple[0]
	threads_axis = tuple[1][0]
	cycles_axis = tuple[1][1]
	x_ticks = np.arange(0, len(global_threads))
	x_labels = map(str, global_threads)
	ax.xaxis.set_ticks(x_ticks)
	ax.xaxis.set_ticklabels(x_labels)

	print (x_ticks)
	print (cycles_axis)
	ax.plot(x_ticks, cycles_axis, label=str(mechanism), marker=markers[i%len(markers)], markersize=3)
	i = i + 1

plt.title("Grain size = " + str(grain))
lgd = ax.legend(ncol=len(tuples_by_mech), bbox_to_anchor=(0.9, -0.1), prop={'size':8})
plt.savefig('Grain'+str(grain)+'.cycles.png', bbox_extra_artists=(lgd,), bbox_inches='tight')
