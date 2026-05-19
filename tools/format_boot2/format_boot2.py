import os
import time
import datetime
import hashlib
import hmac
import secrets

FILE_PATH = "./Objects/"
FILE_NAME = "boot2.hex"

CREATE_FILE_PATH = "./"
CREATE_FILE_NAME = "boot2.c"

PYTHON_BOOT2_VERSION  = 'V2.0.0'

BOOT2_HEAR_SIZE_BYTE = 120
HASH_OFFSET = 20
HASH_SIZE = 32
SIGNATURE_SIZE = 64
SIGNATURE_OFFSET = HASH_OFFSET + HASH_SIZE
ECC256K1_PRIVATE_KEY_HEX = '91D75732443B75AC2A3CBECDFCE3D29826C726C8EF1E1D81DC5002874D0D5DB9'

SECP256K1_P = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F
SECP256K1_N = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141
SECP256K1_GX = 0x79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798
SECP256K1_GY = 0x483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8

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
        crc = crc_table[(crc ^ int(item, 16)) & 0xFF] ^ (crc >> 8)
        #print("Current Data = " + item + ", Crc = " + hex(crc))
        i = i + 1
    #print("Crc run times = " + str(i))
    return crc ^ 0xFFFFFFFF  # 结果异或值

def extended_gcd(a, b):
    if a == 0:
        return b, 0, 1

    g, x, y = extended_gcd(b % a, a)
    return g, y - (b // a) * x, x

def inverse_mod(value, modulo):
    value %= modulo
    if value == 0:
        raise ZeroDivisionError('division by zero')

    g, x, _ = extended_gcd(value, modulo)
    if g != 1:
        raise ValueError('Mod inverse does not exist')

    return x % modulo

def secp256k1_point_add(point_a, point_b):
    if point_a is None:
        return point_b
    if point_b is None:
        return point_a

    x1, y1 = point_a
    x2, y2 = point_b

    if x1 == x2 and y1 != y2:
        return None

    if x1 == x2 and y1 == 0:
        return None

    if point_a == point_b:
        slope = (3 * x1 * x1) * inverse_mod(2 * y1, SECP256K1_P) % SECP256K1_P
    else:
        slope = (y2 - y1) * inverse_mod(x2 - x1, SECP256K1_P) % SECP256K1_P

    x3 = (slope * slope - x1 - x2) % SECP256K1_P
    y3 = (slope * (x1 - x3) - y1) % SECP256K1_P

    return (x3, y3)

def secp256k1_point_mul(scalar, point):
    result = None
    addend = point

    while scalar:
        if scalar & 1:
            result = secp256k1_point_add(result, addend)
        addend = secp256k1_point_add(addend, addend)
        scalar >>= 1

    return result

def bits2int(data):
    value = int.from_bytes(data, 'big')
    bit_len = len(data) * 8
    n_bit_len = SECP256K1_N.bit_length()
    if bit_len > n_bit_len:
        value >>= (bit_len - n_bit_len)
    return value

def int2octets(value):
    return value.to_bytes(32, 'big')

def bits2octets(data):
    z1 = bits2int(data)
    z2 = z1 - SECP256K1_N
    if z2 < 0:
        z2 = z1
    return int2octets(z2)

def deterministic_k(private_key, digest):
    key = int2octets(private_key)
    data = bits2octets(digest)
    v = b'\x01' * 32
    k = b'\x00' * 32

    k = hmac.new(k, v + b'\x00' + key + data, hashlib.sha256).digest()
    v = hmac.new(k, v, hashlib.sha256).digest()
    k = hmac.new(k, v + b'\x01' + key + data, hashlib.sha256).digest()
    v = hmac.new(k, v, hashlib.sha256).digest()

    while True:
        candidate = b''
        while len(candidate) < 32:
            v = hmac.new(k, v, hashlib.sha256).digest()
            candidate += v

        secret = bits2int(candidate)
        if 1 <= secret < SECP256K1_N:
            return secret

        k = hmac.new(k, v + b'\x00', hashlib.sha256).digest()
        v = hmac.new(k, v, hashlib.sha256).digest()

def secp256k1_sign_digest(digest):
    private_key = int(ECC256K1_PRIVATE_KEY_HEX, 16)
    if not 1 <= private_key < SECP256K1_N:
        raise ValueError('Invalid ECC private key')

    z = int.from_bytes(digest, 'big')

    while True:
        nonce = secrets.randbelow(SECP256K1_N - 1) + 1
        point = secp256k1_point_mul(nonce, (SECP256K1_GX, SECP256K1_GY))
        r = point[0] % SECP256K1_N
        if r == 0:
            continue

        s = (inverse_mod(nonce, SECP256K1_N) * (z + r * private_key)) % SECP256K1_N
        if s == 0:
            continue

        return int2octets(r) + int2octets(s)

def secp256k1_public_key_bytes():
    private_key = int(ECC256K1_PRIVATE_KEY_HEX, 16)
    if not 1 <= private_key < SECP256K1_N:
        raise ValueError('Invalid ECC private key')

    point = secp256k1_point_mul(private_key, (SECP256K1_GX, SECP256K1_GY))
    if point is None:
        raise ValueError('Invalid ECC public key')

    return int2octets(point[0]) + int2octets(point[1])

def secp256k1_verify_digest(public_key, digest, signature):
    if len(public_key) != SIGNATURE_SIZE or len(signature) != SIGNATURE_SIZE:
        return False

    x = int.from_bytes(public_key[:32], 'big')
    y = int.from_bytes(public_key[32:], 'big')
    r = int.from_bytes(signature[:32], 'big')
    s = int.from_bytes(signature[32:], 'big')

    if not (0 <= x < SECP256K1_P and 0 <= y < SECP256K1_P):
        return False

    if ((y * y - (x * x * x + 7)) % SECP256K1_P) != 0:
        return False

    if not (1 <= r < SECP256K1_N and 1 <= s < SECP256K1_N):
        return False

    z = int.from_bytes(digest, 'big')
    w = inverse_mod(s, SECP256K1_N)
    u1 = (z * w) % SECP256K1_N
    u2 = (r * w) % SECP256K1_N
    point_g = secp256k1_point_mul(u1, (SECP256K1_GX, SECP256K1_GY))
    point_q = secp256k1_point_mul(u2, (x, y))
    point = secp256k1_point_add(point_g, point_q)

    if point is None:
        return False

    return (point[0] % SECP256K1_N) == r


def read_hex_file(file_path):
    with open(file_path, 'r') as file:
        lines = file.readlines()

    global Size
    DATA = []
    for line in lines:
        # 去除行末的换行符
        line = line.strip()

        if line.startswith(':'):
            # 提取字节数和地址
            byte_count = int(line[1:3], 16)
            record_type = int(line[7:9], 16)
            # 只考虑数据记录类型
            if record_type == 0:
                for j in range(byte_count):
                    DATA.append(line[9+(j) * 2: 9 + ((j+1) * 2)])
                    Size = Size+1
    Size = Size - BOOT2_HEAR_SIZE_BYTE
    return DATA

def get_bin_size(file):
    binfile = open(file, 'rb')  # 打开二进制文件
    size = os.path.getsize(file)  # 获得文件大小
    binfile.close()
    return size

def get_bin_data(file):
    data = []
    binfile = open(file, 'rb')  # 打开二进制文件
    size = os.path.getsize(file)  # 获得文件大小
    for i in range(size):
        new_data = binfile.read(1)

        data.append(new_data.hex())
        # print((new_data.hex()))
    binfile.close()
    return data


def create_c_file(file):
    cfile = open(file, mode='w', encoding='utf-8')
    cfile.close()

def get_boot2_info(Header):
    global Flag,Version,LoadAddr
    #byte 0-3 Flag
    Flag = int(Header[0],16) | int(Header[1],16) << 8 | int(Header[2],16) << 16 | int(Header[3],16) << 24
    Version = int(Header[4],16) | int(Header[5],16) << 8 | int(Header[6],16) << 16 | int(Header[7],16) << 24
    LoadAddr = int(Header[16],16) | int(Header[17],16) << 8 | int(Header[18],16) << 16 | int(Header[19],16) << 24

def write_c_file_build_info(cfile):
    cfile.write('/******************************************************************************\n')
    cfile.write(
        '* Note: This content is used for SLC series chip Boot2 code loading and is not required if the chip category is non-boot2 mode.\n')
    cfile.write('* \n')
    cfile.write('* -----------------------------Build info--------------------------------------\n')
    cfile.write('* Boot2.hex Build Time       : [' + HexLastBuildTime  + '].\n')
    cfile.write('* Boot2.c   Build Time       : [' +
                time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()) + '].\n')
    cfile.write('* Boot2 Version              : [' + hex(Version) + '].\n')
    cfile.write('* After Build Python Version : [' + PYTHON_BOOT2_VERSION + '].\n')
    cfile.write('* -----------------------------------------------------------------------------\n')
    cfile.write('* \n')
    cfile.write(
        '* THIS CODE IS LIMITED TO GUIDE APPLICATION DEVELOPMENT AND INCREASE DEVELOPMENT SPEED.\n')
    cfile.write(
        '* AS A RESULT, SLC IS NOT RESPONSIBLE FOR ANY CLAIMS ARISING DIRECTLY OR INDIRECTLY FROM SUCH FIRMWARE AND/OR CONTENT.\n')
    cfile.write('* \n')
    cfile.write('* Copyright (c) 2025 SLC Semiconductor all rights reserved.\n')
    cfile.write('******************************************************************************/\n')
    cfile.write('\n\n')

