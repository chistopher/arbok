
from __future__ import print_function

import argparse
import os
import subprocess
import sys
import pathlib
import pprint as pp
import time
#from prettytable import PrettyTable

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

parser = argparse.ArgumentParser()
parser.add_argument('executable', help='the executable we want to test')
parser.add_argument('--datadir', required=False, default='fastestspeedrun', help='specify the directory containing the test files', nargs=1)
parser.add_argument('--testcase', required=False, help="specify a single testcase to be done", nargs=1)
args = parser.parse_args()

# wget -r --no-parent https://speedyguy17.info/icpc/data/nwerc/2018/fastestspeedrun/data/secret/
pp.pprint(args)
datadir = pathlib.Path(args.datadir[0])
for filename in os.listdir(datadir):
    if filename.endswith(".in"):
        scenario_name = filename[:-3]
        infile = datadir / filename
        answerfile = datadir / f"{scenario_name}.ans"
        
        if args.testcase != None and scenario_name != args.testcase[0]:
            continue
        
        if not infile.exists():
            eprint(f"Skipping scenario { scenario_name } due to missing result file.")
            continue
        
        f = open(str(infile), "r")
        scenario = f.read()
        f.close()
        f = open(str(answerfile), "r")
        correct_answer = f.read().strip()
        f.close()

        cmd = [args.executable]
        ip = scenario.encode('utf-8')
        time_start = time.perf_counter_ns()
        result = subprocess.run(cmd, stdout=subprocess.PIPE, input=ip)
        time_end = time.perf_counter_ns()
        execution_time_ms = (time_end - time_start) // 1000000

        output = result.stdout.decode('utf-8').strip()

        if output != correct_answer:
            eprint(f'Output for scenario { scenario_name }: {output}, but correct result is {correct_answer}')
        else:
            execution_time_string = f"        {execution_time_ms}"[-8:]
            emoji = "🍺" if execution_time_ms < 5000 else "🤦‍♂️"
            print(f'{execution_time_string} ms {emoji} for scenario { scenario_name }')
            
