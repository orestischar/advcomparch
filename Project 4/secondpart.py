#!/usr/bin/env python

import sys, os
import itertools, operator
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np

# data to plot
n_groups = 5
share_all=(608241, 438799, 592055, 582382, 10444642)
share_L3=(960860, 686348, 830923, 701550, 10228819)
share_noth=(1014100, 661610, 816785, 702711, 10263988)

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
plt.ylabel('Cycles')
plt.title('Cycles for various topologies')
plt.xticks(index + bar_width, ('DTAS_CAS', 'DTAS_TS', 'DTTAS_CAS', 'DTTAS_TS', 'MUTEX'))
plt.yticks(np.arange(0, 11000000, 1000000))
plt.legend()
plt.grid(True, 'major', 'y')
lgd = ax.legend(bbox_to_anchor=(0.9, -0.1), prop={'size':8})
plt.savefig('plot1', bbox_extra_artists=(lgd,), bbox_inches='tight')
