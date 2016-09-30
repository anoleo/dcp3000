
#include <sys/types.h>
#include <sys/file.h>

#include <sys/stat.h>
#include <dirent.h>

#include <unistd.h>

#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

#include <string.h>

#include <stdlib.h>

#include <time.h>

#include "readline/readline.h"
#include "readline/history.h"
#include "readline/xmalloc.h"

//#include "readline/posixstat.h"
#include "cmdline.h"
#include "../utils/queue.h"
#include "../utils/hashlib.h"

#define USE_HASH_TABLE

/* A structure which contains information on the commands this program
   can understand. */

typedef struct {
  char *name;			/* User printable name of the function. */
  cmdline_func func;		/* Function to call to do the job. */
  char *doc;			/* Documentation for this function.  */
} COMMAND;

static HASH_TABLE *cmd_table;


typedef struct {
	uid_t uid, euid;
	gid_t gid, egid;
	char *progname;
	char *homedir;
	HASH_TABLE *cmd_table;
	int runflag;
	int findenv;
	list_st *filelist;
	char *cwd;
	char prompt[256];
}cmdline_st;

static cmdline_st cmdline;

static void set_prompt_prefix(char *prefix)
{
	sprintf(cmdline.prompt, "%s@%s:", prefix, cmdline.progname);
	cmdline.cwd = &cmdline.prompt[strlen(cmdline.prompt)];
	printf("cmdline prompt : %s \n", cmdline.prompt);
}

static void update_prompt_cwd()
{
	char *cwd = cmdline.cwd;
	if(strcmp(getcwd(cwd, 200), cmdline.homedir) == 0){
		sprintf(cwd, "~# ");
	}else{
		sprintf(cwd+strlen(cwd), "# ");
	}
	
	//printf("cmdline cwd : %s \n", cmdline.cwd);
}



char *
dupstr (char *s)
{
  char *r;

  r = xmalloc (strlen (s) + 1);
  strcpy (r, s);
  return (r);
}

/* Look up NAME as the name of a command, and return a pointer to that
   command.  Return a NULL pointer if NAME isn't a command name. */
COMMAND *
find_command (char *name)
{
	BUCKET_CONTENTS *hash_item = hash_search(name, cmdline.cmd_table, 0);
	if(hash_item)
		return (COMMAND *)hash_item->data;
	else
		return NULL;
}


/* Strip whitespace from the start and end of STRING.  Return a pointer
   into STRING. */
char *
stripwhite (char *string)
{
  register char *s, *t;

  for (s = string; whitespace (*s); s++)
    ;
    
  if (*s == 0)
    return (s);

  t = s + strlen (s) - 1;
  while (t > s && whitespace (*t))
    t--;
  *++t = '\0';

  return s;
}

/* **************************************************************** */
/*                                                                  */
/*                  Interface to Readline Completion                */
/*                                                                  */
/* **************************************************************** */
static int traverse_exec_files(list_st *plist, const char *dir, const char *prefix)
{
	char name[256];
	struct dirent *dp;
	DIR *dfd;
	int findit = 0;

	struct stat stbuf;

	if (stat(dir, &stbuf) == -1) {
		fprintf(stderr, "stat: can't access %s\n", dir);
		return -1;
	}
	if ((stbuf.st_mode & S_IFMT) != S_IFDIR){
		return -2;
	}

	if ((dfd = opendir(dir)) == NULL) {
		fprintf(stderr, "dirwalk: can't open %s\n", dir);
		return -3;
	}
	
	while ((dp = readdir(dfd)) != NULL) {

		if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
			continue;    /* skip self and parent */
		
		//printf("traverse_dir : dp->d_name :  %s\n", dp->d_name);

		if (strncmp (dp->d_name, prefix, strlen(prefix)) == 0){

			sprintf(name, "%s/%s", dir, dp->d_name);

			//printf("traverse_dir : %s\n", name);

			if (stat(name, &stbuf) == -1) {
				fprintf(stderr, "fsize: can't access %s\n", name);
				continue;
			}

			findit = 0;

			//printf("file %s, uid : %d, gid : %d, stmode : %#o \n", dp->d_name, stbuf.st_uid, stbuf.st_gid, stbuf.st_mode);
			
			if((cmdline.euid == stbuf.st_uid) && (S_IXUSR & stbuf.st_mode)){
				findit = 1;
			}else if((cmdline.egid == stbuf.st_gid) && (S_IXGRP & stbuf.st_mode)){
				findit = 1;
			}else if((cmdline.euid != stbuf.st_uid) && (cmdline.egid != stbuf.st_gid)){
				if((S_IXOTH & stbuf.st_mode)){
					findit = 1;
				}
			}

			if (findit){
				//printf(" find file :  %s\n", name); 
				list_add(plist, dp->d_name, 0);
			}
			
		}
	}
	
	closedir(dfd);

	return 0;
}

