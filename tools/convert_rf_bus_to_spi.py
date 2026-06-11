#!/usr/bin/env python3
"""Convert RF bus register access (RF_PMU-> etc.) to SPI helper calls."""

import re
import sys
from pathlib import Path

REG_RF_H = Path(__file__).resolve().parents[1] / "source/drivers/win04/register/reg_rf.h"

MODULES = {
    "RF_PMU": ("rf_pmu_handle_t", "SLC_RF_SPI_ADDR_PMU"),
    "RF_CTRL": ("rf_ctrl_handle_t", "SLC_RF_SPI_ADDR_CTRL"),
    "RF_AGC": ("rf_agc_handle_t", "SLC_RF_SPI_ADDR_AGC"),
    "RF_PLL": ("rf_pll_handle_t", "SLC_RF_SPI_ADDR_PLL"),
    "RF_DFE": ("rf_dfe_handle_t", "SLC_RF_SPI_ADDR_DFE"),
}

ACCESS_RE = re.compile(
    r"^(?P<indent>\s*)(?P<prefix>.*?)(?P<mod>RF_(?:PMU|CTRL|AGC|PLL|DFE))->(?P<reg>[A-Z0-9_]+)"
)


def parse_reg_offsets(path: Path) -> dict[str, dict[str, str]]:
    text = path.read_text(encoding="utf-8", errors="replace")
    offsets: dict[str, dict[str, str]] = {mod: {} for mod in MODULES}

    struct_re = re.compile(
        r"typedef struct\s*\{(?P<body>.*?)\}\s*(?P<name>rf_\w+_handle_t)\s*;",
        re.DOTALL,
    )
    field_re = re.compile(
        r"__IOM\s+uint32_t\s+(?P<field>[A-Z0-9_]+)\s*;\s*//\s*(?P<off>0x[0-9A-Fa-f]+)"
    )

    struct_to_mod = {v[0]: k for k, v in MODULES.items()}

    for match in struct_re.finditer(text):
        struct_name = match.group("name")
        if struct_name not in struct_to_mod:
            continue
        mod = struct_to_mod[struct_name]
        for field_match in field_re.finditer(match.group("body")):
            offsets[mod][field_match.group("field")] = field_match.group("off")

    return offsets


def spi_addr(mod: str, reg: str, offsets: dict[str, dict[str, str]]) -> str:
    off = offsets[mod].get(reg)
    if off is None:
        raise KeyError(f"unknown register {mod}->{reg}")
    macro = MODULES[mod][1]
    return f"{macro}({off})"


def convert_line(line: str, offsets: dict[str, dict[str, str]]) -> str:
    m = ACCESS_RE.match(line.rstrip("\n"))
    if not m:
        return line

    indent = m.group("indent")
    prefix = m.group("prefix")
    mod = m.group("mod")
    reg = m.group("reg")
    rest = line[m.end() :].rstrip("\n")
    addr = spi_addr(mod, reg, offsets)

    # return (RF_PMU->INT_FLAG & MASK);
    if prefix.strip().startswith("return"):
        if "&" in rest and "|=" not in rest and "=" not in rest.replace("==", ""):
            expr = rest.strip().rstrip(";")
            if expr.startswith("&"):
                expr = expr[1:].strip()
            return f"{indent}return (slc_rf_spi_read32_cmd({addr}) & {expr};\n"
        if rest.strip().startswith(";"):
            return f"{indent}return slc_rf_spi_read32_cmd({addr});\n"

    # assignment read: val = RF_PMU->REG;
    assign_read = re.match(r"\s*=\s*(.+?);\s*$", rest)
    if assign_read and prefix.strip() and not prefix.rstrip().endswith(("&", "|", "^", "+", "-", "*", "/")):
        rhs = assign_read.group(1).strip()
        if rhs == reg or rhs.endswith(f"{mod}->{reg}"):
            lhs = prefix.rstrip()
            return f"{indent}{lhs} = slc_rf_spi_read32_cmd({addr});\n"

    # read in expression: (RF_PMU->REG & MASK) >> SHIFT
    if rest.strip() and not rest.lstrip().startswith(("|=", "&=", "=", "+=")):
        expr = f"{mod}->{reg}{rest.rstrip()}"
        new_expr = expr.replace(f"{mod}->{reg}", f"slc_rf_spi_read32_cmd({addr})", 1)
        return f"{indent}{prefix}{new_expr}"

    # RF_PMU->REG |= MASK;
    m_or = re.match(r"\s*\|=\s*(.+?);\s*$", rest)
    if m_or:
        return f"{indent}slc_rf_spi_reg_or_mask({addr}, {m_or.group(1).strip()});\n"

    # RF_PMU->REG &= ~MASK;
    m_clr = re.match(r"\s*&=\s*~(.+?);\s*$", rest)
    if m_clr:
        return f"{indent}slc_rf_spi_reg_clr_mask({addr}, {m_clr.group(1).strip()});\n"

    # RF_PMU->REG &= MASK;  (rare, without ~)
    m_and = re.match(r"\s*&=\s*(.+?);\s*$", rest)
    if m_and:
        mask = m_and.group(1).strip()
        return (
            f"{indent}slc_rf_spi_reg_update({addr}, ~({mask}), "
            f"slc_rf_spi_read32_cmd({addr}) & ({mask}));\n"
        )

    # RF_PMU->REG = val;
    m_set = re.match(r"\s*=\s*(.+?);\s*$", rest)
    if m_set:
        return f"{indent}slc_rf_spi_write32_cmd({addr}, {m_set.group(1).strip()});\n"

    raise ValueError(f"unhandled access pattern: {line.rstrip()}")


