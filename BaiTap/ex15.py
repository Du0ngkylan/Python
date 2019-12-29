import os
import sys
import binascii

def Decode_HEX(fileName):
    with open(fileName) as f, open('decode_hex.txt', 'wb') as fout:
        for line in f:
            fout.write(binascii.unhexlify(''.join(line.split())))
            fout.write("\n")

if __name__ == '__main__':
    Decode_HEX("encode_hex.txt")