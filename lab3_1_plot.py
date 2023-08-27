import matplotlib.pyplot as plt
import numpy as np

f = open("lab3_1_1.txt", "r")
x,y = [],[]

for i in range(1,11):
    str = f.readline()
    x.append(i/10)
    for val in str.split():
        if "." in val:
            y.append(float(val))

f.close()

plt.plot(x, y, '-o', label='Ring')

f = open("lab3_1_2.txt", "r")
x,y = [],[]

for i in range(1,11):
    str = f.readline()
    x.append(i/10)
    for val in str.split():
        if "." in val:
            y.append(float(val))

f.close()

plt.plot(x, y, '-o', label='MeshXY')

plt.legend()
plt.xlabel('Injection load (flits/node/cycle)')
plt.ylabel('Latency (cycles)')

plt.savefig('lab3_1.jpg')
plt.show()