char *
internal_command_generator (const char *text, int state)
{
	static BUCKET_CONTENTS *item = NULL;
	static int len, bucket_idx;
	int flag = 0;
	char *name;

  /* If this is a new word to complete, initialize now.  This includes
     saving the length of TEXT for efficiency, and initializing the index
     variable to 0. */
	if (!state){
		cmdline.findenv = 0;
		bucket_idx = 0;
		item = hash_items (0, cmdline.cmd_table);
		len = strlen(text);
	}


	//printf("command_generator : %s, %d \n", text, state);

  /* Return the next name which partially matches from the command list. */
	for(;bucket_idx<cmdline.cmd_table->nbuckets; bucket_idx++){
		
		//printf("command_generator : bucket_idx %d \n", bucket_idx);
		if(flag) item = hash_items (bucket_idx, cmdline.cmd_table);
		else flag = 1;
		
		while(item){
			name = ((COMMAND *)(item->data))->name;
			
			//printf("command_generator : name %s \n", name);
			if (strncmp (name, text, len) == 0){
				item = item->next;
				//printf("command_generator : %s cmd \n", name);
				return (dupstr(name));
			}
			item = item->next;
		}
	}
	
	/* If no names matched, then return NULL. */
	return ((char *)NULL);
}

char *
envpath_command_generator (const char *text, int state)
{

	char *path;
	char *dir;
	char *name;

	if (!state){
		
		cmdline.findenv = 1;
		
		path = dupstr(getenv("PATH"));
		
		dir= strtok(path, ":");

		while(dir){
			//printf("%d : %s \n", state, dir);
			traverse_exec_files(cmdline.filelist, dir, text);
			dir= strtok(NULL, ":");	
		}

		free(path);
	}

	name = list_dequeue(cmdline.filelist);
	
	if(name)
		return name;
	else
		return ((char *)NULL);
}

/* Generator function for command completion.  STATE lets us know whether
   to start from scratch; without any state (i.e. STATE == 0), then we
   start at the top of the list. */
char *
command_generator (const char *text, int state)
{

	char *cmd_name;

	cmd_name = internal_command_generator(text, state);
	if(cmd_name){
		return cmd_name;
	}

	cmd_name = envpath_command_generator(text, cmdline.findenv);
	if(cmd_name){
		return cmd_name;
	}
 
	/* If no names matched, then return NULL. */
	return ((char *)NULL);
}

/* Attempt to complete on the contents of TEXT.  START and END bound the
   region of rl_line_buffer that contains the word to complete.  TEXT is
   the word to complete.  We can use the entire contents of rl_line_buffer
   in case we want to do some simple parsing.  Return the array of matches,
   or NULL if there aren't any. */
char **
fileman_completion (text, start, end)
     const char *text;
     int start, end;
{
  char **matches;

  matches = (char **)NULL;

  /* If this word is at the start of the line, then it is a command
     to complete.  Otherwise it is the name of a file in the current
     directory. */
  if (start == 0)
    matches = rl_completion_matches (text, command_generator);

  return (matches);
}

/* Tell the GNU Readline library how to complete.  We want to try to complete
   on command names if this is the first word in the line, or on filenames
   if not. */
void initialize_readline (char *readline_name)
{
  /* Allow conditional parsing of the ~/.inputrc file. */
  rl_readline_name = readline_name;

  /* Tell the completer that we want a crack first. */
  rl_attempted_completion_function = fileman_completion;
}

int print_cmddoc(BUCKET_CONTENTS *item)
{
	
	COMMAND *command = (COMMAND *)item->data;

	if(command){
		printf ("%s\t\t%s.\n", item->key, command->doc);
	}

	return 0;
}


/* Print out help for ARG, or for all of the commands if ARG is
   not present. */
int com_help (int argc, char *argv[])
{
	int i;
	char *name;
	COMMAND *command;

	if(argc>1){
		for(i=1;i<argc; i++){
			command = find_command(argv[i]);
			if(command){
				printf ("%s\t\t%s.\n", command->name, command->doc);
			}else{
				printf ("No commands match `%s'.  \n", argv[i]);
			}
		}
	}else{
		hash_walk(cmdline.cmd_table, print_cmddoc);
	}

	return (0);
}

/* Change to the directory ARG. */
int com_cd (int argc, char *argv[])
{
	int ret;
	
	if(argc>1){
		ret = chdir (argv[1]);
	}else{
		ret = chdir (cmdline.homedir);
	}

	if (ret == -1){
		perror (argv[1]);
		return 1;
	}

	//system("pwd");
	
	return (0);
}

