import serial
from ctypes import *
from dataclasses import dataclass
from crc import *
import crcmod

#header
cmd = bytearray('AURA'.encode())
cmd.extend(bytearray(4))
cmd.extend(bytearray(4))
cmd.extend(bytearray(4))
# chunk header
cmd.extend(bytearray(1))
cmd.extend(bytearray(1))
cmd.extend(bytearray(2))
print(cmd)

crc16 = crcmod.mkCrcFun(0x18005, rev=True, initCrc=0xFFFF, xorOut=0x0000)
crc = crc16(cmd)
print(hex(crc))

ser = serial.Serial()
ser.baudrate= 115200
ser.port = 'COM11'
ser.open()

ser.write(cmd)
ser.write(crc.to_bytes(2,'big'))

response = ser.read(36)
print(response)