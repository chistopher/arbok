#!/usr/bin/python3

import glob
import os

spdrun_dir = os.path.abspath('./fastestspeedrun/')

for testcasefile in glob.glob(f'{spdrun_dir}/*.in'):
    name = os.path.basename(testcasefile)
    print(f'processing {name}')

    res_file = f'{spdrun_dir}/{name[:-3]}.wsoap'
    
    inf = open(testcasefile, 'r')
    ouf = open(res_file, 'w')

    n = int(inf.readline())
    ouf.write(f'{n+1} {n*(n+2)}\n')
    for level in range(1,n+1):
        vals = list(map(int,inf.readline().split()))
        assert(len(vals) == n+3)
        ouf.write(f'{vals[0]} {level} {vals[1]}\n') 
        for item in range(0,n+1):
            ouf.write(f'{item} {level} {vals[2+item]}\n')

    inf.close()
    ouf.close()


