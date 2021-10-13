
import os
import subprocess
import time

dirnames = ['konect', 'networkrepository']
algos = ['pq', 'treap', 'set']
cli = '../cmake-build-release/arbok-cli'

os.makedirs('results', exist_ok=True)
logfile = open('results/log', 'w')

for dirname in dirnames:
    
    resfile = f'results/runtime-{dirname}.csv'
    with open(resfile, 'w') as csv:
        csv.write('file,n,m,weighted,algo,time\n')

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

