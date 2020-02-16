import os
import sys
import hashlib

BLOCKSIZE = 65536
hasher = hashlib.md5()
with open('ml-1m-README.txt', 'rb') as pFile:
    buf = pFile.read(BLOCKSIZE)
    while len(buf) > 0:
        hasher.update(buf)
        buf = pFile.read(BLOCKSIZE)
print(hasher.hexdigest())