def merge_reg_updates(lines: list[str]) -> list[str]:
    """Merge consecutive clr_mask + or_mask/reg_update on same register."""
    out: list[str] = []
    i = 0
    clr_re = re.compile(
        r"^(?P<indent>\s*)slc_rf_spi_reg_clr_mask\((?P<addr>[^,]+),\s*(?P<mask>.+)\);\s*$"
    )
    or_re = re.compile(
        r"^(?P<indent>\s*)slc_rf_spi_reg_or_mask\((?P<addr>[^,]+),\s*(?P<bits>.+)\);\s*$"
    )
    while i < len(lines):
        m_clr = clr_re.match(lines[i].rstrip("\n"))
        if m_clr and i + 1 < len(lines):
            m_or = or_re.match(lines[i + 1].rstrip("\n"))
            if (
                m_or
                and m_clr.group("indent") == m_or.group("indent")
                and m_clr.group("addr") == m_or.group("addr")
            ):
                out.append(
                    f"{m_clr.group('indent')}slc_rf_spi_reg_update("
                    f"{m_clr.group('addr')}, {m_clr.group('mask')}, "
                    f"{m_or.group('bits')});\n"
                )
                i += 2
                continue
        out.append(lines[i])
        i += 1
    return out


def convert_file(path: Path, offsets: dict[str, dict[str, str]]) -> int:
    text = path.read_text(encoding="utf-8", errors="replace")
    lines = text.splitlines(keepends=True)
    converted = 0
    new_lines: list[str] = []

    for line in lines:
        if "RF_PMU->" in line or "RF_CTRL->" in line or "RF_AGC->" in line or "RF_PLL->" in line or "RF_DFE->" in line:
            try:
                new_line = convert_line(line, offsets)
                if new_line != line:
                    converted += 1
                new_lines.append(new_line)
            except (KeyError, ValueError) as exc:
                print(f"{path}:{exc}: {line.rstrip()}", file=sys.stderr)
                new_lines.append(line)
        else:
            new_lines.append(line)

    new_lines = merge_reg_updates(new_lines)

    if converted:
        content = "".join(new_lines)
        if "slc_private_spi_frame.h" not in content:
            include_line = '#include "slc_private_spi_frame.h"\n'
            inserted = False
            final_lines: list[str] = []
            for line in new_lines:
                final_lines.append(line)
                if not inserted and line.startswith("#include"):
                    final_lines.append(include_line)
                    inserted = True
            if not inserted:
                final_lines.insert(0, include_line)
            new_lines = final_lines

        path.write_text("".join(new_lines), encoding="utf-8", newline="\n")

    return converted


def main() -> int:
    root = Path(__file__).resolve().parents[1]
    offsets = parse_reg_offsets(REG_RF_H)
    files = [
        root / "source/drivers/win04/hw_flash/src/slc_rf.c",
        root / "source/drivers/win04/hw_flash/src/slc_cali.c",
        root / "source/drivers/win04/hw_flash/src/slc_phy_trx.c",
    ]

    total = 0
    for file_path in files:
        if not file_path.exists():
            print(f"skip missing: {file_path}", file=sys.stderr)
            continue
        count = convert_file(file_path, offsets)
        print(f"{file_path.name}: converted {count} lines")
        total += count

    print(f"total converted: {total}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
