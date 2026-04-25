```
WIN04_SDK/
│
├── library/                                ← 工程所需库文件
│   ├── boot2.c                             ← boot2 code(供app层编译使用)
│   ├── win04_rom_lib.lib                   ← rom_lib静态库，包含底层驱动，代码存放在rom
│   ├── win04_rom_lib.hex                   ← rom_lib的16进制文件（release版本不发布该文件）
│   └── win04_rom_gcc_lib.lib               ← rom_lib静态库(GCC版，release版本不发布)
│
├── project/                                ← keil工程主目录及入口
│   ├── rf_test/                            ← 测试工程目录（release版本不发布，无需关注）
│   ├── coremark/                           ← coremark跑分工程（release版本不发布，无需关注）
│   ├── library/                            ← rom_lib库的工程目录以及boot2相关工程（release版本不发布，无需关注）
│   └── demo/
│       ├── gcc_demo/                       ← GCC工程实例（release版本不发布，无需关注）
│       └── soc_test/                       ← 各外设验证用例的demo，应用时可参考该工程的用例使用驱动接口
│           ├── config/                     ← 工程配置目录
│           │   └── app_cfg.h               ← 工程配置宏定义头文件
│           │
│           ├── Listings/                   ← 编译产物，无需关注
│           ├── Objects/                    ← 编译产物（asm、map、axf所在目录），无需关注
│           │
│           ├── boot2.bin                   ← boot2代码的bin
│           ├── firmware.bin                ← app层的固件
│           ├── flash.bin                   ← boot2.bin和firmware.bin合一起的bin，仅内部调试用
│           │
│           ├── format_firmware_header.py   ← 生成firmware.bin头信息的脚本
│           ├── project_maker.bat           ← KEIL编译后执行的脚本，用来生成axf、bin等，最后调用头信息生成脚本
│           │
│           ├── soc_test.sct                ← 工层的链接脚本，memmap定义
│           ├── main.c                      ← 工程的主函数源文件
│           └── soc_test.uvprojx            ← 工程启动文件
│
├── source/                                 ← 工程源码目录
│   ├── board/                              ← BSP板级支持包
│   │   └── win04/                          ← win04的EVB板支持包，包含必要的初始化，IOMUX排布等
│   │
│   ├── component/                          ← 组件
│   │   ├── error_def/                      ← （必选）错误类型定义，当前不可删除
│   │   ├── utility/                        ← （必选）常用工具函数
│   │   │
│   │   ├── crypto/                         ← （可选）软件加密算法，如ECC、AES、SHA等
│   │   ├── private_spi/                    ← （可选）私有SPI协议，提供给PHY only和RF only使用
│   │   ├── debug/                          ← （可选）串口日志的uart初始化重定向，以及hardfault的堆栈dump
│   │   └── ntshell/                        ← （可选）uart console的终端命令行控件，具体见目录内README.md
│   │
│   ├── device/                             ← 芯片基本信息：core相关头文件、寄存器
│   │   ├── ARMCM0plus/                     ← CM0+基础头信息目录
│   │   │   ├── Include/                    ← CM0+启动相关标准头文件
│   │   │   ├── Source/                     ← CM0+的 KEIL 环境启动文件
│   │   │   └── startup_gcc/                ← CM0+的 GCC 环境启动文件（release版本不发布）
│   │   │
│   │   ├── CMSIS_Core/
│   │   │   └── Include/                    ← CM0+通用的标准头文件
│   │   │
│   │   ├── reg_device/                     ← 芯片寄存器头文件选择控制
│   │   │
│   │   └── win04/                          ← win04芯片寄存器地址定义和版本信息
│   │       ├── reg_win04.h                 ← win04芯片寄存器定义头文件
│   │       ├── version.c                   ← boot2和firmware的版本信息
│   │       └── version.h                   ← boot2和firmware的版本信息头文件
│   │
│   ├── drivers/                            ← 底层驱动代码，应用层不可更改
│   │   └── win04/                          ← win04的底层驱动代码
│   │       ├── hw_rom/                     ← 存放在rom的驱动源码
│   │       │   ├── inc/                    ← 头文件
│   │       │   └── src/                    ← 源码
│   │       │
│   │       ├── hw_flash/                   ← 存放在flash的驱动源码
│   │       │   ├── inc/                    ← 头文件
│   │       │   └── src/                    ← 源码
│   │       │
│   │       ├── patch/                      ← rom内驱动的bugfix
│   │       │   ├── inc/                    ← 头文件
│   │       │   └── src/                    ← 源码
│   │       │
│   │       └── register/                   ← 各外设驱动的寄存器排布及定义
│   │
│   ├── hal/                                ← 硬件抽象层，提供给应用层的驱动接口
│   │   ├── inc/                            ← 头文件
│   │   └── src/                            ← 源码
│   │
│   ├── mem_cfg/                            ← 芯片的内存相关头文件
│   │   ├── flash/                          ← 不同厂商flash的头文件定义，如内存大小，控制命令
│   │   └── win04/
│   │       ├── compile.h                   ← 编译器相关以及section等的宏定义
│   │       └── win04_mem_cfg.h             ← win04的内存信息排布
│   │
│   └── project_source/                     ← 应用层源码所在目录
│       ├── soc_test/                       ← soc_test工程的测试用例源码demo
│       ├── rf_test/                        ← phy/rf硬测工程的项目源码（release版本不发布）
│       └── library/
│           ├── boot2/                      ← boot2的项目源码
│           └── bootloader/                 ← rom内bootloader的项目源码（release版本不发布）
│
├── third_party/                            ← 第三方库
│   └── CoreMark/                           ← coremark的源码
│
├── tools/                                  ← 项目脚本工具及示例demo
│   ├── adapt_for_v5/                       ← 适配v5工程，生成v5工程所需sct的脚本
│   ├── flash_algorithm/                    ← keil工程适配flash烧录算法的工程（release版本不发布）
│   ├── jlink_cmd/                          ← jlink连接脚本示例（release版本不发布）
│   ├── format_boot2/                       ← boot2工程编译后生成bin的脚本及boot2.c的脚本
│   ├── format_firmware/                    ← 在bin文件开头增加头信息的脚本
│   ├── make_gcc_lib/                       ← 适配GCC工程，生成GCC环境所需rom_lib库的脚本（release版本不发布）
│   ├── get_ila_data.py                     ← ILA上位机工具源码（release版本不发布，仅底软开发使用）
│   └── process_cali_data.py                ← 根据Excel表生成校准数据表的脚本（release版本不发布，仅底软开发使用）
│
└── FPGA_fast_project.uvmpw                 ← 快速启动的工程文件（包含rom_lib（release版本不包含）、boot2、soc_test）