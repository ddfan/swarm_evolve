#!/usr/bin/env python

import sys
import numpy as np
import numpy.matlib
import matplotlib.pyplot as plt

maxscore=110

filepath=sys.argv[1]
arr=np.loadtxt(filepath)
std=np.std(arr,axis=1)
mean=np.mean(arr,axis=1)
shape=arr.shape
x=np.matlib.repmat(np.arange(shape[0]), shape[1], 1)
x=x.transpose().flatten()
y=arr.flatten()

plt.figure()
#plt.errorbar(np.arange(0,len(mean)),mean,yerr=std)
plt.scatter(x,y,color='b', s=50, alpha=.1)
plt.plot(np.arange(0,len(mean)),mean,color='k')
#plt.plot(np.arange(0,len(mean)),mean*0+maxscore,color='k')
axes = plt.gca()
axes.set_xlim([0,len(mean)])
plt.show()
