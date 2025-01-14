import serial
import serial.tools.list_ports

for port in serial.tools.list_ports.comports():
	print(f'# {port = } / {port.device = }')

# port = <serial.tools.list_ports_common.ListPortInfo object at 0x00000XXXXXXXXXXX> / port.device = 'COM1'
# port = <serial.tools.list_ports_common.ListPortInfo object at 0x00000YYYYYYYYYYY> / port.device = 'COM2'
