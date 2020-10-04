#!/usr/bin/env python3

import sys
import numpy as np
import random
from math import log2


import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

x_Axis = ['8.4.4096B', '16.4.4096B', '32.4.4096B', '64.1.4096B', '64.2.4096B', '64.4.4096B', '64.8.4096B', '64.16.4096B', '64.32.4096B', '64.64.4096B', '128.4.4096B', '256.4.4096B']
ipc1_Axis = [0.25392694, 0.33852755, 0.40573407, 0.33623345, 0.40008259, 0.42479961, 0.45634942, 0.46248094, 0.46418112, 0.46447044, 0.43316641, 0.43843619]
ipc2_Axis = [0.25392694, 0.3046748, 0.3286446, 0.27159467, 0.30701075, 0.30967891, 0.31604479, 0.30427662, 0.29012544, 0.27579096, 0.28420048, 0.25889219]


fig, ax1 = plt.subplots()
ax1.grid(True)
ax1.set_xlabel("TLBSize.Assoc.BlockSize")

xAx = np.arange(len(x_Axis))
ax1.xaxis.set_ticks(np.arange(0, len(x_Axis), 1))
ax1.set_xticklabels(x_Axis, rotation=45)
ax1.set_xlim(-0.5, len(x_Axis) - 0.5)

ax1.set_ylabel("$IPC$")
line1 = ax1.plot(ipc1_Axis, label="previous ipc", color="palevioletred",marker='x')
line2 = ax1.plot(ipc2_Axis, label="current ipc", color="red",marker='x')


lns = line1 + line2
labs = [l.get_label() for l in lns]


plt.title("Mean Values: IPC Comparison")
lgd = plt.legend(lns, labs)
lgd.draw_frame(False)
plt.savefig("comp.png",bbox_inches="tight")