#!/usr/bin/python3

import glob
import os
import shutil

target_dir = 'networkrepository'

# function to convert .edges files
def convert_edges(in_name, out_name, weighted):
    n = 0
    m = 0
    with open(in_name, 'r') as in_f:
        for line in in_f:
            if line[0] == '%':
                continue
            line = line.replace(',',' ')
            parts = line.split()
            m += 1
            n = max(n, max(map(int,parts[:2])))
        
    temp_name = f'{target_dir}/tmp.txt'
    with open(in_name, 'r') as in_f:
        with open(temp_name, 'w') as tmp:
            tmp.write(f'{n} {m}\n')
            for line in in_f:
                if line[0] == '%':
                    continue
                line = line.replace(',',' ')
                parts = line.split()
                fr,to = map(int,parts[:2])
                if not weighted:
                    tmp.write(f'{fr-1} {to-1}\n')
                else:
                    tmp.write(f'{fr-1} {to-1} {parts[2]}\n')
        
    shutil.move(temp_name, out_name)
    print(f'\tconverted into {out_name}')


# function to convert .mtx files
def convert_mtx(in_name, out_name):
    temp_name = f'{target_dir}/tmp.txt'
    with open(in_name, 'r') as in_f:
        with open(temp_name, 'w') as tmp:
            in_f.readline() # skip first header comment
            header = in_f.readline()
            while header[0] == '%':
                header = in_f.readline() # some file have more comments
            n, n2, m = map(int,header.split())
            if n!=n2:
                print(f'\tgraph is bipartite; skipping')
                return 
            tmp.write(f'{n} {m}\n')
            for line in in_f:
                if line[0] == '%':
                    continue
                parts = line.split()
                fr,to = map(int,parts[:2])
                tmp.write(f'{fr-1} {to-1} {parts[2]}\n')
    
    shutil.move(temp_name, out_name)
    print(f'\tconverted into {out_name}')

    
# do work
for filename in glob.glob(f'{target_dir}/*'):
    if not filename.endswith(('.mtx', '.edges')):
        continue
    os.chmod(filename, 0o644)

    # read header
    with open(filename) as f:
        head = f.read(512)
   
    # filter bad ones out; there is really nothing useful otherwise
    if filename.endswith('.mtx') and any(p in head for p in ('symmetric', 'complex', 'pattern', 'real')): 
        continue
    if filename.endswith('.edges') and not head.startswith(('% asym', '% directed')): 
        continue 

    # check if already done
    print(f'found graph file to convert: {filename}')
    name = os.path.splitext(os.path.basename(filename))[0]
    if any(os.path.isfile(f'{target_dir}/{name}.{ext}') for ext in ('soap','wsoap')):
        print(f'\tsoap file found for {name}; skipping archive')
        continue
    
    if filename.endswith('.mtx'):
        convert_mtx(filename, f'{target_dir}/{name}.wsoap')
    else: # .edges
        weighted = ' weighted' in head or 'posweighted' in head
        convert_edges(filename, f'{target_dir}/{name}' + ('.soap','.wsoap')[weighted], weighted)


