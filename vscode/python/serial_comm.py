import serial
import crcmod
import struct
from collections import namedtuple

crc16 = crcmod.mkCrcFun(0x18005, rev=True, initCrc=0xFFFF, xorOut=0x0000)

temp_addr = bytearray([0xd6,0x5e, 0x9b, 0x4f])
press_addr = 0x86C97864
own_addr = 0x00000000
handle_addr = 0x00
exp_addr = bytearray([97, 52, 232, 28])
exp_addr = int.from_bytes(exp_addr, byteorder='little', signed=False)

err_ans_sz = 20+6+2
whoami_ans_sens_sz = 20+8+2
data_ans_hum_press_sz = 20+8+8+2
data_ans_temp_sz = 20+8+2

#whoami
cmd_whoami = bytearray(20)
struct.pack_into('4s I I I H H', cmd_whoami, 0, b'AURA', 1, own_addr, 0, 1, 0)
cmd_whoami.extend(crc16(cmd_whoami).to_bytes(2,'little'))

#data req
cmd_data = bytearray(20)
struct.pack_into('4s I I I H H', cmd_data, 0, b'AURA', 1, own_addr, 0, 3, 0)
cmd_data.extend(crc16(cmd_data).to_bytes(2,'little'))

#write req for handle
cmd_write_handle = bytearray(20)
struct.pack_into('4s I I I H H', cmd_write_handle, 0, b'AURA', 1, 0, handle_addr, 7, 0)
cmd_write_handle.extend(crc16(cmd_write_handle).to_bytes(2,'little'))

#write req for expander
cmd_write_exp = bytearray(26)
struct.pack_into('4s I I I H H B B H H', cmd_write_exp, 0, b'AURA', 1, 0, exp_addr, 5, 6, 4, 4, 2, 0x00FF)
cmd_write_exp.extend(crc16(cmd_write_exp).to_bytes(2,'little'))

#write req for sens
cmd_write_sens_1meas = bytearray(28)
struct.pack_into('4s I I I H H B B H f', cmd_write_sens_1meas, 0, b'AURA', 1, 0, press_addr, 5, 8, 4, 7, 4, 5.5)
cmd_write_sens_1meas.extend(crc16(cmd_write_sens_1meas).to_bytes(2,'little'))

cmd_write_sens_2meas = bytearray(36)
struct.pack_into('4s I I I H H B B H f B B H f', cmd_write_sens_2meas, 0, b'AURA', 1, 0, press_addr, 5, 16, 4, 7, 4, 5.5, 4, 7, 4, 0)
cmd_write_sens_2meas.extend(crc16(cmd_write_sens_2meas).to_bytes(2,'little'))
print(' '.join(format(x, '02x') for x in cmd_write_sens_2meas))

ser = serial.Serial()
ser.baudrate= 19200
ser.port = 'COM13'
ser.timeout = 0.05
ser.open()

for i in range(1):
    ser.write(cmd_write_sens_2meas)
    for k in range(1):
        print('err ans sensor', k)
        response  = ser.read(err_ans_sz)
        if response: print(struct.unpack('4s I I I H H B B H H H', response))
        print(' '.join(format(x, '02x') for x in response))
            
# for i in range(10):
#     if i%2 == 1:
#         ser.write(cmd_write_sens_2meas)
#         for k in range(1):
#             print('err ans sensor', k)
#             response  = ser.read(err_ans_sz)
#             if response: print(struct.unpack('4s I I I H H B B H H H', response))

#     else:
#         ser.write(cmd_data)

#         for r in range(1):
#             print('data sensor')
#             response  = ser.read(data_ans_hum_press_sz)
#             if response: print(struct.unpack('4s I I I H H B B H f B B H f H', response))
#             print(' '.join(format(x, '02x') for x in response))