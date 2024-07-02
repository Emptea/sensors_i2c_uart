from enum import IntEnum
from collections import namedtuple 
from typing import NamedTuple

class lock:
    unlock = 0
    lock = 0x00FF

class fcn_id(IntEnum):
    WHOAMI_REQ = 1
    WHOAMI_ANS = 2
    STATUS_REQ = 3
    STATUS_ANS = 4
    WRITE_REQ = 5
    WRITE_ANS = 6
    READ_REQ = 7
    READ_ANS = 8

class data_type(IntEnum):
    i8 = 1
    u8 = 2
    i16 = 3
    u16 = 4
    i32 = 5
    u32 = 6
    f32 = 7
    f64 = 8
    string = 9
    arr_i8 = 10
    arr_u8 = 11
    arr_u16 = 12
    arr_i32 = 13
    arr_u32 = 15
    arr_f32 = 16
    arr_f64 = 17
    uid = 19
    arr_uid = 20
    uid_range = 21
    access_uid = 22

data_formats = {data_type.i8: 'b',
                data_type.u8: 'B',
                data_type.i16: 'h',
                data_type.u16: 'H',
                data_type.i32: 'i',
                data_type.u32: 'I',
                data_type.f32: 'f',
                data_type.f64: 'd',
                data_type.string: 's',
                data_type.uid_range: 'B B'}

class device_id(IntEnum):
    lm75bd = 1
    tmp112 = 2
    sht30 = 3
    zs05 = 4
    bmp180 = 5
    lps22hb = 6
    handle = 7
    exp = 8
    wetsens = 9

class id_data_handle(IntEnum):
    error = 0x03
    lock_status = 0x04
    new_uid_amount = 0x05
    uid_write = 0x06
    uid_some = 0x07
    read_saved_uids = 0x08
    saved_uid_amount = 0x09
    clear_uid = 0x0A
    time = 0x0B
    write_access = 0x0C
    read_new_uids = 0x0D
    uid_last = 0x0E
    uid_current = 0x0F

class id_data_sens(IntEnum):
    temp = 4
    hum = 5
    press = 6

temp_addr = bytearray([0xd6,0x5e, 0x9b, 0x4f])
press_addr = 0x86C97864
own_addr = 0x00000000
all_addr = 0x00000000
handle_addr = 0x40d17bbd #0x2dac37ce 
exp_addr = bytearray([97, 52, 232, 28])
exp_addr = int.from_bytes(exp_addr, byteorder='little', signed=False)

format_header = '4s I I I H H'
sz_header = 20
format_data_hdr = 'B B H'
sz_data_hdr = 4
format_u8 = format_data_hdr + 'B'
format_u16 = format_data_hdr + 'H'
format_u32 = format_data_hdr + 'I'
format_f32 = format_data_hdr + 'f'

class Header(NamedTuple):
    protocol: str
    cnt: int
    src: str
    dst: str
    cmd: int
    data_sz: int

# Header = namedtuple('Header', 'protocol cnt src dst cmd data_sz')
ChunkHeader = namedtuple('ChunkHeader', 'id type payload_sz')
Data = namedtuple('Data', 'data')
DataAccess = namedtuple('DataAccess', 'uid_field uid_len uid time flag_valid')
DataUid = namedtuple('DataUid', 'uid_len uid')
