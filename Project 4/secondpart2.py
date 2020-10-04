#!/usr/bin/env python

import sys, os
import itertools, operator
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np

# data to plot
n_groups = 5
share_all=(0.89, 0.80, 0.86, 0.85, 1.36)
share_L3=(0.88, 0.79, 1.00, 0.97, 1.39)
share_noth=(0.89, 0.77, 1.01, 0.90, 1.39)

# create plot
fig, ax = plt.subplots()
index = np.arange(n_groups)
bar_width = 0.2
opacity = 0.8

rects1 = plt.bar(index, share_all, bar_width,
alpha=opacity,
color='darkred',
label='share all')

rects2 = plt.bar(index + bar_width, share_L3, bar_width,
alpha=opacity,
color='darkmagenta',
label='share L3')

rects3 = plt.bar(index + 2*bar_width, share_noth, bar_width,
alpha=opacity,
color='peru',
label='share nothing')

#plt.xlabel('Person')
plt.ylabel('Energy')
plt.title('Energy for various topologies')
plt.xticks(index + bar_width, ('DTAS_CAS', 'DTAS_TS', 'DTTAS_CAS', 'DTTAS_TS', 'MUTEX'))
plt.legend()
plt.grid(True, 'major', 'y')
lgd = ax.legend(bbox_to_anchor=(0.9, -0.1), prop={'size':8})
plt.savefig('plot2', bbox_extra_artists=(lgd,), bbox_inches='tight')
