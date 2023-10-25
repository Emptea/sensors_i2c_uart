%% CALIBRATION VALUES
ac1 = 10107;
ac2 =-1272;
ac3 =-14648;
ac4 = 34156;
ac5 = 24663;
ac6 = 20999;
b1 = 6515;
b2 = 54;
mb = -32768;
mc = -11786;
md = 3025;

%% UNCOMP VALUES
ut = hex2dec('0x2A00');
up = hex2dec('0x3000');

%% ALGORITHM TEMPERATURE
x1 = (ut-ac6)*ac5/2^15;
x2 = mc*2^11/(x1+md);
b5 = x1+x2;
t = (b5+8)/2^4;

%% ALGORITHM PRESSURE
b6 = b5-4000;
x1 = (b2*(b6*b6/2^12))/2^11;
x2 = ac2*b6/2^11;
x3 = x1+x2;
b3 = ((ac1*4+x3)+2)/4;
x1 = ac3*b6/2^13;
x2 = (b1*(b6*b6/2^12))/2^16;
x3 = ((x1+x2)+2)/2^2;
b4 = ac4*abs(x3+32768)/2^15;
b7 = (abs(up)-b3)*50000;
if (b7<hex2dec('0x80000000'))
    p = b7*2/b4;
else
    p = b7/b4*2;
end
x1 = p/2^8*p/2^8;
x1 = x1*3038/2^16;
x2 = (-7357*p)/2^16;
p = p+(x1+x2+3791)/2^4;