import time
import serial
import random
from HexToBin import Hex2Bin
from optparse import OptionParser


usage = '''
         usage: %prog [options]
         Files needed:    com_select(Serial address of the device) (optional, default = /dev/ttyUSB0)
                          bps(Baud rate of communication) (optional, default = 115200)
                          string_len(Number of generated sequences) (optional, default = 1000)
                          flag_bin(Binary file) (optional, default = 1)
                          file(Record Location) (optional, default = ./test.txt)
                          ID (ID of user) (optional, default = 19970408)
                          Type (The methods of STM32) (optional, default = 00)
                          From_Addr (From Address) (optional, default = 01)
                          To_Addr (To Address) (optional, default = 02)
                          Len (The len of package data) (optional, default = 0010)
                          Data (package data) (optional, default = 0123456789)
                          err (Error) (optional, default = 1)
      '''
parser = OptionParser(usage =usage)
parser.add_option('-c', '--com', dest='com',  default='/dev/ttyUSB0',
                  help='com_select(device port) (optional, default = /dev/ttyUSB0)')
parser.add_option('-b', '--bps', dest='bps', type=int,default=115200,
                  help='bps(device bps) (optional, default = 115200)')
parser.add_option('-s', '--string', dest='string', type=int, default=1000,
                  help='string_len(data length) (optional, default = 1000)')
parser.add_option('-f', '--flag', dest='flag', type=int, default=1,
                  help='flag_bin(bin) (optional, default = 1)')
parser.add_option('-A', '--AddressFile', dest='AddressFile',  default='./test.txt',
                  help='file(file) (optional, default = ./test.txt)')
parser.add_option('-I', '--ID', dest='ID',  default='19970408',
                  help='ID (ID of user) (optional, default = 19970408)')
parser.add_option('-T', '--Type', dest='Type', default='00',
                  help='Type (The methods of STM32) (optional, default = 00)')
parser.add_option('-F', '--From', dest='From',  default='01',
                  help='From_Addr (From Address) (optional, default = 01)')
parser.add_option('-t', '--to', dest='To', default='02',
                  help='To_Addr (To Address) (optional, default = 02)')
parser.add_option('-L', '--Len', dest='Len', default='0010',
                  help='Len (The len of package data) (optional, default = 0010)')
parser.add_option('-D', '--Data', dest='Data',  default='0123456789',
                  help='Data (package data) (optional, default = 0123456789)')
parser.add_option('-E', '--Err', dest='Error', default='1',
                  help='err (Error) (optional, default = 1)')
options, args = parser.parse_args()

com_select=options.com
bps=options.bps
string_len=options.string
flag_bin=options.flag
file=options.AddressFile
ID=options.ID
Type=options.Type
From_Addr=options.From
To_Addr=options.To
Len=options.Len
Data=options.Data
Err=options.Error


#功能选择控制部分
#输出规定长度散列序列
len_s = 0
temp_str=[]
str_word = 'Z'+ID+Type+From_Addr+To_Addr+Len+Data+Err+'Y'
ser = serial.Serial(com_select,bps)
if(Type=='00'):
    with open(file, 'ab+') as f:
        while len_s < string_len:
            ser.write(str_word.encode())
            line = ser.readline()
            f.write(line.strip())
            if line.strip().decode()=='':
                temp_str.append(1)
                if len(temp_str)==8:
                    print("	request timeout!")
                    break
                continue
            temp_str=[]
            len_s = len_s + 1
            print(line.strip())
            print(len_s)
    ser.close()
    if flag_bin == 1:
        Hex2Bin(file)
#停止产生随机序列
elif(Type=='01'):
    print("Device stopped generating random sequence！")
    ser.write(str_word.encode())
    ser.close()
#开启产生随机序列
elif(Type=='02'):
    print("Device turned on to generate random sequence！")
    ser.write(str_word.encode())
    ser.close()
#清空循环队列数据
elif (Type == '03'):
    print("Device cleared circular queue buffer！")
    ser.write(str_word.encode())
    ser.close()
