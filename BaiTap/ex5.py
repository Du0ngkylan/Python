import sys
import os

def Read_File(filename, DataLines):
    with open(filename) as fileObj:
        for line in fileObj:
            try:
                Data = line.split()
                DataLines += [int(item) for item in Data]
            except:
                return False
                 
    return True

def WriteFile(DataLines):
    f = open("T10I4D100K.txt", "w")
    
    Data = sorted(DataLines)
    for i in range(0,len(Data),16):
        f.write(" ".join([str(i) for i in Data[i:i+16]]) + "\n")

    f.close()

def main(INPUT_FILE = ''):
    
    if(INPUT_FILE == ''):
        print "Error: No Input File."
        return None
    if not os.path.exists(INPUT_FILE): # Check if path exits
        print "File does not exist"
        return None

    DataLines = []

    bResult = Read_File(INPUT_FILE, DataLines)
    if not bResult:
        print "Read File Fail"
        return None

    WriteFile(DataLines)

if __name__ == '__main__':
    main("T10I4D100K.dat")