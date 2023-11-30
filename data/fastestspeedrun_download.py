#!/usr/bin/python3

import re
import requests
from os import path
from bs4 import BeautifulSoup

url = 'https://speedyguy17.info/icpc/data/nwerc/2018/fastestspeedrun/data/secret/'

page = requests.get(url)
soup = BeautifulSoup(page.text, features="lxml")

for link_tag in soup.find_all('a', href=re.compile("0.*\.(in)|(ans)")):
    link = link_tag.get('href')
    file_name = f'fastestspeedrun/{link}'
    
    if path.isfile(file_name):
        print(f'\t{file_name} is already present; skipping')
        continue
        
    print(f'found test {link}, trying to download')
    f = requests.get(path.join(url, link))
    open(file_name, 'wb').write(f.content)

