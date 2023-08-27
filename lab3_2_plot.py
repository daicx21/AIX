import matplotlib.pyplot as plt
import numpy as np

f = open("lab3_2_1.txt", "r")
x,y = [],[]

for i in range(1,11):
    str = f.readline()
    x.append(i/10)
    for val in str.split():
        if "." in val:
            y.append(float(val))

f.close()

plt.semilogy(x, y, '-o', label='VC=1, Depth=1')

f = open("lab3_2_2.txt", "r")
x,y = [],[]

for i in range(1,11):
    str = f.readline()
    x.append(i/10)
    for val in str.split():
        if "." in val:
            y.append(float(val))

f.close()

plt.semilogy(x, y, '-o', label='VC=16, Depth=1')

f = open("lab3_2_3.txt", "r")
x,y = [],[]

for i in range(1,11):
    str = f.readline()
    x.append(i/10)
    for val in str.split():
        if "." in val:
            y.append(float(val))

f.close()

plt.semilogy(x, y, '-o', label='VC=1, Depth=16')

plt.legend()
plt.xlabel('Injection load (flits/node/cycle)')
plt.ylabel('Latency (cycles)')

plt.savefig('lab3_2.jpg')
plt.show()