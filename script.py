import os
import numpy as np
import matplotlib.pyplot as plt
import subprocess
import re

def get_str(inj_rate, routing_algorithm, adaptive_algorithm, traffic):
    return "./build/NULL/gem5.opt \
        configs/example/garnet_synth_traffic.py \
        --network=garnet --num-cpus=64 --num-dirs=64 \
        --topology=Cube --dimension=3 --routing-algorithm={} --adaptive-algorithm={}  \
        --inj-vnet=0 --synthetic={} \
        --sim-cycles=10000 --injectionrate={} \
        --vcs-per-vnet=3 ".format(routing_algorithm,adaptive_algorithm,traffic,inj_rate)
       
        
get_number_float = r'\d+\.\d+'
get_number_int = r'\d+'
 
def get_latency():
    res = subprocess.getoutput('''grep "average_packet_latency" m5out/stats.txt | sed 's/system.ruby.network.average_packet_latency\s*/average_packet_latency = /' ''')
    # print(res)
    res_num = float(re.findall(get_number_float, res)[0])
    # print(res_num)
    return res_num


def get_hops():
    res = subprocess.getoutput('''grep "average_hops" m5out/stats.txt | sed 's/system.ruby.network.average_hops\s*/average_hops = /' ''')
    # print(res)
    get_number = r'(()|(\d+))'
    res_num = float(re.findall(get_number_float, res)[0])
    # print(res_num)
    return res_num

def get_queue_latency():
    res = subprocess.getoutput('''grep "average_packet_queueing_latency" m5out/stats.txt | sed 's/system.ruby.network.average_packet_queueing_latency\s*/average_packet_queueing_latency = /' ''')
    # print(res)
    res_list = re.findall(get_number_float, res)
    if len(res_list) == 0:
        res_list = re.findall(get_number_int, res)
    res_num = float(res_list[0])
    # print(res_num)
    return res_num

def get_network_latency():
    res = subprocess.getoutput('''grep "average_packet_network_latency" m5out/stats.txt | sed 's/system.ruby.network.average_packet_network_latency\s*/average_packet_network_latency = /' ''')
    # print(res)
    res_list = re.findall(get_number_float, res)
    if len(res_list) == 0:
        res_list = re.findall(get_number_int, res)
    res_num = float(res_list[0])
    # print(res_num)
    return res_num

N = 4
M = 50
_names = ["DOR", "BOE Random", "BOE Local", "BOE RCA1D"]
_routing_algorithm = ["0", "5", "5", "5"]
_adaptive_algorithm = ["0", "0", "1", "2"]

inj_rate_list = np.zeros(M)
avg_latency = np.zeros((N,M))

for k in range(0, M):
    x = (0.98/(M-1)) * k + 0.02
    inj_rate_list[k] = x
    for i in range(0, N):
        sys_str = get_str(inj_rate=x, routing_algorithm=_routing_algorithm[i], adaptive_algorithm=_adaptive_algorithm[i], traffic="uniform_random")
        os.system(sys_str)
        avg_latency[i][k] = get_latency()

plt.figure("uniform_random_BOE")
for i in range(0, N):
    plt.plot(inj_rate_list[:], avg_latency[i][:], '-o', markersize=4, linewidth=1, label = "{}".format(_names[i]))
        
plt.xlabel("Injection Rate")
plt.ylabel("Latency")
plt.ylim(0,100)
plt.legend()
plt.savefig('uniform_random_BOE.png')
plt.show()

inj_rate_list = np.zeros(M)
avg_latency = np.zeros((N,M))

for k in range(0, M):
    x = (0.98/(M-1)) * k + 0.02
    inj_rate_list[k] = x
    for i in range(0, N):
        sys_str = get_str(inj_rate=x, routing_algorithm=_routing_algorithm[i], adaptive_algorithm=_adaptive_algorithm[i], traffic="bit_reverse")
        os.system(sys_str)
        avg_latency[i][k] = get_latency()

plt.close()
plt.figure("bit_reverse_BOE")
for i in range(0, N):
    plt.plot(inj_rate_list[:], avg_latency[i][:], '-o', markersize=4, linewidth=1, label = "{}".format(_names[i]))
        
plt.xlabel("Injection Rate")
plt.ylabel("Latency")
plt.ylim(0,100)
plt.legend()
plt.savefig('bit_reverse_BOE.png')
plt.show()

