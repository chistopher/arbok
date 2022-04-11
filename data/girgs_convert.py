#!/usr/bin/python3

import glob
import random

for graph in glob.glob(f'girgs/*.txt'):
    print(graph)
    inp = open(graph,'r')
    oup = open(graph[:-4] + '.soap', 'w')
    n,m = map(int,inp.readline().split())
    oup.write(f'{n} {m}\n')
    inp.readline()
    for l in inp:
        a,b = l.split()
        if random.getrandbits(1):
            a,b = b,a
        oup.write(f'{a} {b}\n')
    inp.close()
    oup.close()