int com_prompt (int argc, char *argv[])
{
	int ret;
	
	if(argc>1){
		set_prompt_prefix(argv[1]);
	}
	
	return (0);
}

/* The user wishes to quit using this program.  Just set DONE non-zero. */
int com_quit (int argc, char *argv[])
{
	cmdline.runflag = 0;
	return (0);
}

int test_cmd(int argc, char *argv[])
{
	int i;
	printf("test_cmd : %d arguments \n", argc);
	for(i=0; i<argc; i++){
		printf("%d , %s \n", i, argv[i]);
	}
	return 0;
}


int show_cmdline_table(int argc, char *argv[])
{
	hash_pstats(cmdline.cmd_table, "cmdline hashtable");
	return 0;
}



/* Execute a command line. */
int
execute_line (char *line)
{
	char *word;
	COMMAND *command;

	int argc = 0;
	static char *argv[100];

	word = strpbrk(line, " \t");
	if(word)*word = 0;

	
	if(strcmp(line, cmdline.progname)==0){
		printf("%s already run\n", cmdline.progname);
		return -1;
	}
	
	command = find_command(line);	

	if(word)*word = 32;
	
	if (command){
		word= strtok(line, " \t");
		while(word){
			//printf("execute_line : %d , %s\n", argc, word);
			argv[argc++] = word;
			word= strtok(NULL, " \t");	
		}
		//printf("execute_line : find command %s , execute it\n", command->name);
		return (command->func(argc, argv));
	}else{	
		//printf("execute_line : don't find command %s , execute system cmd\n", argv[0]);
		return system(line);
	}

	/* Call the function. */
	
}

void regist_internal_command(char *name, cmdline_func func, char *doc)
{
	COMMAND *command;
	char *key;

	key = dupstr(name);

	BUCKET_CONTENTS *hash_item = hash_insert(key, cmdline.cmd_table, 0);
	
	if(hash_item && (hash_item->times_found==0)){
		command = xmalloc(sizeof(COMMAND));
		command->name = key;
		command->func = func;
		command->doc = doc;
		hash_item->data = command;
	}else{
		free(key);
	}

}

void init_cmdline(char *program_name)
{
	
	cmdline.runflag = 0;

	cmdline.uid = getuid();
	cmdline.euid = geteuid();
	cmdline.gid = getgid();
	cmdline.egid = getegid();

	//printf("uid : %d, euid : %d, gid : %d, egid : %d \n", cmdline.uid, cmdline.euid, cmdline.gid, cmdline.egid);

	cmdline.homedir = getenv("HOME");
	cmdline.progname = dupstr(program_name);
	cmdline.cmd_table = hash_create(STRING_KEY, 0);

	cmdline.filelist = create_list(STRING_DATA);

	initialize_readline(cmdline.progname);

	regist_internal_command("help", com_help, "Display this text");
	regist_internal_command("?", com_help, "Synonym for `help'");
	regist_internal_command("cd", com_cd, "Change to directory DIR");
	regist_internal_command("quit", com_quit, "Quit cmd line");
	regist_internal_command("exit", com_quit, "Quit cmd line");
	regist_internal_command("prompt", com_prompt, "set cmd line prompt");
	
}

void start_cmdline(char *prompt)
{
	char *line, *tempstr;
	char *cwd;
	int cwdlen;
	HIST_ENTRY *histentry;
	int his_offset;
	
	char databuf[128];

	cmdline.runflag = 1;
	
	set_prompt_prefix(prompt);
	
	while(cmdline.runflag){

		update_prompt_cwd();
		
		tempstr = readline (cmdline.prompt);
		
		if (!tempstr)
			continue;

		line = stripwhite(tempstr);

		if (*line){
			histentry = current_history();
			if(histentry &&  (strcmp(line, histentry->line)==0)){
				his_offset = where_history();
				remove_history(his_offset);
			}
			
			add_history (line);
			execute_line (line);
		}

		free (tempstr);
	}

	free(cmdline.progname);
	hash_flush(cmdline.cmd_table, NULL);
	hash_dispose(cmdline.cmd_table);
	
}

void exit_cmdline(void)
{
	cmdline.runflag = 0;
}

#if 0
int main(int argc, char *argv[])
{
	init_cmdline(argv[0]);
	
	
	regist_internal_command("test", test_cmd, "test command");
	regist_internal_command("pstat", show_cmdline_table, "show cmdline hashtable");

	start_cmdline("zzz");

	return 0;
}
#endif
