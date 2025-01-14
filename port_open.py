import serial
import serial.tools.list_ports

# 'COM2' 9600bps Parityなしの場合
Serial_Port = serial.Serial(port="/dev/cu.usbserial-140", baudrate=115200)
