# import sys
# import csv

# reader = csv.reader(open("files.csv"), delimiter=";")


# import csv

# reader = csv.DictReader(open('nzrainfall.csv', 'r'))
# result = sorted(reader, key=lambda d: float(d['Auckland']))

# writer = csv.DictWriter(open('output.csv', 'w'), reader.fieldnames)
# writer.writeheader()
# writer.writerows(result)
def line_to_tuple(line):
    data = line.split()
    return (data[0],int(data[1]),int(data[2]),int(data[3]),int(data[4]),int(data[5]))

with open('nzrainfall.csv') as fin, open('output.csv','w') as fout:
    non_blank_lines = (line for line in fin if line.strip())
    sorted_lines = sorted(non_blank_lines,key=line_to_tuple)
    fout.writelines(sorted_lines)
