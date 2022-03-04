import matplotlib.pyplot as plt
import seaborn as sns
# sns.set_theme()

########################for Enwik8 data##########################################
Enwik8_Chunk_size = [10000,19000,28000,37000,46000,55000,64000,73000,82000,91000,100000 ]
# with dictionary compression 
Enwik8_dict_Compression_Size_RLZ = [36676181,35909723,35557828,35404362,35292227,35191545,35163084,35113027,35093452,35080922,35034613]
Enwik8_dict_Ave_Runtime_RLZ = [0.000021,0.000039,0.000056,0.000073,0.000089,0.000106,0.000122,0.000138,0.000151,0.000166,0.000179]
Enwik8_dict_Compression_Size_Bvrlz = [36608429,35874783,35534342,35386803,35278265,35179963,35153213,35104448,35085872,35074152,35028479]
Enwik8_dict_Ave_Runtime_Bvrlz = [0.000021,0.000039,0.000057,0.000073,0.000089,0.000105,0.000121,0.000134,0.000149,0.000166,0.000176]
Enwik8_dict_Compression_Size_DS = [43983965,41064226,39785107,39142822,38615675,38285853,38044482,37903173,37741464,37621590,37436587]
Enwik8_dict_Ave_Runtime_DS = [0.000065,0.000087,0.000106,0.000125,0.000146,0.000163,0.000180,0.000196,0.000212,0.000229,0.000245]
# without dictionary compressoin 
Enwik8_Compression_Size_RLZ = [45062891,44157711,43267583,42707722,42362424,42104034,41921054,41779032,41673651,41582425,41512802]
Enwik8_Ave_Runtime_RLZ = [0.000017,0.000027,0.000035,0.000046,0.000056,0.000065,0.000074,0.000083,0.000094,0.000104,0.000115]
Enwik8_Compression_Size_Bvrlz = [44994542,44122450,43243887,42690010,42348341,42092352,41911099,41770380,41666006,41575597,41506616]
Enwik8_Ave_Runtime_Bvrlz = [0.000017,0.000027,0.000036,0.000048,0.000057,0.000066,0.000074,0.000085,0.000094,0.000105,0.000116]
Enwik8_Compression_Size_DS = [59914123,56455286,53467001,51529111,49944011,49027284,48261515,47694560,47309242,46879962,46491515]
Enwik8_Ave_Runtime_DS = [0.000135,0.000203,0.000236,0.000259,0.000277,0.000296,0.000312,0.000329,0.000349,0.000363,0.000375]





# #################### Enwik9 ###############
Enwik9_Chunk_size = [ 100000,190000,280000,370000,460000,550000,640000 ,730000,820000,910000,1000000]
# with dictionary compressoin 
Enwik9_dict_Compression_Size_RLZ = [318072572,316240026,315581573,315102153,314850453,314785490,314527926 ,314518452,314408957,314228256,314179125]
Enwik9_dict_Ave_Runtime_RLZ = [0.000168,0.000288,0.000419,0.000520,0.000642,0.000743 ,0.000838,0.000949,0.001055 ,0.001149,0.001279 ]
Enwik9_dict_Compression_Size_Bvrlz = [317998647,316201805,315555852,315082897,314835130,314772765,314517073,314509011,314400617,314220800,314172364]
Enwik9_dict_Ave_Runtime_Bvrlz = [0.000165,0.000296,0.000417,0.000524,0.000643,0.000750,0.000863,0.000959,0.001076,0.001167,0.001281]
Enwik9_dict_Compression_Size_DS = [437932230,431392421,428421639,426088267,424448939,424029611,422387942,421499804,421405502,420399488,420038303]
Enwik9_dict_Ave_Runtime_DS = [0.000912,0.001622,0.002310,0.002960,0.003627,0.004285,0.004881,0.005520,0.006188,0.006793,0.007410]

