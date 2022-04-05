import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

plotname = 'enwik9-tab-bv-lz-withoutdictionary'

#data = pd.read_excel('plot.xlsx')


tab_dict_X1 = [415309220, 360900023,346591711,317984579,314410989]
tab_dict_Y1 = [0.000003, 0.000011,0.000021,0.000170,0.001319]

tab_X1 = [569136229,453632734,415753038,366972559,358272549]
tab_y1 = [0.000004,0.000010,0.000017,0.000110,0.000979]

Bv_dict_X1 = [407223511,359352615,345835045,317912448,314404561]
Bv_dict_Y1 = [0.000003,0.000011,0.000068 , 0.000168,0.001358] 

Bv_X1 = [561104152,452074029,414991492,366899999,358266083]
Bv_Y1 = [0.000004,0.000011,0.000017,0.000110,0.000984]


tab_dict_X = [element /   10**9  for element in tab_dict_X1   ]
tab_dict_Y = [element *10**3  for  element in tab_dict_Y1  ]
tab_X = [element /   10**9  for element in tab_X1  ]
tab_y = [element *10**3 for element in tab_y1  ]

Bv_dict_X = [element /   10**9  for element in Bv_dict_X1   ]
Bv_dict_Y = [element *10**3  for element in Bv_dict_Y1   ]
Bv_X = [element /   10**9  for element in Bv_X1   ]
Bv_Y = [element *10**3  for element in Bv_Y1  ]



#[element /   1073741824  for element in Enwik8_dict_Compression_Size_RLZ   ]


legend1 = 'TAB-LZ'
legend2 = 'TAB-LZ with dictionary'
legend3 = 'BV-LZ'
legend4 = 'BV-LZ with dictionary'
xlabel1 = 'Compressed filesize (in GB)'
ylabel1 = 'Avg random access time (in ms)'
xaxis = 'compsize'
yaxis =  'randactime'

plt1, = plt.plot(tab_X,tab_y, color='black', marker='o', linewidth=2, markersize=7, label=legend1)
plt2, = plt.plot(tab_dict_X,tab_dict_Y, color='blue', linestyle='dashed', marker='x', linewidth=2, markersize=7, label=legend2)
plt3, = plt.plot(Bv_X,Bv_Y, color='red', linestyle='dotted', marker='s', linewidth=2, markersize=7, label=legend3)
plt4, = plt.plot(Bv_dict_X,Bv_dict_Y, color='green', linestyle='dashdot', marker='*', linewidth=2, markersize=7, label=legend4)
plt.grid(True)
plt.xlabel(xlabel1)
plt.ylabel(ylabel1)
plt.legend()
plt.savefig(plotname+'.pdf')
