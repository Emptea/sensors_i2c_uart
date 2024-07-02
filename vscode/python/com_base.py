from formats import *
import struct
import serial

def unpack_chunk_data(resp, chunk_hdr, bias):
    if chunk_hdr.type == data_type.arr_uid:
        format = data_formats[data_type.u8] + '7s'
        data = []
        n_cards = int(chunk_hdr.payload_sz/8)
        for i in range(n_cards):
            data += DataUid._make(struct.unpack_from(format,resp,sz_data_hdr+i*8))
    elif chunk_hdr.type == data_type.access_uid:
        format = data_formats[data_type.u32] + data_formats[data_type.u8] +'7s' + data_formats[data_type.u32] + data_formats[data_type.u16]
        data = DataAccess._make(struct.unpack_from(format,resp,sz_data_hdr+bias))
    else: 
        format = data_formats[chunk_hdr.type]
        data = Data._make(struct.unpack_from(format,resp,sz_data_hdr+bias))
    print(data)
    

def unpack_chunk(ser, hdr):
    resp = ser.read(hdr.data_sz+2)
    print(' '.join(format(x, '02x') for x in resp))
    sz_resp = hdr.data_sz
    bias = 0
    while(sz_resp):
        chunk_hdr = ChunkHeader._make(struct.unpack_from(format_data_hdr,resp, bias))
        print(chunk_hdr)
        if chunk_hdr.payload_sz > 0:
            unpack_chunk_data(resp, chunk_hdr, bias)
        bias+=sz_data_hdr+chunk_hdr.payload_sz    
        sz_resp -= sz_data_hdr+chunk_hdr.payload_sz
    

def ask(ser, cmd):
    ser.write(cmd)
    print(' '.join(format(x, '02x') for x in cmd))
    hdr_response=ser.read(sz_header)
    #print(' '.join(format(x, '02x') for x in hdr_response))
    if hdr_response:
        hdr = Header._make(struct.unpack(format_header, hdr_response))
        print(hdr)
        if(hdr.data_sz>3):
            unpack_chunk(ser,hdr)
    print('')
    return hdr.src