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

format_header = '4s I I I H H'
format_data_hdr = 'B B H'
format_u8 = format_data_hdr + 'B'
format_u16 = format_data_hdr + 'H'
format_u32 = format_data_hdr + 'I'
format_f32 = format_data_hdr + 'f'

err_ans_sz = 20+6+2
err_ans_format = format_header + format_u16 + 'H'

data_ans_exp_sz = 20+6+6+2*6+2
data_ans_exp_format = format_header + format_u16  + format_u16 + format_u16 + format_u16 + 'H'

whoami_ans_sens_sz = 20+8+2
whoami_ans_sens_format = format_header + format_u32 + 'H'

data_ans_hum_press_sz = 20+8+8+2
data_ans_hum_press_format = format_header + format_f32 + format_f32 + 'H'

data_ans_temp_sz = 20+8+2
data_ans_temp_format = format_header + format_f32 + 'H'

data_ans_wetsens_sz = 20+6+2
data_ans_wetsens_format = format_header + format_u8 + 'H'

#whoami
cmd_whoami = bytearray(20)
struct.pack_into(format_header, cmd_whoami, 0, b'AURA', 1, own_addr, 0, 1, 0)
cmd_whoami.extend(crc16(cmd_whoami).to_bytes(2,'little'))

#data req
cmd_data = bytearray(20)
struct.pack_into(format_header, cmd_data, 0, b'AURA', 1, own_addr, 0, 3, 0)
cmd_data.extend(crc16(cmd_data).to_bytes(2,'little'))

#write req for handle
cmd_write_handle = bytearray(20)
struct.pack_into(format_header, cmd_write_handle, 0, b'AURA', 1, 0, handle_addr, 7, 0)
cmd_write_handle.extend(crc16(cmd_write_handle).to_bytes(2,'little'))

#write req for expander
cmd_write_exp = bytearray(26)
struct.pack_into(format_header + format_u16, cmd_write_exp, 0, b'AURA', 1, 0, exp_addr, 5, 6, 4, 4, 2, 0x00FF)
cmd_write_exp.extend(crc16(cmd_write_exp).to_bytes(2,'little'))

#write req for sens
cmd_write_sens_1meas = bytearray(28)
struct.pack_into(format_header + format_f32, cmd_write_sens_1meas, 0, b'AURA', 1, 0, press_addr, 5, 8, 4, 7, 4, 5.5)
cmd_write_sens_1meas.extend(crc16(cmd_write_sens_1meas).to_bytes(2,'little'))

cmd_write_sens_2meas = bytearray(36)
struct.pack_into(format_header + format_f32 + format_f32, cmd_write_sens_2meas, 0, b'AURA', 1, 0, press_addr, 5, 16, 4, 7, 4, 5.5, 4, 7, 4, 0)
cmd_write_sens_2meas.extend(crc16(cmd_write_sens_2meas).to_bytes(2,'little'))

def ask(cmd, ans_format, ans_sz):
    ser.write(cmd)
    response = ser.read(ans_sz)
    if response:
        print(' '.join(format(x, '02x') for x in response))
        try:
            unpacked_response = struct.unpack(ans_format, response)
            print(unpacked_response)  # For debugging or verification
            return unpacked_response
        except struct.error as e:
            print(f"Can't unpack struct: {e}")
            return None
    else:
        return None  # or handle the case where response is empty


def ask_sens():
    resp_whoami = ask(cmd_whoami, whoami_ans_sens_format, 30)
    if resp_whoami is None:
        print("Failed to unpack response for WHOAMI")
        return

    try:
        if resp_whoami[9] == 1:
            ask(cmd_data, data_ans_temp_format, data_ans_temp_sz)
            print('temp sens')
        elif resp_whoami[9] == 9:
            ask(cmd_data, data_ans_wetsens_format, data_ans_wetsens_sz)
            print('wet sens')
        elif resp_whoami[9] == 5:
            ask(cmd_data, data_ans_hum_press_format, data_ans_hum_press_sz)
            print('press+temp sens')
        elif resp_whoami[9] == 4:
            ask(cmd_data, data_ans_hum_press_format, data_ans_hum_press_sz)
            print('hum+temp sens')
        else:
            print('not a sens')
    except IndexError as e:
        print(f"Error accessing field in response: {e}")


ports = [f'/dev/ttyUSB{i}' for i in range(8)]

ser = serial.Serial()
ser.baudrate = 19200
ser.timeout = 0.05

for port in ports:
    print(port)
    try:
        ser.port = port
        ser.open()
        ask_sens()
        ser.close()
    except serial.SerialException as e:
        print(f"Port {port} closed: {e}")
    except Exception as e:
        print(f"Unexpected error on port {port}: {e}")
    finally:
        if ser.is_open:
            ser.close()
    print()