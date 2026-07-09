import matplotlib.pyplot as plt
import numpy as np

data = np.loadtxt('timing.txt')
frame, ms = data[:,0], data[:,1]

plt.scatter(frame, ms, s=5, label='per-frame time')
z = np.polyfit(frame, ms, 2)
plt.plot(frame, np.polyval(z, frame), 'r-', label='trend')
plt.axhline(50, color='g', linestyle='-', label='deadline (50ms)')
plt.xlabel('Frame #'); plt.ylabel('Total time (ms)')
plt.legend()
plt.savefig('jitter_plot.png')