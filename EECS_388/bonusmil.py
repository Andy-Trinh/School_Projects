import csv
import serial
import time

ser1 = serial.Serial('/dev/ttyAMA1')
ser1.baudrate = 115200

l = []

with open('Data1.csv', mode='r') as csv_file:
    csv_reader = csv.reader(csv_file, delimiter=',')
    next(csv_reader)
    for row in csv_reader:
        l.append([row[1], row[2], row[3]])
        
with open('Data2.csv', mode='r') as csv_file2:
    csv2_reader = csv.reader(csv_file2, delimiter=',')
    next(csv2_reader)
    for row in csv2_reader:
        l.append([row[1], row[2], row[3]])
i=0
j=20
x=0
   
while x != 20:
    print(f"Batch {i}")
    print(f"{l[i][0]}, {l[i][1]}, {l[i][2]}, {l[j][0]}, {l[j][1]}, {l[j][2]}")
    ser1.write(bytes(f"{l[i][0]}, {l[i][1]}, {l[i][2]}, {l[j][0]}, {l[j][1]}, {l[j][2]}\n", encoding='utf8'))
    time.sleep(int(l[i][2]))
    i += 1
    j += 1
    x += 1
ser1.close()
print("Transfer complete!")