# withot dictionary compressoin 
Enwik9_Compression_Size_RLZ = [367190310,364255150,362418542,360927590,360105824 ,359629940,359188718,358916056,358712991,358497882,358327476]
Enwik9_Ave_Runtime_RLZ = [0.000108,0.000195,0.000265 ,0.000348,0.000431,0.000523,0.000592,0.000684,0.000773,0.000857 ,0.000939]
Enwik9_Compression_Size_Bvrlz = [367115943,364216743,362392667,360908220,360090409,359617139,359177800,358906559,358704602,358490381,358320675]
Enwik9_Ave_Runtime_Bvrlz = [0.000108,0.000196,0.000263,0.000343,0.000433,0.000516,0.000594,0.000690,0.000773,0.000851,0.000936]
Enwik9_Compression_Size_DS = [513973724,500667962,495587888,490499442,487223533,485147948,483348749 ,481514412 ,480976477,480214350 ,479202463]
Enwik9_Ave_Runtime_DS = [0.001084,0.001447,0.002616,0.003319,0.004006, 0.004690, 0.005377,0.006054,0.006752,0.007455,0.008114]



############################## IMDB ###########################
IMDB_Chunk_size = [ 214623,407785,600947,794109,987271,1180433,1373595,1566757,1759919,1953081]
# with dictionary compressoin 
IMDB_dict_Compression_Size_RLZ = [389593543,386557268,384374273,382821983,381183800,380086548,378959540,378072887,377400632,376584654]
IMDB_dict_Ave_Runtime_RLZ = [0.000236,0.000422,0.000605,0.000790,0.000953,0.001169,0.001347,0.001487,0.001664,0.001839]
IMDB_dict_Compression_Size_Bvrlz = [389519116,386518795,384348385,382802603,381168379,380073742,378948617,378063387,377392240,376577152]
IMDB_dict_Ave_Runtime_Bvrlz = [0.000235,0.000423,0.000604,0.000803,0.001008,0.001160,0.001319,0.001489,0.001693,0.001880]
IMDB_dict_Compression_Size_DS = [600840796,568824862,551387509,542685392,538816099,535553956,528708289,525593488,523471895,521206641]
IMDB_dict_Ave_Runtime_DS = [0.001630,0.002616,0.003451,0.004315,0.005228,0.006081,0.006950,0.007757,0.008593,0.009388]

# withot dictionary compressoin 
IMDB_Compression_Size_RLZ = [387391598,384856817,383291804,382756086,382266320,382054105,381854769,381627201,381558994,381472745]
IMDB_Ave_Runtime_RLZ = [0.000168,0.000311,0.000447,0.000592,0.000737,0.000876,0.001021,0.001163,0.001315,0.001457]
IMDB_Compression_Size_Bvrlz = [387317165,384818333,383265907,382736697,382250889,382041291,381843838,381617694,381550596,381465236]
IMDB_Ave_Runtime_Bvrlz = [0.000166,0.000306,0.000453,0.000604,0.000736,0.000892,0.001039,0.001164,0.001310,0.001449]
IMDB_Compression_Size_DS = [588228346,561982695,549791252,541278934,539219376,537902271,536542278,535410059,534932596,534373183]
IMDB_Ave_Runtime_DS = [0.001358,0.002186,0.002969,0.003795,0.004653,0.005521,0.006385,0.007226,0.008073,0.008941]



########################  Protein ##############
Protein_Chunk_size = [ 8581,16305,24029,31753,39477,47201,54925,62649,70373,78097]
# with dictionary compressoin 
Protein_dict_Compression_Size_RLZ = [23057014,21841640,21387941,21132360,20994714,20887087,20751977,20684844,20643733,20632299]
Protein_dict_Ave_Runtime_RLZ = [0.000009,0.000015,0.000021,0.000026,0.000032,0.000038,0.000044,0.000049,0.000055,0.000061]
Protein_dict_Compression_Size_Bvrlz = [22989391,21806781,21364508,21114847,20980785,20875528,20742126,20676283,20636172,20625549]
Protein_dict_Ave_Runtime_Bvrlz = [0.000010,0.000015,0.000020,0.000027,0.000032,0.000038,0.000045,0.000049,0.000056,0.000060]
Protein_dict_Compression_Size_DS = [42103480,39064939,37935450,37443363,37074087,36855404,36651038,36340371,36329877,36278263]
Protein_dict_Ave_Runtime_DS = [0.000123,0.000190,0.000255,0.000323,0.000388,0.000455,0.000516,0.000572,0.000645,0.000711]

