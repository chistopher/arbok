#!/usr/bin/python3

import os

os.makedirs("antilemon")

for n_exp in range(2,7):
    n = 10**n_exp
    edges = []
    edges += [(0,i,0) for i in range(1,n//2)]
    edges += [(i,0,0) for i in range(1,n//2)]
    edges += [(i,0,1) for i in range(n//2,n)]
    
    with open(f'antilemon/anti{n}.wsoap', 'w') as ouf:
        ouf.write(f'{n} {len(edges)}\n')
        for u,v,w in edges:
            ouf.write(f'{u} {v} {w}\n')
    print(f'generated anti_{n}')