def write_to_c_file(file, data, size):
    global HexLastBuildTime,Crc,Boot2HeaderCrc
    cfile = open(file, mode='w', encoding='utf-8')

    HeaderData = data[0:BOOT2_HEAR_SIZE_BYTE]

    CodeData = data[BOOT2_HEAR_SIZE_BYTE:Size + BOOT2_HEAR_SIZE_BYTE]
    get_boot2_info(HeaderData)

    # make header
    #byte 8 : 11 - Len
    Len_Temp = size & 0xff
    HeaderData[8] = hex(Len_Temp)[2:]
    Len_Temp = (size>>8) & 0xff
    HeaderData[9] = hex(Len_Temp)[2:]
    Len_Temp = (size >> 16) & 0xff
    HeaderData[10] = hex(Len_Temp)[2:]
    Len_Temp = (size >> 24) & 0xff
    HeaderData[11] = hex(Len_Temp)[2:]

    # byte 12 : 15 - CRC
    #print(CodeData)
    Boot2CodeCrc = crc32_ieee(CodeData)
    Crc = Boot2CodeCrc
    Len_Temp = Boot2CodeCrc & 0xff
    HeaderData[12] = hex(Len_Temp)[2:]
    Len_Temp = (Boot2CodeCrc >> 8) & 0xff
    HeaderData[13] = hex(Len_Temp)[2:]
    Len_Temp = (Boot2CodeCrc >> 16) & 0xff
    HeaderData[14] = hex(Len_Temp)[2:]
    Len_Temp = (Boot2CodeCrc >> 24) & 0xff
    HeaderData[15] = hex(Len_Temp)[2:]

    print("Boot2CodeCrc Value = " + hex(Boot2CodeCrc))
    #print(HeaderData)

    # 1. 将 CodeData (字符串列表) 转换为 bytes 对象以便哈希计算
    # 假设 CodeData 中的元素是类似 "1f", "ab" 这样的十六进制字符串
    code_bytes = bytes.fromhex(''.join(CodeData))
    # 2. 计算 SHA256
    sha256_digest = hashlib.sha256(code_bytes).digest()
    sha256_hash = sha256_digest.hex()
    # 3. 将计算出的 32 字节 (64 个字符) Hash 填充到 HeaderData 偏移 20 开始的位置
    # HeaderData[20] 到 HeaderData[51] 将被填充
    hash_idx = 0
    for i in range(HASH_OFFSET, HASH_OFFSET + HASH_SIZE):
        # 每次取两个字符（一个字节）
        byte_str = sha256_hash[hash_idx : hash_idx + 2]
        HeaderData[i] = byte_str
        hash_idx += 2

    print("Boot2Code SHA256 Hash = " + sha256_hash)

    signature = secp256k1_sign_digest(sha256_digest)
    public_key = secp256k1_public_key_bytes()
    signature_hash = signature.hex()
    sign_idx = 0
    for i in range(SIGNATURE_OFFSET, SIGNATURE_OFFSET + SIGNATURE_SIZE):
        HeaderData[i] = signature_hash[sign_idx : sign_idx + 2]
        sign_idx += 2

    print("ECC256K1 Public Key = " + public_key.hex())
    print("Boot2Code ECC256K1 Signature = " + signature_hash)
    if not secp256k1_verify_digest(public_key, sha256_digest, signature):
        raise ValueError('ECC256K1 local verify failed')
    print("Boot2Code ECC256K1 Local Verify = OK")

    HeaderWoCrc = HeaderData[0:BOOT2_HEAR_SIZE_BYTE - 4]
    #print(HeaderWoCrc)
    Boot2HeaderCrc = crc32_ieee(HeaderWoCrc)
    print("Boot2HeaderCrc Value = " + hex(Boot2HeaderCrc))
    #Boot2Header Crc
    Len_Temp = Boot2HeaderCrc & 0xff
    HeaderData[BOOT2_HEAR_SIZE_BYTE-4] = hex(Len_Temp)[2:]
    Len_Temp = (Boot2HeaderCrc >> 8) & 0xff
    HeaderData[BOOT2_HEAR_SIZE_BYTE-3] = hex(Len_Temp)[2:]
    Len_Temp = (Boot2HeaderCrc >> 16) & 0xff
    HeaderData[BOOT2_HEAR_SIZE_BYTE-2] = hex(Len_Temp)[2:]
    Len_Temp = (Boot2HeaderCrc >> 24) & 0xff
    HeaderData[BOOT2_HEAR_SIZE_BYTE-1] = hex(Len_Temp)[2:]

    write_c_file_build_info(cfile)

    #make code
    cfile.write('const unsigned char gau8Boot2Code[' + str(Size + BOOT2_HEAR_SIZE_BYTE) + '] __attribute__ ((section(".boot2.code"))) = { \n')
    counter = 0
    for hex_value in HeaderData:
        byte_value = "0x" + str(hex_value) + ", "
        cfile.write(byte_value + '\t')
        counter = counter + 1
        if ((counter % 16) == 0):
            cfile.write('\n')

    for hex_value in CodeData:
        byte_value = "0x" + str(hex_value) + ", "
        cfile.write(byte_value + '\t')
        counter = counter + 1
        if((counter % 16) == 0):
            cfile.write('\n')
    cfile.write('};\n')
    cfile.close()

if __name__ == '__main__':

    #Read bin & record data
    binfilepath = FILE_PATH + FILE_NAME
    print("File path and name:" + binfilepath)

    data = read_hex_file(binfilepath)
    modified_timestamp = os.path.getmtime(binfilepath)

    HexLastBuildTime = datetime.datetime.fromtimestamp(modified_timestamp)
    HexLastBuildTime = HexLastBuildTime.strftime('%Y-%m-%d %H:%M:%S')

    time.sleep(1)

    #Create C source & save data
    cfilepath = CREATE_FILE_PATH + CREATE_FILE_NAME
    write_to_c_file(cfilepath, data, Size)
    print("Boot2 Flag = " + hex(Flag))
    print("Boot2 Version = " + hex(Version))
    print("Boot2 LoadAddr = " + hex(LoadAddr))
    print("BIN size = ", Size)
