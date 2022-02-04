import matplotlib.pyplot as plt
import seaborn as sns
sns.set_theme()

# rlzCompressedSize = [313334930, 312600501, 312439803, 312367375, 312335915, 312324174, 312311357, 312304335, 312290149, 312285861, 312282689]
# rlzTime = [0.000724, 0.006790, 0.012810, 0.018691, 0.024614, 0.030549, 0.036578, 0.049006, 0.055119, 0.063100, 0.069792]

# plt.figure()
# plt.plot(rlzCompressedSize, rlzTime, label="RLZ (plain chunking)")
# plt.title("Compressed size v/s Random Access time")
# plt.xlabel("Compressed file size (in B)")
# plt.ylabel("Avg. random access time for a chunk (in s)")
# plt.legend()
# plt.savefig("../figs/random_access_times_rlz.png")
# plt.savefig("../figs/random_access_times_rlz.svg")
# plt.show()

# dsCompressedSize = [420304666, 411262217, 410374438, 407144034 , 403980748 , 403230398, 402382745, 408486728, 411246811,  404671821, 405816080 ]
# dsTime = [0.053237, 0.504910, 0.941064, 1.329871, 1.707224, 2.102259, 2.455775, 2.903300, 3.183511, 3.584570, 3.947258]
# plt.figure()
# plt.plot(dsCompressedSize, dsTime, label="Tatwawadi-based scheme", color='g')
# plt.title("Compressed size v/s Random Access time")
# plt.xlabel("Compressed file size (in B)")
# plt.ylabel("Avg. random access time for a chunk (in s)")
# plt.legend()
# plt.savefig("../figs/random_access_times_ds.png")
# plt.savefig("../figs/random_access_times_ds.svg")
# plt.show()

chunkSize = [536870, 5851892, 11166914, 16481936, 21796958, 27111980, 32427002, 37742024, 43057046, 48372068, 53687090]
rlzSize  = [313334930, 312600501, 312439803, 312367375, 312335915, 312324174, 312311357, 312304335, 312290149, 312285861, 312282689]
dsSize = [420304666, 411262217, 410374438, 407144034 , 403980748 , 403230398, 402382745, 408486728, 411246811, 404671821, 405816080]
plt.figure()
plt.plot(chunkSize, rlzSize, label="Tatwawadi-based scheme", color='g')
plt.plot(chunkSize, dsSize, label="RLZ (plain chunking)", color='r')
plt.title("Compressed size v/s Chunk size")
plt.xlabel("Chunk size (in B)")
plt.ylabel("Compressed file size (in B)")
plt.legend()
plt.savefig("../figs/sizes.png")
plt.savefig("../figs/sizes.svg")
plt.show()