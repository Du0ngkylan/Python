import msvcrt
import os

while True:
    if msvcrt.kbhit():
        key = msvcrt.getch()
        os.system('cls')
        print (key)