for k in range(0, M):
    x = (0.98/(M-1)) * k + 0.02
    inj_rate_list[k] = x
    for i in range(0, N):
        sys_str = get_str(inj_rate=x, routing_algorithm=_routing_algorithm[i], adaptive_algorithm=_adaptive_algorithm[i], traffic="bit_complement")
        os.system(sys_str)
        avg_latency[i][k] = get_latency()

plt.close()
plt.figure("bit_complement_BOE")
for i in range(0, N):
    plt.plot(inj_rate_list[:], avg_latency[i][:], '-o', markersize=4, linewidth=1, label = "{}".format(_names[i]))

plt.xlabel("Injection Rate")
plt.ylabel("Latency")
plt.ylim(0,100)
plt.legend()
plt.savefig('bit_complement_BOE.png')
plt.show()

for k in range(0, M):
    x = (0.98/(M-1)) * k + 0.02
    inj_rate_list[k] = x
    for i in range(0, N):
        sys_str = get_str(inj_rate=x, routing_algorithm=_routing_algorithm[i], adaptive_algorithm=_adaptive_algorithm[i], traffic="shuffle")
        os.system(sys_str)
        avg_latency[i][k] = get_latency()

plt.close()
plt.figure("shuffle_BOE")
for i in range(0, N):
    plt.plot(inj_rate_list[:], avg_latency[i][:], '-o', markersize=4, linewidth=1, label = "{}".format(_names[i]))
        
plt.xlabel("Injection Rate")
plt.ylabel("Latency")
plt.ylim(0,100)
plt.legend()
plt.savefig('shuffle_BOE.png')
plt.show()


N = 4
M = 50
_names = ["DOR", "EVC Random", "EVC Local", "EVC RCA1D"]
_routing_algorithm = ["0", "6", "6", "6"]
_adaptive_algorithm = ["0", "0", "1", "2"]

inj_rate_list = np.zeros(M)
avg_latency = np.zeros((N,M))

for k in range(0, M):
    x = (0.98/(M-1)) * k + 0.02
    inj_rate_list[k] = x
    for i in range(0, N):
        sys_str = get_str(inj_rate=x, routing_algorithm=_routing_algorithm[i], adaptive_algorithm=_adaptive_algorithm[i], traffic="uniform_random")
        os.system(sys_str)
        avg_latency[i][k] = get_latency()

plt.close()
plt.figure("uniform_random_EVC")
for i in range(0, N):
    plt.plot(inj_rate_list[:], avg_latency[i][:], '-o', markersize=4, linewidth=1, label = "{}".format(_names[i]))
        
plt.xlabel("Injection Rate")
plt.ylabel("Latency")
plt.ylim(0,100)
plt.legend()
plt.savefig('uniform_random_EVC.png')
plt.show()

inj_rate_list = np.zeros(M)
avg_latency = np.zeros((N,M))

for k in range(0, M):
    x = (0.98/(M-1)) * k + 0.02
    inj_rate_list[k] = x
    for i in range(0, N):
        sys_str = get_str(inj_rate=x, routing_algorithm=_routing_algorithm[i], adaptive_algorithm=_adaptive_algorithm[i], traffic="bit_reverse")
        os.system(sys_str)
        avg_latency[i][k] = get_latency()

plt.close()
plt.figure("bit_reverse_EVC")
for i in range(0, N):
    plt.plot(inj_rate_list[:], avg_latency[i][:], '-o', markersize=4, linewidth=1, label = "{}".format(_names[i]))
        
plt.xlabel("Injection Rate")
plt.ylabel("Latency")
plt.ylim(0,100)
plt.legend()
plt.savefig('bit_reverse_EVC.png')
plt.show()

for k in range(0, M):
    x = (0.98/(M-1)) * k + 0.02
    inj_rate_list[k] = x
    for i in range(0, N):
        sys_str = get_str(inj_rate=x, routing_algorithm=_routing_algorithm[i], adaptive_algorithm=_adaptive_algorithm[i], traffic="bit_complement")
        os.system(sys_str)
        avg_latency[i][k] = get_latency()

plt.close()
plt.figure("bit_complement_EVC")
for i in range(0, N):
    plt.plot(inj_rate_list[:], avg_latency[i][:], '-o', markersize=4, linewidth=1, label = "{}".format(_names[i]))
        
plt.xlabel("Injection Rate")
plt.ylabel("Latency")
plt.ylim(0,100)
plt.legend()
plt.savefig('bit_complement_EVC.png')
plt.show()

