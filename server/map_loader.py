import csv
import serial

SERIAL_PORT = 'COM6'

ser = serial.Serial(SERIAL_PORT, 115200, timeout=1)

index = 0
with open("node_map.csv", newline='') as nodeMap:
    nodeReader = csv.reader(nodeMap, delimiter=',')
    for row in nodeReader:
        