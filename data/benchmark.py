#!/usr/bin/python3

import os
import subprocess
import time
import sys
import pandas as pd

dirnames = ['fastestspeedrun', 'konect', 'networkrepository', 'yosupo', 'antilemon']
#dirnames = ['girgs']
algos = ['pq', 'treap', 'hollow', 'set', 'gabow', 'lemon', 'atofigh', 'felerius', 'spaghetti', 'yosupo', 'matrix']
#algos = ['atofigh']
cli = '../build/arbok-cli'


os.makedirs('results', exist_ok=True)
logfile = open('results/log', 'w')

for dirname in dirnames:
    
    resfile = f'results/runtime-{dirname}.csv'
    with open(resfile, 'w') as csv:
        csv.write('file,n,m,weighted,opt,algo,con,run,rec,del\n')

    for filename in os.listdir(dirname):
        if not filename.endswith('soap'):
            continue
        filename = os.path.join(dirname, filename)
        print(filename)
        with open(filename, 'r') as f:
            n,m = map(int,next(f).split())
            #if n>1e6:
                #print(f'\tn={n} too large, skip')
                #continue

        for algo in algos:
            print(f'\trunning {algo}', end=' ')
            if algo=='matrix' and n>=100000:
                print('too large for matrix')
                continue

            clicall = [cli, '-input', filename, '-csv', resfile, '-algo', algo, '-check', '0']
            if dirname in ['fastestspeedrun', 'yosupo']:
                clicall += ['-root', '0']
            else: 
                clicall += ['-giantCC', '1']
            
            st = time.time()
            try:
                res = subprocess.run(clicall, stdout=logfile, timeout=60*30)
            except subprocess.TimeoutExpired:
                print('timed out')
                continue
            en = time.time()
            if res.returncode == 0:
                print(f'done in {en-st:.4}s', flush=True)
            else:
                print(f'WARNING: non-zero exit code ({res.returncode})', flush=True)