for k in range(0, M):
    x = (0.98/(M-1)) * k + 0.02
    inj_rate_list[k] = x
    for i in range(0, N):
        sys_str = get_str(inj_rate=x, routing_algorithm=_routing_algorithm[i], adaptive_algorithm=_adaptive_algorithm[i], traffic="shuffle")
        os.system(sys_str)
        avg_latency[i][k] = get_latency()

plt.close()
plt.figure("shuffle_EVC")
for i in range(0, N):
    plt.plot(inj_rate_list[:], avg_latency[i][:], '-o', markersize=4, linewidth=1, label = "{}".format(_names[i]))
        
plt.xlabel("Injection Rate")
plt.ylabel("Latency")
plt.ylim(0,100)
plt.legend()
plt.savefig('shuffle_EVC.png')
plt.show()


N = 5
M = 50

_names = ["DOR 2D", "DOR 3D", "PAR", "BOE", "EVC"]
_routing_algorithm = ["0", "0", "4", "5", "6"]
_adaptive_algorithm = ["0", "0", "2", "2", "2"]
_dim = [2, 3, 3, 3, 3]

inj_rate_list = np.zeros(M)
avg_latency = np.zeros((N,M))

for k in range(0, M):
    x = (0.98/(M-1)) * k + 0.02
    inj_rate_list[k] = x
    for i in range(0, N):
        sys_str = get_str(inj_rate=x, routing_algorithm=_routing_algorithm[i], adaptive_algorithm=_adaptive_algorithm[i], traffic="uniform_random", dim=_dim[i])
        os.system(sys_str)
        avg_latency[i][k] = get_latency()

plt.figure("uniform_random_all")
for i in range(0, N):
    plt.plot(inj_rate_list[:], avg_latency[i][:], '-o', markersize=4, linewidth=1, label = "{}".format(_names[i]))
        
plt.xlabel("Injection Rate")
plt.ylabel("Latency")
plt.ylim(0,100)
plt.legend()
plt.savefig('uniform_random_all.png')
plt.show()

inj_rate_list = np.zeros(M)
avg_latency = np.zeros((N,M))

for k in range(0, M):
    x = (0.98/(M-1)) * k + 0.02
    inj_rate_list[k] = x
    for i in range(0, N):
        sys_str = get_str(inj_rate=x, routing_algorithm=_routing_algorithm[i], adaptive_algorithm=_adaptive_algorithm[i], traffic="bit_reverse", dim=_dim[i])
        os.system(sys_str)
        avg_latency[i][k] = get_latency()

plt.close()
plt.figure("bit_reverse_all")
for i in range(0, N):
    plt.plot(inj_rate_list[:], avg_latency[i][:], '-o', markersize=4, linewidth=1, label = "{}".format(_names[i]))
        
plt.xlabel("Injection Rate")
plt.ylabel("Latency")
plt.ylim(0,100)
plt.legend()
plt.savefig('bit_reverse_all.png')
plt.show()

for k in range(0, M):
    x = (0.98/(M-1)) * k + 0.02
    inj_rate_list[k] = x
    for i in range(0, N):
        sys_str = get_str(inj_rate=x, routing_algorithm=_routing_algorithm[i], adaptive_algorithm=_adaptive_algorithm[i], traffic="bit_complement", dim=_dim[i])
        os.system(sys_str)
        avg_latency[i][k] = get_latency()

plt.close()
plt.figure("bit_complement_all")
for i in range(0, N):
    plt.plot(inj_rate_list[:], avg_latency[i][:], '-o', markersize=4, linewidth=1, label = "{}".format(_names[i]))
        
plt.xlabel("Injection Rate")
plt.ylabel("Latency")
plt.ylim(0,100)
plt.legend()
plt.savefig('bit_complement_all.png')
plt.show()

for k in range(0, M):
    x = (0.98/(M-1)) * k + 0.02
    inj_rate_list[k] = x
    for i in range(0, N):
        sys_str = get_str(inj_rate=x, routing_algorithm=_routing_algorithm[i], adaptive_algorithm=_adaptive_algorithm[i], traffic="shuffle", dim=_dim[i])
        os.system(sys_str)
        avg_latency[i][k] = get_latency()

plt.close()
plt.figure("shuffle_all")
for i in range(0, N):
    plt.plot(inj_rate_list[:], avg_latency[i][:], '-o', markersize=4, linewidth=1, label = "{}".format(_names[i]))
        
plt.xlabel("Injection Rate")
plt.ylabel("Latency")
plt.ylim(0,100)
plt.legend()
plt.savefig('shuffle_all.png')
plt.show()