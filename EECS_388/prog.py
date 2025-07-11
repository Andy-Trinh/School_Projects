import csv
import serial
ser1 = serial.Serial('/dev/ttyAMA1')
ser1.baudrate = 115200
print(ser1.name + " at baudrate:" + ser1.baudrate)

with open('milestone2.csv', mode='r') as csv_file:
    csv_reader = csv.reader(csv_file, delimiter=',')
    line_count = 0
    for row in csv_reader:
        print(f'\t{row[0]} angle, {row[1]} speed, {row[2]} duration')
        command_str = str("angle: " + row[0] + ", speed: " + row[1] + ", duration: " + row[2])
        ser1.write(bytes(command_str))
        line_count += 1
ser1.close()