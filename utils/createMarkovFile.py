import numpy as np

path = '../data/markov'
data = ''
p0 = 0.5
p1 = 1 - p0
p = 0.99
transitionMatrix = [[p, 1 - p], [1 - p, p]]

data += str(np.random.binomial(n=1, p=p1))
fileSize = 1e9
arr = [0, 1]

for i in range(int(fileSize - 1)):
    prev = data[-1]
    data += str(np.random.choice(a=arr, replace=True, p=transitionMatrix[int(prev)]))

with open(path, 'w') as f:
    f.write(data)

print(f"File {path} of size {int(fileSize)} B created successfully.")