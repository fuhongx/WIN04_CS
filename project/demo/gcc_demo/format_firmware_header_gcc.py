import os

BOOTLOADER_HEADER_SIZE  = 24
BOOT2_HEAR_SIZE_BYTE    = 4 * 6


#Record Boot info
Size = 0
Crc = 0
Version = 0
Flag = 0
LoadAddr = 0
HexLastBuildTime = 0
Boot2HeaderCrc = 0

CRC32_IEEE_TABLE = [
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
    0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
    0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
    0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
    0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
    0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
    0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
    0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
    0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
    0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
    0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
    0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
    0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
    0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
    0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
    0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
    0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
    0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
    0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
    0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
    0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
    0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
    0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
    0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
    0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
    0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
    0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
    0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
    0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
    0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
    0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
    0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
    0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
    0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
    0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
]


# 计算crc校验函数
def crc32_ieee(data):
    crc_table = CRC32_IEEE_TABLE
    crc = 0xFFFFFFFF  # 初始值
    i = 0
    for item in data:
        crc = crc_table[(crc ^ item) & 0xFF] ^ (crc >> 8)
        # print("Current Data = " + item + ", Crc = " + hex(crc))
        i = i + 1

    return crc ^ 0xFFFFFFFF  # 结果异或值

def get_bin_size(file):
    binfile = open(file, 'rb')  # 打开二进制文件
    size = os.path.getsize(file)  # 获得文件大小
    binfile.close()
    return size

def get_bin_data(file):
    binfile = open(file, 'rb')  # 打开二进制文件
    size = os.path.getsize(file)  # 获得文件大小

    new_data = binfile.read(size)
    binfile.close()
    return new_data


def get_info(Header):
    global Flag,Version,LoadAddr
    #byte 0-3 Flag
    Flag = (Header[0]) | (Header[1]) << 8 | (Header[2]) << 16 | (Header[3]) << 24
    Version = (Header[4]) | (Header[5]) << 8 | (Header[6]) << 16 | (Header[7]) << 24
    LoadAddr = (Header[16]) | (Header[17]) << 8 | (Header[18]) << 16 | (Header[19]) << 24

def make_header_info(data,size):
    HeaderData = data[0:BOOT2_HEAR_SIZE_BYTE]

    CodeData = data[BOOT2_HEAR_SIZE_BYTE:size + BOOT2_HEAR_SIZE_BYTE]
    get_info(HeaderData)

    CodeSize = size - BOOT2_HEAR_SIZE_BYTE
    # print('size = ' + str(CodeSize))
    Len_Temp = CodeSize & 0xff
    HeaderData[8] = Len_Temp
    Len_Temp = (CodeSize >> 8) & 0xff
    HeaderData[9] = Len_Temp
    Len_Temp = (CodeSize >> 16) & 0xff
    HeaderData[10] = Len_Temp
    Len_Temp = (CodeSize >> 24) & 0xff
    HeaderData[11] = Len_Temp

    # byte 12 : 15 - CRC
    Boot2CodeCrc = crc32_ieee(CodeData)
    Len_Temp = Boot2CodeCrc & 0xff
    HeaderData[12] = Len_Temp
    Len_Temp = (Boot2CodeCrc >> 8) & 0xff
    HeaderData[13] = Len_Temp
    Len_Temp = (Boot2CodeCrc >> 16) & 0xff
    HeaderData[14] = Len_Temp
    Len_Temp = (Boot2CodeCrc >> 24) & 0xff
    HeaderData[15] = Len_Temp

    # print("Boot2CodeCrc Value = " + hex(Boot2CodeCrc))

    HeaderWoCrc = HeaderData[0:BOOT2_HEAR_SIZE_BYTE - 4]
    Boot2HeaderCrc = crc32_ieee(HeaderWoCrc)
    # Boot2Header Crc
    Len_Temp = Boot2HeaderCrc & 0xff
    HeaderData[20] = Len_Temp
    Len_Temp = (Boot2HeaderCrc >> 8) & 0xff
    HeaderData[21] = Len_Temp
    Len_Temp = (Boot2HeaderCrc >> 16) & 0xff
    HeaderData[22] = Len_Temp
    Len_Temp = (Boot2HeaderCrc >> 24) & 0xff
    HeaderData[23] = Len_Temp
    new_data = []
    new_data[0:BOOT2_HEAR_SIZE_BYTE] = HeaderData
    new_data[BOOT2_HEAR_SIZE_BYTE:size + BOOT2_HEAR_SIZE_BYTE] = CodeData
    return new_data

BOOT2_SIZE = 0x1000

def make_boot2_gcc(name,Size):
    file = name
    Boot2_Output = name + '_boot2'

    try:
        os.remove(Boot2_Output + '.bin')
    except:
        print('remove done!')

    try:
        os.remove(Boot2_Output + '.hex')
    except:
        print('remove done!')

    data = get_bin_data(file + '.bin')
    new_data = []
    for i in data:
        new_data.append(i)

    InputData = bytes(new_data[0:Size])
    with open(Boot2_Output + '.hex', 'wb') as f:
        # 将每个元素打包成二进制数据并写入文件
        f.write(InputData)
        f.close()
    try:
        os.rename(Boot2_Output + '.hex', Boot2_Output + '.bin')
    except:
        print("Bin rename done!\n")

    try:
        os.remove(Boot2_Output + '.hex')
    except:
        print('remove done!')

def make_firm_gcc(name, boot2SIze):
    file = name
    Output = name + '_firmware'

    try:
        os.remove(Output + '.bin')
    except:
        print('remove done!')

    try:
        os.remove(Output + '.hex')
    except:
        print('remove done!')

    data = get_bin_data(file + '.bin')
    Size = get_bin_size(file + '.bin')
    new_data = []

    for i in data[boot2SIze:Size]:
        new_data.append(i)

    Format_data = make_header_info(new_data, Size - boot2SIze)


    InputData = bytes(Format_data)
    with open(Output + '.hex', 'wb') as f:
        # 将每个元素打包成二进制数据并写入文件
        f.write(InputData)
        f.close()
    try:
        os.rename(Output + '.hex', Output + '.bin')
    except:
        print("Bin rename done!\n")

    try:
        os.remove(Output + '.hex')
    except:
        print('remove done!')

def combine_bin_files(boot2_path, firmware_path, output_path):
    """
    将boot2.bin和firmware.bin合并到一个文件中

    参数:
        boot2_path (str): boot2.bin文件的路径
        firmware_path (str): firmware.bin文件的路径
        output_path (str): 合并后的输出文件路径
    """
    try:
        # 读取boot2.bin文件
        with open(boot2_path, 'rb') as boot2_file:
            boot2_data = boot2_file.read()
        
        # 读取firmware.bin文件
        with open(firmware_path, 'rb') as firmware_file:
            firmware_data = firmware_file.read()
        
        # 将两个文件的数据合并
        combined_data = boot2_data + firmware_data
        
        # 将合并后的数据写入输出文件
        with open(output_path, 'wb') as output_file:
            output_file.write(combined_data)

        print(f"combine bin success {output_path}")

    except FileNotFoundError as e:
        print(f"err: not found - {e}")
    except Exception as e:
        print(f"combine occurred an error: {e}")

import sys

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print("argv err!")
        sys.exit(1)

    Boot2Size = int(sys.argv[2], 16)

    make_boot2_gcc(sys.argv[1], Boot2Size)
    make_firm_gcc(sys.argv[1], Boot2Size)

    combine_bin_files(sys.argv[1] + '_boot2.bin', sys.argv[1] + '_firmware.bin', sys.argv[1] + '.bin')
