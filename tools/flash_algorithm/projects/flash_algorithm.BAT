SET PATH=D:\KEIL_MDK5\ARM\ARMCLANG\Bin;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Windows\System32\OpenSSH\;C:\Users\zhangsenjia\AppData\Local\Programs\Python\Python313;C:\Users\zhangsenjia\AppData\Local\Programs\Python\Python313\Scripts;C:\Program Files\TortoiseSVN\bin;C:\Program Files\Git\cmd;C:\Program Files (x86)\GNU Arm Embedded Toolchain\10 2021.10\bin;C:\Users\zhangsenjia\AppData\Local\Programs\Python\Launcher\;C:\Program Files (x86)\Microsoft\Edge\Application;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Windows\System32\OpenSSH\;C:\Users\zhangsenjia\AppData\Local\Microsoft\WindowsApps;C:\Program Files (x86)\7-Zip;C:\Users\zhangsenjia\AppData\Local\Programs\Python\Python313;C:\Users\zhangsenjia\AppData\Local\Programs\Python\Python313\Scripts;D:\WIN04\tools\mingw64\bin;C:\Program Files\SEGGER\JLink;D:\softwares\PyCharm 2025.2\bin
SET CPU_TYPE=ARMCM0P
SET CPU_VENDOR=ARM
SET UV2_TARGET=flash_algorithm
SET CPU_CLOCK=0x00B71B00
"D:\KEIL_MDK5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\hw_sysctrl.__i"
"D:\KEIL_MDK5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\hw_gpio.__i"
"D:\KEIL_MDK5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\hw_flash.__i"
"D:\KEIL_MDK5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\hw_uart.__i"
"D:\KEIL_MDK5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\debug.__i"
"D:\KEIL_MDK5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\utility.__i"
"D:\KEIL_MDK5\ARM\ARMCLANG\Bin\ArmAsm" --Via ".\objects\utility_1._ia"
"D:\KEIL_MDK5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\flash_prog.__i"
"D:\KEIL_MDK5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\flashdev.__i"
"D:\KEIL_MDK5\ARM\ARMCLANG\Bin\ArmLink" --Via ".\Objects\Algorithm.lnp"
"D:\KEIL_MDK5\ARM\ARMCLANG\Bin\fromelf.exe" ".\Objects\Algorithm.axf" --i32combined --output ".\Objects\Algorithm.hex"
cmd.exe /C copy "Objects\Algorithm.axf" ".\WIN04_Flash_Algorithm.FLM"
