import serial
from formats import *
from com import *
import time

ser = serial.Serial()
ser.baudrate= 115200
ser.port = 'COM18'
ser.timeout = 0.05
ser.open()

time.sleep(4)
addr = com_req_whoami(ser)
# print(addr)
com_req_data(ser)
com_write_sens_1meas(ser, addr, 5.5)
#com_write_sens_2meas(ser, addr, id_data_sens.hum, 3.5, 5.5)
time.sleep(1)
com_req_data(ser)
# com_handle_open(ser, addr, lock.unlock)

# com_handle_erase_cards(ser, addr)
# cards = list([7, b"4\x11\x9ci4'\xe6", 
#               4, b"\xa5\x82<\x9a\x00\x00\x00"])
# com_handle_write_uids(ser, addr, cards, 2)

# com_handle_write_uids(ser, addr, cards, 2)

# n_cards = 5
# com_handle_read_uids(ser, addr, id_data_handle.read_saved_uids, n_cards)

# n_cards = 3
# com_handle_read_uids(ser, addr, id_data_handle.read_new_uids, n_cards)

# com_handle_read_last_uid(ser, addr, 2)

# com_handle_write_time(ser, addr, 10)