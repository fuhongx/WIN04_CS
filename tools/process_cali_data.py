import pandas as pd
import sys

def binary_to_int(binary_str):
    """将二进制字符串转换为整数"""
    try:
        if pd.isna(binary_str):
            return 0
            
        # 确保转换为字符串并去除所有空白字符
        binary_str = str(binary_str).strip()
        
        # 如果是空字符串，返回0
        if not binary_str:
            return 0
            
        # 移除可能的前导零（保留至少一位）
        binary_str = binary_str.lstrip('0') or '0'
        
        # 验证是否为有效的二进制字符串
        if not all(c in '01' for c in binary_str):
            print(f"Warning: Invalid binary string: {binary_str}")
            return 0
            
        # 转换为整数
        return int(binary_str, 2)
        
    except Exception as e:
        print(f"Error converting binary string: {binary_str}, Error: {str(e)}")
        return 0

def excel_to_c_array(excel_file, sheet_name=0):
    try:
        # 读取Excel文件，将所有列作为字符串读取
        df = pd.read_excel(excel_file, sheet_name=sheet_name, dtype=str)
        
        # 生成C代码
        c_code = []
        c_code.append("/* Auto-generated calibration table */\n")
        c_code.append("typedef struct {")
        c_code.append("    uint16_t decision_threshold;")
        c_code.append("    uint8_t rxabb_cbpf_ctrim;  // 7-bit binary")
        c_code.append("    uint8_t lpf_ctrim;")
        c_code.append("    uint8_t tia;")
        c_code.append("    uint8_t freq_doubler;")
        c_code.append("    uint8_t lo_ppf;")
        c_code.append("    uint8_t pll_dac_lpf;")
        c_code.append("} qmx_rccali_t;\n")
        
        c_code.append("static const qmx_rccali_t calib_table[] = {")
        
        # 处理每一行数据
        for index, row in df.iterrows():
            try:
                # 处理 Decision Thershold
                decision = 0 if pd.isna(row['Decision Thershold']) or row['Decision Thershold'] == '' else int(row['Decision Thershold'])
                
                # 处理二进制数据
                rxabb_val = binary_to_int(row['RXABB_CBPF_CTRIM[6:0]'])
                lpf_val = binary_to_int(row['LPF_CTRIM'])
                tia_val = binary_to_int(row['TIA'])
                freq_val = binary_to_int(row['Freq Doubler'])
                lo_val = binary_to_int(row['LO_PPF'])
                pll_val = binary_to_int(row['2PPLL_DAC_LPF'])
                
                # 格式化输出
                values = [
                    f"{decision:4d}",
                    f"{rxabb_val:3d} /* 0b{rxabb_val:08b} */",
                    f"{lpf_val:3d} /* 0b{lpf_val:08b} */",
                    f"{tia_val:3d} /* 0b{tia_val:08b} */",
                    f"{freq_val:3d} /* 0b{freq_val:08b} */",
                    f"{lo_val:3d} /* 0b{lo_val:08b} */",
                    f"{pll_val:3d} /* 0b{pll_val:08b} */"
                ]
                
                c_code.append(f"    {{{', '.join(values)}}},")
                
            except Exception as e:
                print(f"Warning: Skipping row {index + 2} due to error: {str(e)}")
                continue
        
        c_code.append("};\n")
        c_code.append(f"#define CALIB_TABLE_SIZE (sizeof(calib_table) / sizeof(calib_table[0]))")
        
        return '\n'.join(c_code)
        
    except Exception as e:
        print(f"Error: {str(e)}")
        return None

def main():
    if len(sys.argv) != 2:
        print("Usage: python excel_to_c.py <excel_file>")
        return
    
    excel_file = sys.argv[1]
    c_code = excel_to_c_array(excel_file)
    
    if c_code:
        # 保存到文件
        output_file = excel_file.rsplit('.', 1)[0] + '_calib.c'
        with open(output_file, 'w') as f:
            f.write(c_code)
        print(f"Generated C code saved to: {output_file}")

if __name__ == "__main__":
    main()
