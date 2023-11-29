import serial
import crcmod

#whoami
cmd_whoami = bytearray('AURA'.encode())
cmd_whoami.extend(bytearray([1]))
cmd_whoami.extend(bytearray(3))
cmd_whoami.extend(bytearray(4))
cmd_whoami.extend(bytearray(4))
cmd_whoami.extend(bytearray([1])) #1 - whoami, 3 - data
cmd_whoami.extend(bytearray(1))
cmd_whoami.extend(bytearray(2))

#data req
cmd_data = bytearray('AURA'.encode())
cmd_data.extend(bytearray([1]))
cmd_data.extend(bytearray(3))
cmd_data.extend(bytearray(4))
cmd_data.extend(bytearray(4))
cmd_data.extend(bytearray([3])) #1 - whoami, 3 - data
cmd_data.extend(bytearray(1))
cmd_data.extend(bytearray(2))

#write req for handle
cmd_write = bytearray('AURA'.encode())
cmd_write.extend(bytearray([1]))
cmd_write.extend(bytearray(3))
cmd_write.extend(bytearray(4))
cmd_write.extend(bytearray([206,55, 172, 45])) #id of handle
cmd_write.extend(bytearray([7])) #1 - whoami, 3 - data, 5 - write req
cmd_write.extend(bytearray(1))
cmd_write.extend(bytearray(2))

#write req for expander
cmd_write_exp = bytearray('AURA'.encode())
cmd_write_exp.extend(bytearray([1]))
cmd_write_exp.extend(bytearray(3))
cmd_write_exp.extend(bytearray(4))
cmd_write_exp.extend(bytearray([97, 52, 232, 28])) #id of expander
cmd_write_exp.extend(bytearray([5,0])) #1 - whoami, 3 - data, 5 - write req
cmd_write_exp.extend(bytearray([6,0]))
cmd_write_exp.extend(bytearray([4,4,2,0]))
cmd_write_exp.extend(bytearray([0,0]))

crc16 = crcmod.mkCrcFun(0x18005, rev=True, initCrc=0xFFFF, xorOut=0x0000)

ser = serial.Serial()
ser.baudrate= 115200
ser.port = 'COM6'
ser.timeout = 1
ser.open()

ser.write (cmd_write_exp)
ser.write(crc16(cmd_write_exp).to_bytes(2,'little'))
response  = ser.read(20+2*8+2) #whoami from expander
print(' '.join(format(x, '02x') for x in response))

# for i in range(2):
#     if i%2 == 0:
#         ser.write(cmd_whoami)
#         ser.write(crc16(cmd_whoami).to_bytes(2,'little'))
#         print('whoami expander')
#         response  = ser.read(30) #whoami from expander
#         print(' '.join(format(x, '02x') for x in response))
#         for k in range(2):
#             print('whoami sensor', k)
#             response  = ser.read(20+8+8+2) #whoami from sensor
#             print(' '.join(format(x, '02x') for x in response))

#     else:
#         ser.write(cmd_data)
#         ser.write(crc16(cmd_data).to_bytes(2,'little'))

#         print('data expander')
#         response  = ser.read(20)
#         print(' '.join(format(x, '02x') for x in response))
#         response  = ser.read(6)
#         print(' '.join(format(x, '02x') for x in response))
#         response  = ser.read(6)
#         print(' '.join(format(x, '02x') for x in response))
#         response  = ser.read(6)
#         print(' '.join(format(x, '02x') for x in response))
#         response  = ser.read(6)
#         print(' '.join(format(x, '02x') for x in response))
#         response  = ser.read(2)
#         print(' '.join(format(x, '02x') for x in response))

#         for r in range(2):
#             print('data sensor', r)
#             response  = ser.read(20 + 2*8 + 2)
#             print(' '.join(format(x, '02x') for x in response))