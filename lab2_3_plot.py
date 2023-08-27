import matplotlib.pyplot as plt
import numpy as np

f = open("lab2_3_1.txt", "r")
x,y = [],[]

for i in range(1,11):
    str = f.readline()
    x.append(i/10)
    for val in str.split():
        if "." in val:
            y.append(float(val))

f.close()

plt.semilogy(x, y, '-o', label='router latency=1')

f = open("lab2_3_2.txt", "r")
x,y = [],[]

for i in range(1,11):
    str = f.readline()
    x.append(i/10)
    for val in str.split():
        if "." in val:
            y.append(float(val))

f.close()

plt.semilogy(x, y, '-o', label='router latency=2')

f = open("lab2_3_3.txt", "r")
x,y = [],[]

for i in range(1,11):
    str = f.readline()
    x.append(i/10)
    for val in str.split():
        if "." in val:
            y.append(float(val))

f.close()

plt.semilogy(x, y, '-o', label='router latency=3')

plt.legend()
plt.xlabel('Injection load (flits/node/cycle)')
plt.ylabel('Latency (cycles)')

plt.savefig('lab2_3.jpg')
plt.show()