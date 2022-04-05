import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

plotname = 'enwik9-tab-bv-lz-withoutdictionary'

data = pd.read_excel('plot.xlsx')


legend1 = 'TAB-LZ'
legend2 = 'TAB-LZ with dictionary'
legend3 = 'BV-LZ'
legend4 = 'BV-LZ with dictionary'
xlabel1 = 'Compressed filesize (in GB)'
ylabel1 = 'Avg random access time (in ms)'
xaxis = 'compsize'
yaxis =  'randactime'

plt1, = plt.plot(data[xaxis]/10**9,data[yaxis]*10**3, color='black', marker='o', linewidth=2, markersize=7, label=legend1)
#plt2, = plt.plot(data[xaxis+'2']/10**9,data[yaxis+'2']*10**3, color='blue', linestyle='dashed', marker='x', linewidth=2, markersize=7, label=legend2)
plt3, = plt.plot(data[xaxis+'3']/10**9,data[yaxis+'3']*10**3, color='red', linestyle='dotted', marker='s', linewidth=2, markersize=7, label=legend3)
#plt4, = plt.plot(data[xaxis+'4']/10**9,data[yaxis+'4']*10**3, color='green', linestyle='dashdot', marker='*', linewidth=2, markersize=7, label=legend4)
plt.grid(True)
plt.xlabel(xlabel1)
plt.ylabel(ylabel1)
plt.legend()
plt.savefig(plotname+'.pdf')
