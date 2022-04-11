#!/usr/bin/python3

import os
import subprocess

girg_cli = "../../girgs/build/gengirg"
os.makedirs("girgs", exist_ok=True)
logfile = open('results/girgs_log', 'w')

n = 10**5
for deg in range(50,1001,50):
    print(deg)
    clicall = [girg_cli, '-n', str(n), '-deg', str(deg), '-edge', '1', '-file', f'girgs/girg{deg:05d}']
    subprocess.run(clicall, stdout=logfile)

