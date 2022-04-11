#!/usr/bin/python3

import os
import subprocess
import time
import sys
import pandas as pd

algos = ['pq', 'treap', 'hollow', 'set', 'gabow', 'lemon', 'atofigh', 'felerius', 'spaghetti', 'yosupo']
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
cli = '../build/arbok-cli'


os.makedirs('results', exist_ok=True)
logfile = open('results/log', 'w')

# just benchmark on a small set of files to get fast feedback when optimizing some algo
resfile = f'results/quick.csv'
with open(resfile, 'w') as csv:
    csv.write('file,n,m,weighted,opt,algo,con,run,rec,del\n')
for algo in algos:
    print(f'running {algo}', end=' ')
    st = time.time()
    for f in files:
        print('.',end='',flush=True)
        subprocess.call([cli, '-input', f, '-algo', algo, '-csv', resfile, '-check', '0'], stdout=logfile)
    en = time.time()
    print(f' {en-st:.4}s')
df = pd.read_csv(resfile) 
df['total'] = df.con + df.run + df.rec + df['del']
print(df.groupby('algo').sum())

