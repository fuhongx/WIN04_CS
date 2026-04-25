import struct
import os
import time
import datetime
from tkinter.constants import FIRST

File_path = '../../../../library/'
File_name = 'win04_rom_lib.lib'

except_list = [
    '_',
    '__',
    'std',
    'main',
    'ckcpu_vsr_table',
    'Reset_Handler'
]

def make_except(data):
    for j in except_list:
        if (data[0:len(j)] == j):
            print('except:' + j)
            print(line[10:10 + len(j)])
            return False
    return  True

if __name__ == '__main__':
    writer = open(File_path +'win04_rom_gcc_lib.lib', 'w')

    modified_timestamp = os.path.getmtime(File_path + File_name)
    # HexLastBuildTime = time.ctime( modified_timestamp)
    HexLastBuildTime = datetime.datetime.fromtimestamp(modified_timestamp)
    HexLastBuildTime = HexLastBuildTime.strftime('%Y-%m-%d %H:%M:%S')

    # writer.write('; Lib Create time as ' + HexLastBuildTime +'\n\n')

    #Step 1, make all const func
    with open( File_path + File_name, 'r', encoding='utf-8') as file:
        for line in file:
            new_line = line.strip()

            if((line[0] == ';') | (line[0] == '#')):
                continue

            new_func = line[13:len(line) -1] + ' = ' + line[0:10] + ';' + '\n'
            writer.write(new_func)
            # print(new_func)
        #     if(len(line) > 10):
        #         # print(type(line))
        #         # print(len(line))
        #         #get name;
        #         if(line[9] == "<"):
        #             if(line[0] == '2'):
        #                 continue
        #             for i in range(len(line)):
        #                 if(line[i] == ">"):
        #                     #make except
        #                     if(make_except(line[10:i])):
        #                         new_func = line[10:i] + ' = ' + '0x' + line[0:8] + ';' + '\n'
        #                         writer.write(new_func)
        #                     # print(new_func)
        #             # print(line.strip())
        #
        # writer.write('\n\n; Bellow is Ram variable!\n')
        # # Step 2, make all ram func
        # with open(File_path + File_name, 'r', encoding='utf-8') as file:
        #     for line in file:
        #         new_line = line.strip()
        #         # print(line)
        #         if (len(line) > 10):
        #             # print(type(line))
        #             # print(len(line))
        #             # get name;
        #             if (line[9] == "<"):
        #                 if (line[0] != '2'):
        #                     continue
        #                 for i in range(len(line)):
        #                     if (line[i] == ">"):
        #                         if (make_except(line[10:i])):
        #                             new_func = line[10:i] + ' = ' + '0x' + line[
        #                                                                    0:8] + ';' + '\n'
        #                             writer.write(new_func)
        #                         # print(new_func)
