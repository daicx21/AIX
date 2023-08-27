import os

f = open("lab3_1_1.txt", "w")

for i in range(1, 11):
    s1 = "./build/NULL/gem5.opt \
configs/example/garnet_synth_traffic.py \
--network=garnet --num-cpus=16 --num-dirs=16 \
--topology=Ring --routing-algorithm=3 \
--inj-vnet=0 --synthetic=uniform_random \
--sim-cycles=10000 --injectionrate=" + str(i/10)
    print(s1)
    os.system(s1)
    s2 = 'grep "average_packet_latency" m5out/stats.txt | sed "s/system.ruby.network.average_packet_latency\s*/average_packet_latency = /"'
    cur = os.popen(s2)
    f.write(cur.read())

f.close()

f = open("lab3_1_2.txt", "w")

for i in range(1, 11):
    s1 = "./build/NULL/gem5.opt \
configs/example/garnet_synth_traffic.py \
--network=garnet --num-cpus=16 --num-dirs=16 \
--topology=Mesh_XY --mesh-rows=4 \
--inj-vnet=0 --synthetic=uniform_random \
--sim-cycles=10000 --injectionrate=" + str(i/10)
    print(s1)
    os.system(s1)
    s2 = 'grep "average_packet_latency" m5out/stats.txt | sed "s/system.ruby.network.average_packet_latency\s*/average_packet_latency = /"'
    cur = os.popen(s2)
    f.write(cur.read())

f.close()