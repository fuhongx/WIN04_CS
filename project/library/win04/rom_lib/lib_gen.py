import sys
import re

def replace_word(text, old_word, new_word):
    position = text.find(old_word)
    print("Old word is : ", position)
    new_text = text.replace(old_word, new_word)
    print(new_text)
    return new_text

""" 
地址表，移除符号地址在表中指定区域的符号
第一个地址表示起始地址（包含），第二个地址表示结束地址（包含），
例：
    (0x400, 0x400)表示移除地址为0x00000400的符号
    (0x2007d000, 0x2007f000)表示移除0x2007d000~0x2007f000地址区间内的所有符号
"""
range_list = (
    (0x00000000, 0x00000201),
    (0x20004200, 0x200047ff),
)

"""
正则表达式表，移除符号名称满足正则表达式的符号，需要学习正则表达式规则
例:
    "^SystemInit$"表示移除名称为"SystemInit"的符号
    "SystemInit"表示移除名称包含"SystemInit"的所有符号(如"SystemInit"、 "SubSystemInit2"等)
    "^__Vector"表示移除名称以"__Vectors"作为开头的符号(如"__Vector"、"__Vector2"等)
    "\\d{4}-\\d{1,2}-\\d{1,2}"表示移除名称中包含类似"2021-5-31"于这样格式字符串的符号
"""
regex_list = (
    "^SystemInit$",
    r"^__scatterload_\S+$",
    "^main$",
    "^puts$",
    "^__ARM_use_no_argv$",
    "^idle_task_processor$",
    "^__stdout$",
    r"^__\S*printf\S*$",
    r"^__aeabi_\S+$",
    r"^_ll_\S+$",
    r"^_memset\S+$",
    r"^__I\$use\$fp$",
    r"^_double_\S+$",
    r"^_float_\S+$",
    r"^boot_\S+$",
    r"^firmware_\S+$",
    "^__ARM_clz$",
    "^__heap",
    "^__stack_limit$",
    "^__initial_sp$",
    "fputc",
    "malloc",
    "free",
    "calloc",
    r"debug\S+$",
    r"HardFault\S+$",
    "printf",
)


def check_range(addr_str):
    global range_list

    addr = int(addr_str, 16)
    for range_element in range_list:
        start = range_element[0]
        end = range_element[1]
        # print("range(0x%x 0x%x) addr:0x%x" % (start, end, addr))
        # if (start <= addr) and (addr <= end):
        if start <= addr <= end:
            return True

    return False
    pass


def check_regex(symbol_str):
    global regex_list
    for regex_element in regex_list:
        # print("regex: %s %s" % (regex_element, symbol_str))
        if not (re.search(regex_element, symbol_str) is None):
            return True

    return False
    pass


def is_removable(line):
    """拆分该行"""
    elements = line.split(" ")
    # print(elements)

    """检查该行格式，类似于：
    0x00000000 D __Vectors"""
    if (len(elements) != 3) or (len(elements[0]) != 10) or (not elements[0].startswith("0x")):
        return False

    if check_range(elements[0]):
        return True

    if check_regex(elements[2]):
        return True

    return False
    pass


def process(src_file_name, dst_file_name):
    if src_file_name == dst_file_name:
        print("param err!")
        return 2

    f_src = None
    f_dst = None
    try:
        f_src = open(src_file_name, mode="r", encoding="utf-8")
        f_dst = open(dst_file_name, mode="w", encoding="utf-8")

        while True:
            line = f_src.readline()

            if line is None or 0 == len(line):
                break

            if is_removable(line.replace("\n", "")):
                # line = ";" + line
                line = ""

            f_dst.write(line)
    except Exception as e:
        print(e)
        return 2
    finally:
        if f_src is not None:
            f_src.close()
        if f_dst is not None:
            f_dst.close()
    return 0
    pass




if __name__ == '__main__':
    # # argv[1] : input file, or name
    # # argv[2] : Out file
    # print("In python process main!!\n")
    # print(sys.argv[1])
    # New_obj_name = sys.argv[1]
    #
    # New_obj_name = replace_word(New_obj_name, ".bin", ".lib")
    # print(New_obj_name)
    #
    # open(New_obj_name)
    # file = open(New_obj_name, 'r')
    #
    # i =0
    # while 1:
    #     print("Line i = " ,i)
    #     print(file.readline(i))
    #     i=i+1
    #
    # sys.exit(process(sys.argv[1], sys.argv[2]))

    # Lib = './Objects/rom_lib.lib'
    # Creat = '..\..\..\..\library\win04_rom_lib.lib'


    # argv[1] : input file
    # argv[2] : output file
    if len(sys.argv) != 3:
        print("argv err!")
        sys.exit(1)

    if sys.argv[1] == sys.argv[2]:
        print("argv err!")
        sys.exit(1)
    #
    sys.exit(process(sys.argv[1], sys.argv[2]))
    # sys.exit(process(Lib, Creat))
    pass


