import serial
import crcmod
import struct
from collections import namedtuple

crc16 = crcmod.mkCrcFun(0x18005, rev=True, initCrc=0xFFFF, xorOut=0x0000)

temp_addr = bytearray([0xd6,0x5e, 0x9b, 0x4f])
press_addr = 0x86C97864

err_ans_sz = 20+6+2
whoami_ans_sens_sz = 20+8+2
data_ans_hum_press_sz = 20+8+8+2
data_ans_temp_sz = 20+8+2

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
cmd_write_handle = bytearray('AURA'.encode())
cmd_write_handle.extend(bytearray([1]))
cmd_write_handle.extend(bytearray(3))
#cmd_write.extend(bytearray(4))
cmd_write_handle.extend(bytearray([0xd6,0x5e, 0x9b, 0x4f])) #temp sens id
cmd_write_handle.extend(bytearray([206,55, 172, 45])) #id of handle
cmd_write_handle.extend(bytearray([7])) #1 - whoami, 3 - data, 5 - write req
cmd_write_handle.extend(bytearray(1))
cmd_write_handle.extend(bytearray(2))
cmd_write_handle.extend(crc16(cmd_write_handle).to_bytes(2,'little'))

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

#write req for temp sens
cmd_write_sens = bytearray('AURA'.encode())
cmd_write_sens.extend(bytearray([1, 0, 0, 0]))
cmd_write_sens.extend(bytearray(4))
cmd_write_sens.extend(temp_addr) #temp sens id
cmd_write_sens.extend(bytearray([5,0])) #1 - whoami, 3 - data, 5 - write req
cmd_write_sens.extend(bytearray([8,0]))
cmd_write_sens.extend(bytearray([4, 7, 4, 0])) #temp(1), float(1), sz(2) = 4
cmd_write_sens.extend(bytearray(struct.pack("f", 1.1)))
cmd_write_sens.extend(crc16(cmd_write_sens).to_bytes(2,'little'))

#write req for temp+hum sens
cmd_write_hum_sens = bytearray(28)
# 4s - 4 byte string
# I - u32
# H - u16
# B - u8
# f - f32
struct.pack_into('4s I I I H H B B H f', cmd_write_hum_sens, 0, b'AURA', 1, 0, press_addr, 5, 8, 4, 7, 4, 5.5)
cmd_write_hum_sens.extend(crc16(cmd_write_hum_sens).to_bytes(2,'little'))
print(' '.join(format(x, '02x') for x in cmd_write_hum_sens))

ser = serial.Serial()
ser.baudrate= 19200
ser.port = 'COM13'
ser.timeout = 0.05
ser.open()

# for i in range (1):
#     ser.write(cmd_whoami)
#     response  = ser.read(20+8+2) #whoami from temp sens
#     print(' '.join(format(x, '02x') for x in response))

#     print('data')
#     ser.write(cmd_data)
#     response  = ser.read(20+8+2) #data from temp sens
#     print(' '.join(format(x, '02x') for x in response))

#     print('write ans')
#     ser.write(cmd_write_sens)
#     response  = ser.read(20+6+2) #ans write from temp sens
#     print(' '.join(format(x, '02x') for x in response))

#     print('changed data')
#     ser.write(cmd_data)
#     response  = ser.read(20+8+2) #data from temp sens
#     print(' '.join(format(x, '02x') for x in response))

for i in range(2):
    if i%2 == 1:
        # ser.write(cmd_write_hum_sens)
#         # print('whoami expander')
#         # response  = ser.read(30) #whoami from expander
#         # print(' '.join(format(x, '02x') for x in response))
        for k in range(1):
            print('err ans sensor', k)
            response  = ser.read(err_ans_sz)
            if response: print(struct.unpack('4s I I I H H B B H H H', response))
            # print(' '.join(format(x, '02x') for x in response))

    else:
        ser.write(cmd_data)

#         # print('data expander')
#         # response  = ser.read(20)
#         # print(' '.join(format(x, '02x') for x in response))
#         # response  = ser.read(6)
#         # print(' '.join(format(x, '02x') for x in response))
#         # response  = ser.read(6)
#         # print(' '.join(format(x, '02x') for x in response))
#         # response  = ser.read(6)
#         # print(' '.join(format(x, '02x') for x in response))
#         # response  = ser.read(6)
#         # print(' '.join(format(x, '02x') for x in response))
#         # response  = ser.read(2)
#         # print(' '.join(format(x, '02x') for x in response))

        for r in range(1):
            print('data sensor')
            response  = ser.read(data_ans_hum_press_sz)
            if response: print(struct.unpack('4s I I I H H B B H f B B H f H', response))
            # print(' '.join(format(x, '02x') for x in response))