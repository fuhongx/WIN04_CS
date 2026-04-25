@echo off

set SCRIPT_DIR=%~dp0
set sct_file=%1

echo sct_file: %sct_file%

python "%SCRIPT_DIR%gen_sct_v5.py" "%sct_file%"
if %ERRORLEVEL% neq 0 (
    echo "genrate V5 sct file error"
    goto err
)

::succeed
exit /b 0

:err
exit /b 1
