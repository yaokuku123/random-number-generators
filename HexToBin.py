import string
import os

def Hex2Bin(file):
    line = file
    str_f = open(line, 'r')
    c = str_f.read()
    str_f.close()
    c.strip('\n')
    c.replace(' ', '')
    len_s = int(len(c) / 2)  # 计算字节的个数
    list_nums = []  # 定义一个空的list
    print(c)
    i = 0
    for i in range(0, len_s):  # 循环将解析出来的字节填充到list中
        chs = c[2 * i: 2 * i + 2]
        num = int(chs, 16)
        list_nums.append(num)
    bys = bytes(list_nums)  # 将list转换成字节数组
    f = open("test.bin", "wb")  # 以原先的文件名称并且加上.bin后缀后打开文件
    f.write(bys)  # 写byte数组写入到二进制文件中
    f.close()


if __name__ == "__main__":
    file = r"D:\science_research\real_project\STM32\STM32_md5_sha256\python_link\test.txt"
    Hex2Bin(file)
