import matplotlib.pyplot as plt
import numpy as np

f = open("lab2_4_1.txt", "r")
x,y = [],[]

for i in range(1,11):
    str = f.readline()
    x.append(i/10)
    for val in str.split():
        if "." in val:
            y.append(float(val))

f.close()

plt.semilogy(x, y, '-o', label='link width bits=16')

f = open("lab2_4_2.txt", "r")
x,y = [],[]

for i in range(1,11):
    str = f.readline()
    x.append(i/10)
    for val in str.split():
        if "." in val:
            y.append(float(val))

f.close()

plt.semilogy(x, y, '-o', label='link width bits=32')

f = open("lab2_4_3.txt", "r")
x,y = [],[]

for i in range(1,11):
    str = f.readline()
    x.append(i/10)
    for val in str.split():
        if "." in val:
            y.append(float(val))

f.close()

plt.semilogy(x, y, '-o', label='link width bits=64')

f = open("lab2_4_4.txt", "r")
x,y = [],[]

for i in range(1,11):
    str = f.readline()
    x.append(i/10)
    for val in str.split():
        if "." in val:
            y.append(float(val))

f.close()

plt.semilogy(x, y, '-o', label='link width bits=128')

plt.legend()
plt.xlabel('Injection load (flits/node/cycle)')
plt.ylabel('Latency (cycles)')

plt.savefig('lab2_4.jpg')
plt.show()