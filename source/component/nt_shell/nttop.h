#ifndef __NTTOP_H__
#define __NTTOP_H__

typedef int (*USRCMDFUNC)(int argc, char **argv);

#define NT_SHELL_CMDLIST_NUMBER         16  // 最大支持的cmd数
#define NT_SHELL_CMD_WIDTH              16  // 单条cmd的字符长度（不包括参数）
#define NT_SHELL_CMD_DESC_WIDTH         64  // help中显示的帮助信息长度

int ntshell_top_init(void);
void ntshell_top_run_once(void);

int ntshell_cmd_register(char *cmd, char *descriptor, USRCMDFUNC func);
#endif //_NTTOP_H_
