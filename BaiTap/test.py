import os
import re
import sys

from pathlib import Path
files = list(Path(os.getcwd()).rglob("*.h"))
#print(ccc)

#get define memory map
mem = []
with open('input.txt', encoding="utf8") as fin:
    for line in fin:
        mem.append(line.rstrip())
print(mem[0])

lstmemory = set(mem)

with open("result.txt", "w") as output:
    for idx in lstmemory:
        ischeck = False
        for result in files:
            with open(result, encoding="utf8") as fin:
                lines = fin.read()
                if idx in lines:
                    ischeck = True
                    #output.write('DEFINE IN  : {0}\n'.format(result))
                #else:
                    #print('nono')
                    #output.write('NOT DEFINE : {0}\n'.format(result))
        # check addr
        if ischeck == True:
            print('xxx')
        else:
            output.write('Addr {0} Not Define\n'.format(idx))