# withot dictionary compressoin 
Protein_Compression_Size_RLZ = [29267697,26841211,25936747,25431098,25107721,24887250,24689699,24559823,24471252,24361624]
Protein_Ave_Runtime_RLZ = [0.000009,0.000013,0.000017,0.000020,0.000023,0.000027,0.000030,0.000033,0.000037,0.000042]
Protein_Compression_Size_Bvrlz = [29199480,26806074,25913139,25413456,25093693,24875609,24679779,24551201,24463638,24354826]
Protein_Ave_Runtime_Bvrlz = [0.000009,0.000013,0.000016,0.000020,0.000023,0.000027,0.000030,0.000033,0.000037,0.000041]
Protein_Compression_Size_DS = [52884880,47891585,46156015,45221913,44538715,44116652,43771487,43514226,43343395,43128046]
Protein_Ave_Runtime_DS = [0.000217,0.000326,0.000434,0.000544,0.000646,0.000748,0.000851,0.000952,0.001053,0.001154,]




Titles = ["Enwik8", "Enwik9", "IMDB", "Protein"]
# ################convert below chunk sizes to Kb #####################
E8_chunk = [element * 1e6 for element in Enwik8_Chunk_size]
chunksizes = [[element / 1024 for element in Enwik8_Chunk_size], [element / 1024 for element in Enwik9_Chunk_size], [element / 1024 for element in IMDB_Chunk_size], [element / 1024 for element in Protein_Chunk_size]]

#1048576  for MB
#1073741824   for GB 

Dict_Size_RLZ =   [ [element /   1073741824  for element in Enwik8_dict_Compression_Size_RLZ   ],[element /   1073741824  for element in   Enwik9_dict_Compression_Size_RLZ],[element /   1073741824  for element in     IMDB_dict_Compression_Size_RLZ],[element /   1073741824  for element in     Protein_dict_Compression_Size_RLZ]]
Dict_Size_Bvrlz = [ [element /   1073741824  for element in Enwik8_dict_Compression_Size_Bvrlz ],[element /   1073741824  for element in Enwik9_dict_Compression_Size_Bvrlz],[element /   1073741824  for element in   IMDB_dict_Compression_Size_Bvrlz],[element /   1073741824  for element in   Protein_dict_Compression_Size_Bvrlz]]
Dict_Size_DS =    [ [element /   1073741824  for element in Enwik8_dict_Compression_Size_DS    ],[element /   1073741824  for element in    Enwik9_dict_Compression_Size_DS],[element /   1073741824  for element in      IMDB_dict_Compression_Size_DS],[element /   1073741824  for element in      Protein_dict_Compression_Size_DS]]

Dict_Ave_RLZ =    [ [element * 1e6 for element in Enwik8_dict_Ave_Runtime_RLZ   ], [element * 1e6 for element in         Enwik9_dict_Ave_Runtime_RLZ ], [element * 1e6 for element in           IMDB_dict_Ave_Runtime_RLZ], [element * 1e6 for element in           Protein_dict_Ave_Runtime_RLZ ] ]
Dict_Ave_Bvrlz =  [ [element * 1e6 for element in Enwik8_dict_Ave_Runtime_Bvrlz ], [element * 1e6 for element in       Enwik9_dict_Ave_Runtime_Bvrlz ], [element * 1e6 for element in         IMDB_dict_Ave_Runtime_Bvrlz], [element * 1e6 for element in         Protein_dict_Ave_Runtime_Bvrlz ] ]
Dict_Ave_DS =     [ [element * 1e6 for element in Enwik8_dict_Ave_Runtime_DS    ], [element * 1e6 for element in          Enwik9_dict_Ave_Runtime_DS ], [element * 1e6 for element in            IMDB_dict_Ave_Runtime_DS], [element * 1e6 for element in            Protein_dict_Ave_Runtime_DS ] ]


No_Dict_Dict_Size_RLZ = [ [element /   1073741824  for element in Enwik8_Compression_Size_RLZ   ],[element /   1073741824  for element in     Enwik9_Compression_Size_RLZ], [element /   1073741824  for element in     IMDB_Compression_Size_RLZ] ,[element /   1073741824  for element in      Protein_Compression_Size_RLZ ]]
No_Dict_Size_Bvrlz =    [ [element /   1073741824  for element in Enwik8_Compression_Size_Bvrlz ],[element /   1073741824  for element in   Enwik9_Compression_Size_Bvrlz], [element /   1073741824  for element in   IMDB_Compression_Size_Bvrlz] ,[element /   1073741824  for element in    Protein_Compression_Size_Bvrlz ]]
No_Dict_Size_DS =       [ [element /   1073741824  for element in Enwik8_Compression_Size_DS    ],[element /   1073741824  for element in      Enwik9_Compression_Size_DS], [element /   1073741824  for element in      IMDB_Compression_Size_DS] ,[element /   1073741824  for element in       Protein_Compression_Size_DS ]]

