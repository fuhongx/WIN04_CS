@echo off

set AXF_FILE=%1
set AXF_FILE=%AXF_FILE:"=%

set FROMELF=%2

set AXF_SUFFIX=%AXF_FILE:~-4%
set FROMELF_SUFFIX=%FROMELF:"=%
set FROMELF_SUFFIX=%FROMELF_SUFFIX:~-11%

if "%AXF_SUFFIX%" neq ".axf" (
    if "%AXF_SUFFIX%" neq ".AXF" (
        echo "Axf file error!"
        goto PARAM_ERR
    )
)

if "%FROMELF_SUFFIX%" neq "fromelf.exe" (
    echo "fromelf file error"
    goto PARAM_ERR
)

set PROJECT_NAME=%AXF_FILE:~0,-4%

set BIN_FILE=%PROJECT_NAME%.bin
set ASM_FILE=%PROJECT_NAME%.asm

::generate bin
::fromelf.exe --text -c -o "$L@L.asm"  "!L"
%FROMELF% --bin -o "%BIN_FILE%" "%AXF_FILE%"
if %ERRORLEVEL% neq 0 (
    echo "Make bin error"
    goto PROC_ERR
)

::generate asm
%FROMELF% --text -c -o "%ASM_FILE%"  "%AXF_FILE%"
if %ERRORLEVEL% neq 0 (
    echo "Make asm error"
    goto PROC_ERR
)

python .\lib_gen.py .\Objects\rom_lib.lib ..\..\..\..\library\win04_rom_lib.lib
if %ERRORLEVEL% neq 0 (
    echo "Make lib error"
    goto PROC_ERR
) else (
    echo "Make lib success!"
)

del /q .\Objects\rom_lib.lib
	echo "Delete rom_lib.lib success!"

copy .\Objects\rom_lib.hex ..\..\..\..\library\win04_rom_lib.hex

python ..\..\..\..\tools\make_gcc_lib\win04_gcc_lib_make.py
	echo "Make Gcc lib success!"

::succeed
exit /b 0

:PARAM_ERR
exit /b 1

:PROC_ERR
exit /b 2
