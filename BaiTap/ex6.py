import os
import sys

def Encode_HEX(fileName):
    with open(fileName) as f, open('Encode_hex.txt', 'wb') as fout:
        for line in f:
            line = ''.join(line.split())
            fout.write(line.encode('hex'))
            fout.write("\n")

if __name__ == '__main__':
    Encode_HEX("ml-1m-README.txt")