No_Dict_Ave_RLZ =       [[element * 1e6 for element in  Enwik8_Ave_Runtime_RLZ   ], [element * 1e6 for element in    Enwik9_Ave_Runtime_RLZ ], [element * 1e6 for element in    IMDB_Ave_Runtime_RLZ ] ,[element * 1e6 for element in   Protein_Ave_Runtime_RLZ ] ]
No_Dict_Ave_Bvrlz =     [[element * 1e6 for element in  Enwik8_Ave_Runtime_Bvrlz ], [element * 1e6 for element in  Enwik9_Ave_Runtime_Bvrlz ], [element * 1e6 for element in  IMDB_Ave_Runtime_Bvrlz ] ,[element * 1e6 for element in Protein_Ave_Runtime_Bvrlz ] ]
No_Dict_Ave_DS =        [[element * 1e6 for element in  Enwik8_Ave_Runtime_DS    ], [element * 1e6 for element in     Enwik9_Ave_Runtime_DS ], [element * 1e6 for element in     IMDB_Ave_Runtime_DS ] ,[element * 1e6 for element in    Protein_Ave_Runtime_DS ] ]




Png_name_RLZ = ["../figs/Enwik8_RLZ.png", "../figs/Enwik9_RLZ.png", "../figs/IMDB_RLZ.png", "../figs/Protein_RLZ.png"]
Pdf_name_RLZ = ["../figs/Enwik8_RLZ.pdf", "../figs/Enwik9_RLZ.pdf", "../figs/IMDB_RLZ.pdf" ,"../figs/Protein_RLZ.pdf"]

Png_name_BV_RLZ = ["../figs/Enwik8_BV_RLZ.png", "../figs/Enwik9_BV_RLZ.png", "../figs/IMDB_BV_RLZ.png" ,"../figs/Protein_BV_RLZ.png"]
Pdf_name_BV_RLZ = ["../figs/Enwik8_BV_RLZ.pdf", "../figs/Enwik9_BV_RLZ.pdf", "../figs/IMDB_BV_RLZ.pdf" ,"../figs/Protein_BV_RLZ.pdf"]

Png_name_DS = ["../figs/Enwik8_DS.png", "../figs/Enwik9_DS.png", "../figs/IMDB_DS.png", "../figs/Protein_DS.png"]
Pdf_name_DS = ["../figs/Enwik8_DS.pdf", "../figs/Enwik9_DS.pdf", "../figs/IMDB_DS.pdf" ,"../figs/Protein_DS.pdf"]



