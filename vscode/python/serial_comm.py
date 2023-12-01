import serial
import crcmod

crc16 = crcmod.mkCrcFun(0x18005, rev=True, initCrc=0xFFFF, xorOut=0x0000)

#whoami
cmd_whoami = bytearray('AURA'.encode())
cmd_whoami.extend(bytearray([1]))
cmd_whoami.extend(bytearray(3))
cmd_whoami.extend(bytearray(4))
cmd_whoami.extend(bytearray(4))
#cmd_whoami.extend(bytearray([0xd6,0x5e, 0x9b, 0x4f])) #temp sens id
cmd_whoami.extend(bytearray([1])) #1 - whoami, 3 - data
cmd_whoami.extend(bytearray(1))
cmd_whoami.extend(bytearray(2))
cmd_whoami.extend(crc16(cmd_whoami).to_bytes(2,'little'))

#data req
cmd_data = bytearray('AURA'.encode())
cmd_data.extend(bytearray([1]))
cmd_data.extend(bytearray(3))
cmd_data.extend(bytearray(4))
cmd_data.extend(bytearray(4))
cmd_data.extend(bytearray([3])) #1 - whoami, 3 - data
cmd_data.extend(bytearray(1))
cmd_data.extend(bytearray(2))
cmd_data.extend(crc16(cmd_data).to_bytes(2,'little'))

#write req for handle
cmd_write = bytearray('AURA'.encode())
cmd_write.extend(bytearray([1]))
cmd_write.extend(bytearray(3))
#cmd_write.extend(bytearray(4))
cmd_write.extend(bytearray([0xd6,0x5e, 0x9b, 0x4f])) #temp sens id
cmd_write.extend(bytearray([206,55, 172, 45])) #id of handle
cmd_write.extend(bytearray([7])) #1 - whoami, 3 - data, 5 - write req
cmd_write.extend(bytearray(1))
cmd_write.extend(bytearray(2))
cmd_write.extend(crc16(cmd_write).to_bytes(2,'little'))

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
cmd_write_exp.extend(crc16(cmd_write_exp).to_bytes(2,'little'))


ser = serial.Serial()
ser.baudrate= 115200
ser.port = 'COM6'
ser.timeout = 0.03
ser.open()

# ser.write(cmd_whoami)
# response  = ser.read(20+8+2) #whoami from temp sens
# print(' '.join(format(x, '02x') for x in response))

for i in range (100000):
    ser.write(cmd_data)
    response  = ser.read(20+8*2+2) #whoami from temp sens
    print(' '.join(format(x, '02x') for x in response))

# for i in range(2):
#     if i%2 == 0:
#         ser.write(cmd_whoami)
#         print('whoami expander')
#         response  = ser.read(30) #whoami from expander
#         print(' '.join(format(x, '02x') for x in response))
#         for k in range(2):
#             print('whoami sensor', k)
#             response  = ser.read(20+8+8+2) #whoami from sensor
#             print(' '.join(format(x, '02x') for x in response))

#     else:
#         ser.write(cmd_data)

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