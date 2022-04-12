#!/usr/bin/python3

import os
import subprocess
import random

random.seed(1337)
girg_cli = "../../girgs/build/gengirg"
os.makedirs("girgs", exist_ok=True)
logfile = open('results/girgs_log', 'w')

n = 10**4
for deg in range(100,2001,100):
    for i in range(10):
        print(deg, i)
        clicall = [girg_cli, 
            '-n', str(n), 
            '-deg', str(deg), 
            '-edge', '1', 
            '-file', f'girgs/girg{deg:04d}_{i}',
            '-pseed', str(random.randint(0,10**6)), 
            '-aseed', str(random.randint(0,10**6)), 
            '-sseed', str(random.randint(0,10**6)), 
            ]
        subprocess.run(clicall, stdout=logfile)

