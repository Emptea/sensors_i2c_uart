import crcmod
import struct
from formats import *
from com_base import *
crc16 = crcmod.mkCrcFun(0x18005, rev=True, initCrc=0xFFFF, xorOut=0x0000)

def create_hdr(fcn,addr,data_sz):
    hdr = bytearray(sz_header+data_sz)
    struct.pack_into(format_header, hdr, 0, 
                 b'AURA', 
                 1, 
                 own_addr, 
                 addr, 
                 fcn, 
                 data_sz)
    return hdr

def com_req_whoami(ser):       
    cmd_whoami = create_hdr(fcn = fcn_id.WHOAMI_REQ, addr = all_addr, data_sz = 0)
    cmd_whoami.extend(crc16(cmd_whoami).to_bytes(2,'little'))
    addr = ask(ser,cmd_whoami)
    return addr

def com_req_data(ser):
    cmd_data = create_hdr(fcn = fcn_id.STATUS_REQ, addr = all_addr, data_sz = 0)
    cmd_data.extend(crc16(cmd_data).to_bytes(2,'little'))
    ask(ser,cmd_data)

#write req for expander
def com_write_exp(ser, addr):
    cmd_write_exp = create_hdr(fcn_id.WRITE_REQ, addr, 6)
    struct.pack_into(format_u16, cmd_write_exp, sz_header, 
                    4, 
                    data_type.u16, 
                    2, 
                    0x00FF)
    cmd_write_exp.extend(crc16(cmd_write_exp).to_bytes(2,'little'))
    ask(ser, cmd_write_exp)

#write req for sens
def com_write_sens_1meas(ser, addr):
    cmd = create_hdr(fcn_id.WRITE_REQ, addr, 8)
    struct.pack_into(format_f32, cmd, sz_header,
                    id_data_sens.temp, 
                    data_type.f32, 
                    4, 
                    5.5)
    cmd.extend(crc16(cmd).to_bytes(2,'little'))
    ask(ser, cmd)

def com_write_sens_2meas(ser, addr, id_data_2nd):
    cmd = create_hdr(fcn_id.WRITE_REQ, addr, 16)
    struct.pack_into(format_f32, cmd, sz_header,
                    id_data_sens.temp, 
                    data_type.f32, 
                    4, 
                    5.5)
    struct.pack_into(format_f32,cmd, sz_header +sz_data_hdr+ 4,
                    id_data_2nd, 
                    data_type.f32, 
                    4, 
                    1.5)
    cmd.extend(crc16(cmd).to_bytes(2,'little'))

#write cards req for handle
def com_handle_write_uids(ser, addr, cards, n_cards):
    cmd_write_cards_handle = create_hdr(fcn_id.WRITE_REQ, addr, 4+n_cards*8)
    struct.pack_into(format_data_hdr, cmd_write_cards_handle, sz_header,
                    id_data_handle.uid_write, 
                    data_type.arr_uid, 
                    n_cards*8)
    for i in range(n_cards):
        struct.pack_into('B 7s', cmd_write_cards_handle, sz_header+sz_data_hdr+i*8,
                        cards[2*i],cards[2*i+1])
    cmd_write_cards_handle.extend(crc16(cmd_write_cards_handle).to_bytes(2,'little'))
    ask(ser, cmd_write_cards_handle)

#open handle
def com_handle_open(ser, addr, lock_status):
    cmd_open_handle = create_hdr(fcn_id.WRITE_REQ, addr, 6)
    struct.pack_into(format_u16, cmd_open_handle, sz_header,
                    id_data_handle.lock_status, 
                    data_type.u16, 
                    2,
                    lock_status)
    cmd_open_handle.extend(crc16(cmd_open_handle).to_bytes(2,'little'))
    ask(ser, cmd_open_handle)

def com_handle_erase_cards(ser, addr):
    cmd_erase_cards_handle = create_hdr(fcn_id.WRITE_REQ, addr, 6)
    struct.pack_into(format_u16, cmd_erase_cards_handle, sz_header,
                    id_data_handle.clear_uid,
                    data_type.u16, 
                    2,
                    0x00FF)
    cmd_erase_cards_handle.extend(crc16(cmd_erase_cards_handle).to_bytes(2,'little'))
    ask(ser, cmd_erase_cards_handle)

def com_handle_read_uids(ser, addr, id_saved_or_new, n_cards, bias = 0):
    cmd_read_uids_handle = create_hdr(fcn_id.READ_REQ, addr, 6)
    struct.pack_into(format_data_hdr +'2B', cmd_read_uids_handle, sz_header,
                    id_saved_or_new,
                    data_type.uid_range,
                    2,
                    bias,
                    n_cards)
    cmd_read_uids_handle.extend(crc16(cmd_read_uids_handle).to_bytes(2,'little'))
    ask(ser, cmd_read_uids_handle)

def com_handle_read_last_uid(ser, addr, access_uid):
    cmd_read_last_uid = create_hdr(fcn_id.WRITE_REQ, addr, 8)
    struct.pack_into(format_u32, cmd_read_last_uid, sz_header,
                    id_data_handle.uid_last,
                    data_type.u32,
                    4,
                    access_uid)
    cmd_read_last_uid.extend(crc16(cmd_read_last_uid).to_bytes(2,'little'))
    ask(ser, cmd_read_last_uid)

def com_handle_write_time(ser, addr, time):
    cmd_write_time_handle = create_hdr(fcn_id.WRITE_REQ, addr, 8)
    struct.pack_into(format_u32, cmd_write_time_handle, sz_header,
                    id_data_handle.time,
                    data_type.u32,
                    4,
                    time)
    cmd_write_time_handle.extend(crc16(cmd_write_time_handle).to_bytes(2,'little'))
    ask(ser, cmd_write_time_handle)
