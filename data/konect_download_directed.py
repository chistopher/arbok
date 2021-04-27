#!/usr/bin/python3

import re
import requests
from os import path
from bs4 import BeautifulSoup

url = 'http://konect.cc/networks/'

page = requests.get(url)
soup = BeautifulSoup(page.text, features="lxml")

for link_tag in soup.find_all('a'):
    link = link_tag.get('href')
    if link.startswith(('/', 'http', 'mailto')):
        continue
    if not link_tag.parent.parent.find('img', title='Unipartite, directed'):
        continue
    print(f'network {link[:-1]} seems to be directed, tying to download')

    subpage = requests.get(url + link)
    subsoup = BeautifulSoup(subpage.text, features="lxml")
    download_tag = subsoup.find('a', href=re.compile('.*bz2'))
    if not download_tag:
        print(f'\tno download link found on {url+link}; skipping ...')
        continue

    link2 = download_tag.get('href')
    name = path.basename(link2)
    download_url = path.join(url, link, link2)
    file_name = f'konect/{name}'

    if path.isfile(file_name):
        print(f'\t{file_name} is already present; skipping')
        continue

    print(f'\tdownloading from {download_url}')
    bz2 = requests.get(url+download_tag.get('href'))
    open(file_name, 'wb').write(bz2.content)

