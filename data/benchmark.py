#!/usr/bin/python3

import os
import subprocess
import time
import sys
import pandas as pd

dirnames = ['konect', 'networkrepository']
algos = ['gabow']
algos = ['pq', 'treap', 'set', 'gabow']
cli = '../cmake-build-release/arbok-cli'

files = [
    'konect/wikipedia_link_zh_yue.soap',
    'konect/wikipedia_link_gu.soap',
    'konect/wiki_talk_nds.soap',
    'konect/wikipedia_link_th.soap',
    'konect/web-BerkStan.soap',
    'konect/wiki_talk_lv.soap',
    'konect/wiki_talk_ca.soap',
    'konect/epinions.soap',
    'konect/wikipedia_link_eml.soap',
]

os.makedirs('results', exist_ok=True)
logfile = open('results/log', 'w')

def quick_bench():
    resfile = f'results/quick.csv'
    with open(resfile, 'w') as csv:
        csv.write('file,n,m,weighted,opt,algo,con,run,rec,del,merge\n')
    for algo in algos:
        print(f'running {algo}', end=' ')
        st = time.time()
        for f in files:
            print('.',end='',flush=True)
            subprocess.call([cli, '-input', f, '-algo', algo, '-csv', resfile], stdout=logfile)
        en = time.time()
        print(f' {en-st:.4}s')
    df = pd.read_csv(resfile) 
    df['total'] = df.con + df.run + df.rec + df['del']
    print(df.groupby('algo').sum())


if len(sys.argv)==2:
    quick_bench()
    exit(0)

for dirname in dirnames:
    
    resfile = f'results/runtime-{dirname}.csv'
    with open(resfile, 'w') as csv:
        csv.write('file,n,m,weighted,opt,algo,con,run,rec,del,merge\n')

    for filename in os.listdir(dirname):
        if not filename.endswith('soap'):
            continue
        filename = os.path.join(dirname, filename)
        print(filename)
        with open(filename, 'r') as f:
            n,m = map(int,next(f).split())
            if n>1e6:
                print(f'\tn={n} too large, skip')
                continue
        
        for algo in algos:
            print(f'\trunning {algo}', end=' ')
            st = time.time()
            subprocess.call([cli, '-input', filename, '-csv', resfile, '-algo', algo], stdout=logfile)
            en = time.time()
            print(f'done in {en-st:.4}s')

