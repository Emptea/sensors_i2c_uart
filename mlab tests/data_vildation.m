clear all
device = serialport("COM6",115200);
%read header
protocol = read(device,4,"char");
cnt = read(device,1, 'uint16');
dis = read(device,1,'uint8');
flags = read(device,1,'uint8');
src = read(device,1,"uint32");
dest = read(device,1,"uint32");
path = read(device,4,"uint32");
%read chunk
id = read(device, 1, 'uint16');
type = read(device, 1, 'uint16');
payload_sz = read(device, 1, 'uint16');
%read data
if (flags == 2)
    data = read(device, payload_sz, 'uint8');
    switch(type)
        case 1
            data =  typecast(data, 'char');
        case 2
            data =  typecast(data, 'char');
        case 3
             data =  typecast(data, 'int16');
        case 4
             data =  typecast(data, 'uint16');
        case 5
             data =  typecast(data, 'int32');
        case 6
             data =  typecast(data, 'uint32');
        case 7
             data =  typecast(data, 'single');
        case 8
             data =  typecast(data, 'double');
        case 9
             data =  typecast(data, 'char');
    end
end
%read crc
crc = read(device, 1, 'uint16');
if (flags == 2)
    all_msg = [uint8(protocol) typecast(uint16(cnt), 'uint8') ...
                uint8(dis) uint8(flags) typecast(uint32(src), 'uint8') ... 
                typecast(uint32(dest), 'uint8') typecast(uint32(path), 'uint8') ...
                typecast(uint16([id, type, payload_sz]), 'uint8') ...
                typecast(data, 'uint8')];

    if crc == crc16(all_msg)
        switch(type)
            case 1
                fprintf("LM75BD temp = %.2f\n", data(1))
            case 2
                fprintf("TEMP112 temp = %.2f\n", data(1))
            case 3
                fprintf("SHT30 temp = %.2f\n", data(1))
            case 4
                fprintf("ZS05 temp = %.2f. hum = %.2f\n", data(1), data(2))
            case 5
                fprintf("BMP180 temp = %.2f, press = %.2f\n", data(1), data(2))
        end
    end
else
        all_msg = [typecast(protocol, 'uint8') 
                typecast(cnt, 'uint8')
                dis
                flags
                typecast(src, 'uint8')
                typecast(dest, 'uint8')
                typecast(path, 'uint8')
                typecast([id, type, payload_sz], 'uint8')];
        disp(crc == crc16(all_msg))
end





function [crc, hex] = crc16(packet)
    % Calculates CRC16/XModem for packet
    % Input: byte content in an array
    % Ref: https://en.wikipedia.org/wiki/Computation_of_cyclic_redundancy_checks 
    % Usage: crc16( [49 50] ) - ASCII for 1 and 2
    % Validation: dec2hex( crc16( double('123456789') ) ) compare the result at
    % https://www.lammertbies.nl/comm/info/crc-calculation
    % J. Chen. Feb, 2020
    
    crc = 0;
    % crc = hex2dec('FFFF');   % for 0xFFFF version
    
    for i = 1:length(packet)
        crc = bitxor( crc, bitshift(packet(i),8) );
        for bit = 1:8
            if bitand( crc, hex2dec('8000') )     % if MSB=1
              crc = bitxor( bitshift(crc,1), hex2dec('1021') );
            else
              crc = bitshift(crc,1);
            end
            crc = bitand( crc, hex2dec('ffff') );  % trim to 16 bits
        end
    end
    hex = dec2hex(crc);
end
