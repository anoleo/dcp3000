#ifndef	__CMD_LINE_H__
#define	__CMD_LINE_H__


typedef int (*cmdline_func)(int, char **);

void regist_internal_command(char *name, cmdline_func func, char *doc);
void init_cmdline(char *program_name);
void start_cmdline(char *prompt);
void exit_cmdline(void);

#endif
