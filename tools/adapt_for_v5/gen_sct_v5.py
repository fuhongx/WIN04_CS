#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Enhanced V5 scatter generator with:
1. Auto-extract #include "header.h" from .sct template
2. Skip generation if output is up-to-date (incremental build)
"""

import re
import sys
import os
import time

def find_header_in_sct(sct_path):
    """从 .sct 文件中查找 #include "..." 并返回第一个匹配的 .h 路径"""
    sct_dir = os.path.dirname(os.path.abspath(sct_path))
    with open(sct_path, 'r', encoding='utf-8', errors='ignore') as f:
        for line in f:
            line = line.strip()
            # 匹配 #include "path/to/file.h"
            match = re.match(r'#include\s+"([^"]+\.h)"', line)
            if match:
                rel_path = match.group(1)
                abs_path = os.path.normpath(os.path.join(sct_dir, rel_path))
                if os.path.isfile(abs_path):
                    return abs_path
                else:
                    print(f"[WARN] Included header not found: {abs_path}")
    return None

def parse_c_header(header_path):
    macros = {}
    with open(header_path, 'r', encoding='utf-8', errors='ignore') as f:
        for line in f:
            line = re.split(r'//|/\*', line, maxsplit=1)[0].strip()
            if not line.startswith('#define'):
                continue

            match = re.match(r'#define\s+(\w+)\s+(.+)', line)
            if not match:
                continue

            name = match.group(1)
            value = match.group(2).strip()

            if '(' in name:
                continue

            expr = value
            for k, v in macros.items():
                expr = re.sub(r'\b' + re.escape(k) + r'\b', str(v), expr)

            try:
                result = eval(expr, {"__builtins__": {}}, {})
                if isinstance(result, int):
                    macros[name] = result
            except Exception:
                pass
    return macros

def replace_macros_in_line(line, macros):
    def replacer(match):
        word = match.group(0)
        if word in macros:
            return f"0x{macros[word]:X}"
        return word
    return re.sub(r'\b\w+\b', replacer, line)

def need_rebuild(template_file, header_file, output_file):
    """判断是否需要重新生成 output_file"""
    if not os.path.exists(output_file):
        return True

    out_mtime = os.path.getmtime(output_file)
    template_mtime = os.path.getmtime(template_file)
    header_mtime = os.path.getmtime(header_file)

    return template_mtime > out_mtime or header_mtime > out_mtime

def main():
    if len(sys.argv) not in (2, 4):
        print("Usage:")
        print("  python gen_sct_v5.py <template.sct>")  # 自动找 #include
        print("  python gen_sct_v5.py <template.sct> <header.h> <output.sct>")  # 手动指定
        sys.exit(1)

    if len(sys.argv) == 2:
        # 自动模式
        template_file = sys.argv[1]
        output_file = os.path.splitext(template_file)[0] + '_v5.sct'
        header_file = find_header_in_sct(template_file)
        if not header_file:
            print(f"[ERROR] No valid #include \"*.h\" found in {template_file}")
            sys.exit(1)
        print(f"[INFO] Auto-detected header: {header_file}")
    else:
        # 手动模式（兼容旧用法）
        template_file, header_file, output_file = sys.argv[1], sys.argv[2], sys.argv[3]

    # 检查文件是否存在
    for f, name in [(template_file, "template"), (header_file, "header")]:
        if not os.path.isfile(f):
            print(f"[ERROR] {name} file not found: {f}")
            sys.exit(1)

    # 增量构建判断
    if not need_rebuild(template_file, header_file, output_file):
        print(f"[SKIP] Output is up-to-date: {output_file}")
        return

    # 解析宏
    macros = parse_c_header(header_file)
    print(f"[INFO] Parsed {len(macros)} numeric macros from {header_file}")

    # 生成输出
    with open(template_file, 'r', encoding='utf-8', errors='ignore') as fin, \
         open(output_file, 'w', encoding='utf-8') as fout:

        for line in fin:
            stripped = line.strip()
            if stripped.startswith('#!') or stripped.startswith('#include'):
                continue
            new_line = replace_macros_in_line(line, macros)
            fout.write(new_line)

    print(f"[SUCCESS] Generated: {output_file}")

if __name__ == '__main__':
    main()