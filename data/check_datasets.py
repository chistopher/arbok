#!/usr/bin/python3

import os

for dirname in os.listdir('.'):
    if not os.path.isdir(dirname):
        continue

    print(dirname)

    for filename in os.listdir(dirname):
        if not filename.endswith('soap'):
            continue

        print(f'\tchecking {filename} ...', end=' ')
        
        fullname = os.path.join(dirname, filename)
        f = open(fullname, 'r')
        weighted = filename.endswith('wsoap')
        n, m = map(int,next(f).split())

        edges = set()
        mult = 0
        self = 0
        inva = 0

        for i in range(m):
            line = next(f).split()
            u,v = map(int,line[:2])
            if (u,v) in edges:
                # print(f'{i}-th edge is multiedge: {u} {v}')
                mult += 1
            edges.add((u,v))
            if u==v:
                # print(f'{i}-th edge is self-loop: {u} {v}')
                self += 1
            if not 0<=u<=n-1:
                print(f'{i}-th edge invalid u: {u}')
            if not 0<=v<=n-1:
                print(f'{i}-th edge invalid v: {v}')
            
            if not weighted:
                continue
            w = line[2]
            weight_ok = all('0'<=c<='9' or c=='-' for c in w)
            if not weight_ok:
                # print(f'weight of {i}-th edge invalild: {w}')
                inva += 1
            elif int(w)*n >= 1e9:
                # print(f'weight of {i}-th edge is too large: {w}')
                # inva += 1
                pass
        f.close()

        verdict = ''
        if mult:
            verdict += f'{mult} multiedges '
        if self:    
            verdict += f'{self} selfloops '
        if inva:
            verdict += f'{inva} invalid weights '
        
        print('ok' if not verdict else verdict)

