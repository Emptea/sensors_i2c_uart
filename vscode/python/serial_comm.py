import serial
import crcmod

#header
cmd = bytearray('AURA'.encode())
cmd.extend(bytearray([3]))
cmd.extend(bytearray(3))
cmd.extend(bytearray(4))
cmd.extend(bytearray(4))
cmd.extend(bytearray([1])) #1 - whoami, 3 - data
cmd.extend(bytearray(1))
cmd.extend(bytearray(2))
# chunk header
#cmd.extend(bytearray([1])) #for data
#cmd.extend(bytearray(1)) #for whoami
#cmd.extend(bytearray(1))
#cmd.extend(bytearray(2))
print(cmd)

crc16 = crcmod.mkCrcFun(0x18005, rev=True, initCrc=0xFFFF, xorOut=0x0000)
crc = crc16(cmd)
print(hex(crc))

ser = serial.Serial()
ser.baudrate= 115200
ser.port = 'COM6'
ser.timeout = 1
ser.open()

ser.write(cmd)
ser.write(crc.to_bytes(2,'little'))

response = ser.read(20+2*(4+4)+2) #data
#response  = ser.read(20+4+4+2) #whoami
print(' '.join(format(x, '02x') for x in response))

check_crc = crc16(response[0:len(response)-2])
print(hex(check_crc))