for i in range(4):
    plt.figure(figsize=(10,24))
    plt.subplot(3,1, 1)
    plt.plot(chunksizes[i], Dict_Size_RLZ[i],label="RLZ with dictionary",color="g", linewidth=3, marker="s")
    plt.plot(chunksizes[i], No_Dict_Dict_Size_RLZ[i], label="RLZ without dictionary" ,color="r", linewidth=3, marker="o")
    plt.title(Titles[i] + "-RLZ \n Chunk size v/s Total Compression Size ", fontsize=15)
    plt.xlabel("Chunk size (in KB)", fontsize=15)
    plt.ylabel("Compressed file size (in GB)", fontsize=15)
    plt.grid()
    plt.legend()
    plt.subplot(3,1, 2)
    plt.plot(Dict_Size_RLZ[i], Dict_Ave_RLZ[i], color="g", linewidth=3, marker="s")
    plt.title(Titles[i] + " \n RLZ with dictionary \nCompressed size v/s Random Access time", fontsize=15)
    plt.xlabel("Compressed file size (in GB)", fontsize=15)
    plt.ylabel("Avg. random access time for a chunk (in ms)", fontsize=15)
    plt.grid()
    plt.subplot(3,1, 3)
    plt.plot(No_Dict_Dict_Size_RLZ[i], No_Dict_Ave_RLZ [i], color="r", linewidth=3, marker="s")
    plt.title(Titles[i] + " \n RLZ without dictionary  \nCompressed size v/s Random Access time", fontsize=15)
    plt.xlabel("Compressed file size (in GB)", fontsize=15)
    plt.ylabel("Avg. random access time for a chunk (in ms)", fontsize=15)
    plt.grid()
    plt.subplots_adjust(wspace=0.2, 
                        hspace=0.4)
    plt.savefig(Png_name_RLZ[i])
    plt.savefig(Pdf_name_RLZ[i])
    #plt.show()

    plt.figure(figsize=(10,24))
    plt.subplot(3,1, 1)
    plt.plot(chunksizes[i], Dict_Size_Bvrlz[i], label="Dictinary compression - Bvrlz" ,color="g", linewidth=3, marker="s")
    plt.plot(chunksizes[i], No_Dict_Size_Bvrlz[i], label="Without Dictinary compression - Bvrlz" ,color="r", linewidth=3, marker="o")
    plt.title(Titles[i] + " -  BV - RLZ  \n Chunk size v/s Total Compression Size", fontsize=15)
    plt.xlabel("Chunk size (in KB)", fontsize=15)
    plt.ylabel("Compressed file size (in GB)", fontsize=15)
    plt.grid()
    plt.legend()
    plt.subplot(3,1, 2)
    plt.plot(Dict_Size_Bvrlz[i], Dict_Ave_Bvrlz[i],color="g", linewidth=3, marker="s")
    plt.title(Titles[i] + "  \n BV - RLZ with dictionary\nCompressed size v/s Random Access time" , fontsize=15)
    plt.xlabel("Compressed file size (in GB)", fontsize=15)
    plt.ylabel("Avg. random access time for a chunk (in ms)", fontsize=15)
    plt.grid()
    plt.subplot(3,1,3)
    plt.plot(No_Dict_Size_Bvrlz[i], No_Dict_Ave_Bvrlz[i],color="r", linewidth=3, marker="s")
    plt.title(Titles[i] + "  \n BV - RLZ without dictionary  \nCompressed size v/s Random Access time", fontsize=15)
    plt.xlabel("Compressed file size (in GB)", fontsize=15)
    plt.ylabel("Avg. random access time for a chunk (in ms)", fontsize=15)
    plt.grid()
    plt.subplots_adjust(wspace=0.2, 
                        hspace=0.4)
    plt.savefig(Png_name_BV_RLZ[i])
    plt.savefig(Pdf_name_BV_RLZ[i])
    #plt.show()

    plt.figure(figsize=(10,24))
    plt.subplot(3,1, 1)
    plt.plot(chunksizes[i], Dict_Size_DS[i], label="Dictinary compression - Dense Sparse" ,color="g", linewidth=3, marker="s")
    plt.plot(chunksizes[i], No_Dict_Size_DS[i], label="Without Dictinary compression - Dense Sparse" ,color="r", linewidth=3, marker="o")
    plt.title(Titles[i] + " -  Dense Sparse  \n Chunk size v/s Total Compression Size", fontsize=15)
    plt.xlabel("Chunk size (in KB)", fontsize=15)
    plt.ylabel("Compressed file size (in GB)", fontsize=15)
    plt.grid()
    plt.legend()
    plt.subplot(3,1, 2)
    plt.plot(Dict_Size_DS[i], Dict_Ave_DS[i],color="g", linewidth=3, marker="s")
    plt.title(Titles[i] + "  \n Dense Sparse with dictionary\nCompressed size v/s Random Access time" , fontsize=15)
    plt.xlabel("Compressed file size (in GB)", fontsize=15)
    plt.ylabel("Avg. random access time for a chunk (in ms)", fontsize=15)
    plt.grid()
    plt.subplot(3,1,3)
    plt.plot(No_Dict_Size_DS[i], No_Dict_Ave_DS[i],color="r", linewidth=3, marker="s")
    plt.title(Titles[i] + "  \n Dense Sparse without dictionary  \nCompressed size v/s Random Access time", fontsize=15)
    plt.xlabel("Compressed file size (in GB)", fontsize=15)
    plt.ylabel("Avg. random access time for a chunk (in ms)", fontsize=15)
    plt.grid()
    plt.subplots_adjust(wspace=0.2, 
                        hspace=0.4)
    plt.savefig(Png_name_DS[i])
    plt.savefig(Pdf_name_DS[i])
    #plt.show()
