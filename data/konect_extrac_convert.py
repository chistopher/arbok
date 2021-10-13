#!/usr/bin/python3

import tarfile
import glob
import os
import shutil
from tempfile import TemporaryDirectory

konect_dir = os.path.abspath('./konect/')

def is_good_weight(w):
    w = str(w,'utf-8')
    return all('0'<=c<='9' for c in w[w[0]=='-':])

def parse_and_convert(konect_file, out_name):
    n = 0
    m = 0
    weighted = False
    with open('temp.txt', 'w') as outf:
        # handle header
        header = konect_file.readline()
        header_parts = header.split()
        print(f'\theader: {header}')
        assert(header_parts[0]==b'%') 
        assert(header_parts[1]==b'asym') 

        #if header_parts[2] in (b'posweighted', b'positive', b'weighted'):
        if header_parts[2] in (b'posweighted', b'weighted'):
            weighted = True
        
        #gather n,m
        for line in konect_file:
            if line[0] == ord('%'):
                continue
            parts = line.split()
            if weighted and not is_good_weight(parts[2]):
                print(f'\tweight {str(parts[2],"utf-8")} invalid; treating graph as unweighted')
                weighted = False
            m += parts[0]!=parts[1] # do not count self-loops
            n = max(n, max(map(int,parts[:2])))
        
        
        # write result
        konect_file.seek(0)
        outf.write(f'{n} {m}\n')
        for line in konect_file:
            if line[0] == ord('%'):
                continue
            parts = line.split()
            fr,to = map(int,parts[:2])
            if fr==to:
                continue
            if not weighted:
                outf.write(f'{str(fr-1)} {str(to-1)}\n')
            else:
                outf.write(f'{str(fr-1)} {str(to-1)} {str(parts[2], "utf-8")}\n')


    with_ext = out_name + ('.soap','.wsoap')[weighted]
    shutil.move('temp.txt', with_ext)
    print(f'\tsuccessfully wrote {os.path.basename(with_ext)}')
    

with TemporaryDirectory() as tmp_dir:
    os.chdir(tmp_dir)
    for archive in glob.glob(f'{konect_dir}/*.tar.bz2'):
        name = os.path.basename(archive)[13:-8] # trim download.tsv.{name}.tar.bz2
        print(f'processing {name} : {archive}')

        if any(os.path.isfile(f'{konect_dir}/{name}.{ext}') for ext in ('soap','wsoap')):
            print(f'\tsoap file found for {name}; skipping archive')
            continue

        with tarfile.open(archive, 'r:bz2') as tar:
            graph_file = next((k for k in tar if k.name.startswith(f'{name}/out.')), None)
            if not graph_file:
                print('\tno graph found in archive; skipping')
                continue

            print(f'\tfound graph file in archive {graph_file.name}')
            f = tar.extractfile(graph_file)
            parse_and_convert(f, os.path.join(konect_dir,name))

