
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <dirent.h>
#include <signal.h>

#include <unistd.h>

#include <sys/wait.h>

#include "utils/utils.h"
#include "utils/queue.h"
#include "transfer/spi.h"

#include "system.h"
#include "flash/flash.h"
#include "flash/mtd.h"

#include "fpga.h"
#include "fpga_reg.h"
#include "upgrade.h"

static int upgrade_flag = 0;

#define TYPE_MTD_FILE 		1
#define TYPE_MTD_FILE_2 		0x10
#define TYPE_FS_FILE 		2
#define TYPE_FS_DIR			3

#define MTD_CP_RATIO		0.75
#define FS_CP_RATIO		0.25

#define NOR_E_RATIO		0.48
#define NOR_W_RATIO	0.52

typedef struct{
	short type;
	short ratio;
	int size;
	int max_size;
	int base;
	char *target;
	char *flag;
	char name[40];
}upgrade_file_st;

typedef struct {
	int total_size;
	int mtd_files_size;
	int fs_files_size;
	short mtd_ratio;
	short fs_ratio;
	int progress_ratio;
	list_st files;
}upgrade_package_st;

static upgrade_package_st upgrade_package;


program_version_st current_version, *pcur_ver;

static pid_t bin_pid;

//#define p_printf(pid, fmt, ...)	printf("pid [ %d ] : "fmt, pid, ##__VA_ARGS__)
#define p_printf(pid, fmt, ...)	printf(fmt,  ##__VA_ARGS__)


sysinfo_st *sysinfo;

void upgrade_input_value(int idx, int value)
{
	printf("upgrade_status[%d].input_value = %d \n", idx, value);
	
	upgrade_status[idx].input_value = value;
	upgrade_status[idx].hasInput = 1;

	if(idx>0){
		spi_write_block(idx-1, SLAVER_SPI_UPGRADE_INPUT_ADDR, &upgrade_status[idx].hasInput, 4);
	}
}

static int get_upgrade_input(int idx)
{
	upgrade_status[idx].hasInput = 0;
	
	while(upgrade_status[idx].hasInput==0){
		usleep(10);
	}
	
	return upgrade_status[idx].input_value;
}

/* dirwalk:  apply fcn to all files in dir */
static int traverse_upgrade_files(list_st *plist, char *dir, char *prefix, char *subfix)
{
	char name[256];
	struct dirent *dp;
	DIR *dfd;
	//int path_len = 0;

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
		//p_printf(bin_pid,"traverse_dir : dp->d_name :  %s\n", dp->d_name);
		if(strstr(dp->d_name, prefix) && strstr(dp->d_name, subfix)){

			//memset(name, 0, 256);
			sprintf(name, "%s/%s", dir, dp->d_name);
			//p_printf(bin_pid,"traverse_dir : %s\n", name);
			if (stat(name, &stbuf) == -1) {
				fprintf(stderr, "fsize: can't access %s\n", name);
				continue;
			}

			if (S_ISREG(stbuf.st_mode) || S_ISLNK(stbuf.st_mode)){
				//p_printf(bin_pid," find file :  %s\n", name); 
				list_add(plist, name, 0);
			}
		}
	}
	
	closedir(dfd);

	return 0;
	
}


static int read_version_file(program_version_st *p_version, char *version_file)
{
	FILE *fp;
	char buf[128];
	char *pch;
	char *separator = "=: \t";
	u_int value;
	int ret;

	//p_printf(bin_pid,"read version file %s \n", version_file);

	fp = fopen(version_file, "r");
	if(fp==NULL){
		perror("read version file");
		return -1;
	}

	while(fgets(buf, 128, fp)){

		pch = strtok(buf, separator);		
		value =  (u_int)strtoul(strtrim(strtok(NULL, separator)), NULL, 16);

		if(strcmp("devtype", strtrim(pch)) == 0){
			p_version->dev_type = value;
		}else if(strcmp("main", strtrim(pch)) == 0){
			p_version->main_version = value;
		}else if(strcmp("app", strtrim(pch)) == 0){
			p_version->app_version = value;
		}else if(strcmp("fpga", strtrim(pch)) == 0){
			p_version->fpga_version = value;
		}else if(strcmp("kernel", strtrim(pch)) == 0){
			p_version->kernel_version = value;
		}else if(strcmp("firmware", strtrim(pch)) == 0){
			p_version->firmware_version= value;
		}else if(strcmp("web", strtrim(pch)) == 0){
			p_version->web_version = value;
		}
		
	}

	fclose(fp);

	return 0;
}

static void  print_version_file(program_version_st *p_version, char *prefix)
{
	
	if(p_version==NULL){
		return;
	}

	if(prefix==NULL){
		prefix = "";
	}

	p_printf(bin_pid,"%s device type \t: %#x \n", prefix, p_version->dev_type);
	p_printf(bin_pid,"%s main version \t: %#x \n", prefix, p_version->main_version);
	p_printf(bin_pid,"%s firmware version \t: %#x \n", prefix, p_version->firmware_version);
	p_printf(bin_pid,"%s app version \t: %#x \n", prefix, p_version->app_version);
	p_printf(bin_pid,"%s kernel version \t: %#x \n", prefix, p_version->kernel_version);
	p_printf(bin_pid,"%s fpga version \t: %#x \n", prefix, p_version->fpga_version);
	p_printf(bin_pid,"%s web version \t: %#x \n", prefix, p_version->web_version);
	
}

upgrade_file_st *add_upgrade_file(char *filename, int type, char *target)
{
	upgrade_file_st *upgrade_file = xMalloc(sizeof(upgrade_file_st));
	
	strcpy(upgrade_file->name, filename);
	upgrade_file->type = type;
	if((upgrade_file->type  == TYPE_FS_FILE) || (upgrade_file->type  == TYPE_FS_DIR)){
		upgrade_file->target = cpstring(target);
	}else{
		upgrade_file->target = target;
	}
	upgrade_file->max_size = 0;
	upgrade_file->ratio = 0;
	upgrade_file->flag = NULL;

	list_add(&upgrade_package.files, upgrade_file, ASSIGN_FLAG);

	return upgrade_file;
}

void free_upgrade_file(upgrade_file_st *upgrade_file)
{
	if(upgrade_file == NULL){
		return;
	}

	if((upgrade_file->type  == TYPE_FS_FILE) || (upgrade_file->type  == TYPE_FS_DIR)){
		free(upgrade_file->target);
	}

	free(upgrade_file);
}

#define ADD_MTD_FILE_2(name, target, maxsize)		do{	\
	filesize = calc_filesize(file); \
	if(filesize>0){	\
		upgrade_file = add_upgrade_file(name, TYPE_MTD_FILE_2, (char *)(target));	\
		upgrade_file->size = filesize;	\
		upgrade_file->max_size = (maxsize); \
		upgrade_package.total_size += upgrade_file->size;	\
		upgrade_package.mtd_files_size += upgrade_file->size;	\
	}\
}while(0)

#define ADD_MTD_FILE(name, target, maxsize)		do{	\
	filesize = calc_filesize(file); \
	if(filesize>0){	\
		upgrade_file = add_upgrade_file(name, TYPE_MTD_FILE, (char *)(target));	\
		upgrade_file->size = filesize;	\
		upgrade_file->max_size = (maxsize); \
		upgrade_package.total_size += upgrade_file->size;	\
		upgrade_package.mtd_files_size += upgrade_file->size;	\
	}\
}while(0)


#define ADD_FS_WILDCARDFILES(name, target)		do{	\
	filesize = calc_wildcardfiles_size(file); \
	if(filesize>0){	\
		upgrade_file = add_upgrade_file(name, TYPE_FS_FILE, (char *)(target));	\
		upgrade_file->size = filesize;	\
		upgrade_package.total_size += upgrade_file->size;	\
		upgrade_package.fs_files_size += upgrade_file->size;	\
	}\
}while(0)

#define ADD_FS_FILE(name, target)		do{	\
	filesize = calc_filesize(file); \
	if(filesize>0){	\
		upgrade_file = add_upgrade_file(name, TYPE_FS_FILE, (char *)(target));	\
		upgrade_file->size = filesize;	\
		upgrade_package.total_size += upgrade_file->size;	\
		upgrade_package.fs_files_size += upgrade_file->size;	\
	}\
}while(0)

#define ADD_FS_DIR(name, target)		do{	\
	filesize = calc_filesize(file); \
	if(filesize>0){	\
		upgrade_file = add_upgrade_file(name, TYPE_FS_DIR, (char *)(target));	\
		upgrade_file->size = filesize;	\
		upgrade_package.total_size += upgrade_file->size;	\
		upgrade_package.fs_files_size += upgrade_file->size;	\
	}\
}while(0)

int prepare_upgrade_files(char *filedir)
{
	char ch, file[128];
	char *filename;
	upgrade_file_st *upgrade_file = NULL;
	list_st *plist;
	short mtd_file_ratio = 10000, fs_file_ratio = 10000;
	int filesize = 0;

	strcpy(file, filedir);
	filename = &file[strlen(filedir)];

	printf("prepare_upgrade_files : %s \n", filedir);

	upgrade_package.total_size = 0;
	upgrade_package.fs_files_size = 0;
	upgrade_package.mtd_files_size = 0;
	upgrade_package.progress_ratio = 0;
	init_list(&upgrade_package.files, STRUCT_DATA);



	strcpy(filename, "top.rbf");
	if(( access(file,F_OK))<0){
		strcpy(filename, "dcp3000_top.rbf");
	}

	#ifdef USE_NOR_FLASH
	ADD_MTD_FILE(filename, "/dev/mtd4", FPGA_CODE_SIZE);
	if(filesize > 0){
		upgrade_file->flag = "FPGA";
		upgrade_file->base = FPGA_MTD_BASE_ADDR;
	}
	#else
	ADD_FS_DIR(filename, OPT_DIR);
	#endif
	
	strcpy(filename, "uImage");
	ADD_MTD_FILE(filename, "/dev/mtd2", KERNEL_MTD_SIZE);
	if(filesize > 0){
		upgrade_file->base= KERNEL_MTD_BASE_ADDR;
	}

	strcpy(filename, "rootfs.cramfs");
	if(( access(file,F_OK))<0){
		strcpy(filename, "rootfs.jffs2");
	}
	ADD_MTD_FILE(filename, "/dev/mtd3", ROOTFS_MTD_SIZE);
	if(filesize > 0){
		upgrade_file->base= ROOTFS_MTD_BASE_ADDR;
	}
	
	if(upgrade_file)
		upgrade_file->ratio = 10000;

	strcpy(filename, "webroot");
	ADD_FS_DIR(filename, OPT_DIR);

	
	strcpy(filename, "lib");
	ADD_FS_DIR(filename, OPT_DIR);

	strcpy(filename, "lib*.so");
	ADD_FS_WILDCARDFILES(filename, OPT_DIR"/lib/");
	

	strcpy(filename, "driver");
	ADD_FS_DIR(filename, HOME_DIR);

	strcpy(filename, "*.ko");
	ADD_FS_WILDCARDFILES(filename, HOME_DIR"/driver/");

	
	strcpy(filename, "properties");
	ADD_FS_DIR(filename, HOME_DIR);

	strcpy(filename, "*.properties");
	ADD_FS_WILDCARDFILES(filename, HOME_DIR"/properties/");

	strcpy(filename, "version");
	ADD_FS_FILE(filename, HOME_DIR);

	strcpy(filename, "bashrc");
	ADD_FS_FILE(filename, HOME_DIR);
	
	strcpy(filename, "board_info.xml");
	ADD_FS_FILE(filename, HOME_DIR);

	strcpy(filename, "dcp3000");
	if(( access(file,F_OK))<0){
		strcpy(filename, "dcp3000.bin");
	}
	ADD_FS_FILE(filename, HOME_DIR"/dcp3000");
	if((filesize > 0) && ( access(file,X_OK) <0 )){
		sys_cmd("chmod a+x %s", file);
	}
	
	strcpy(filename, "upgrade");
	if(( access(file,F_OK))<0){
		strcpy(filename, "upgrade.bin");
	}
	ADD_FS_FILE(filename, HOME_DIR"/upgrade");
	if((filesize > 0) && ( access(file,X_OK) <0 )){
		sys_cmd("chmod a+x %s", file);
	}

	strcpy(filename, "debug_tools");
	if(( access(file,F_OK))<0){
		strcpy(filename, "debug_tools.bin");
	}
	ADD_FS_FILE(filename, HOME_DIR"/debug_tools");
	if((filesize > 0) && ( access(file,X_OK) <0 )){
		sys_cmd("chmod a+x %s", file);
	}

	strcpy(filename, "watchd");
	if(( access(file,F_OK))<0){
		strcpy(filename, "watchd.bin");
	}
	ADD_FS_FILE(filename, HOME_DIR"/watchd");
	if((filesize > 0) && ( access(file,X_OK) <0 )){
		sys_cmd("chmod a+x %s", file);
	}

	strcpy(filename, "httpd");
	if(( access(file,F_OK))<0){
		strcpy(filename, "httpd.bin");
	}
	ADD_FS_FILE(filename, HOME_DIR"/httpd");
	if((filesize > 0) && ( access(file,X_OK) <0 )){
		sys_cmd("chmod a+x %s", file);
	}

	
	if(upgrade_file)
		upgrade_file->ratio = 10000;


	if(upgrade_package.total_size<=0){
		return -1;
	}
	

	upgrade_package.fs_ratio = (upgrade_package.fs_files_size * FS_CP_RATIO) * 10000
		/(upgrade_package.fs_files_size * FS_CP_RATIO + upgrade_package.mtd_files_size* MTD_CP_RATIO);
	upgrade_package.mtd_ratio = 10000 - upgrade_package.fs_ratio;

	printf("upgrade package : fs files [%d : %d], mtd files [%d : %d]\n", upgrade_package.fs_files_size, upgrade_package.fs_ratio, upgrade_package.mtd_files_size, upgrade_package.mtd_ratio);

	#if 1
	upgrade_file = (upgrade_file_st *)list_access(&upgrade_package.files, 0);
	
	while(upgrade_file){

		if(upgrade_file->size>0){
			
			printf("%s size = %d, type = %d\n", upgrade_file->name, upgrade_file->size, upgrade_file->type);
		
			if(upgrade_file->type == TYPE_MTD_FILE){
				if(upgrade_file->ratio){
					upgrade_file->ratio = mtd_file_ratio;
				}else{
					upgrade_file->ratio = upgrade_file->size *10000LL /upgrade_package.mtd_files_size;
					mtd_file_ratio -= upgrade_file->ratio;
				}
				//printf(" target = %#x, ratio = %d \n", (u_int)upgrade_file->target, upgrade_file->ratio);
				printf(" target = %s, ratio = %d , base : %#x\n", (u_int)upgrade_file->target, upgrade_file->ratio, upgrade_file->base);
				
			}else{
				if(upgrade_file->ratio){
					upgrade_file->ratio = fs_file_ratio;
				}else{
					upgrade_file->ratio = upgrade_file->size *10000LL /upgrade_package.fs_files_size;
					fs_file_ratio -= upgrade_file->ratio;
				}
				printf(" target = %s, ratio = %d \n", upgrade_file->target, upgrade_file->ratio);
			}
		}
		upgrade_file = (upgrade_file_st *)list_next(&upgrade_package.files);
	}

	list_access_end(&upgrade_package.files);
	
	#endif

	return 0;
	
}

int calc_mtd_erase_nor_ratio(int ratio)
{
	return (ratio * upgrade_package.mtd_ratio * NOR_E_RATIO/10000);
}


int calc_mtd_write_nor_ratio(int ratio)
{
	return (ratio * upgrade_package.mtd_ratio * NOR_W_RATIO/10000);
}


int calc_fs_cp_ratio(int ratio)
{
	return (ratio * upgrade_package.fs_ratio/10000);
}

void stat_erase_nor_ratio(double ratio, void *arg)
{
	upgrade_file_st *upgrade_file = (upgrade_file_st *)arg;

	upgrade_status[0].progress_ratio = upgrade_package.progress_ratio 
		+ calc_mtd_erase_nor_ratio((int)(10000*ratio * upgrade_file->ratio)/10000);
	
}

void stat_write_nor_ratio(double ratio, void *arg)
{
	upgrade_file_st *upgrade_file = (upgrade_file_st *)arg;
	
	upgrade_status[0].progress_ratio = upgrade_package.progress_ratio 
		+ calc_mtd_write_nor_ratio((int)(10000*ratio * upgrade_file->ratio)/10000);
}

static int upgrade_program(char *filepath)
{
	int ret = 0;
	int fd;
	u_short v_u16;
	list_st *filelist;
	char ch, file[128];
	char argv_buf[20];
	char *filename, *filedir;
	char *data_prefix;
	struct stat stbuf;
	program_version_st upgrade_version;
	mtdinfo_st *mtdinfo;
	u_int baseAddr;
	int len, base_sector,  sector_count;
	upgrade_file_st *upgrade_file;
	pid_t pid;

	//p_printf(bin_pid,"upgrade_program : upgrade flag = %#x, upgrade_file %s  \n", upgrade_flag, filepath);

	
	strcpy(file, filepath);
	filedir = file;

	if((upgrade_flag&0x10) == 0){
		
		//p_printf(bin_pid,"upgrade_program : ---------------- \n");

		if (stat(filepath, &stbuf) == -1) {
			fprintf(stderr, "stat: can't access %s\n", filepath);
			upgrade_status[0].status = UPGRADE_FILE_ERROR;
			upgrade_status[0].flag = UPGRADE_OVER;
			return -1;
		}

		if(S_ISDIR(stbuf.st_mode)) {
			len = strlen(file);
			if(file[len-1] != '/'){
				file[len++] = '/';
			}	
			filename = &file[len];
		}else{
			filename = strrchr(file, '/');
			if(filename==NULL){
				filename = filedir;
				sys_cmd("tar -xzf %s", filepath);
			}else{
				filename++;
				*filename = 0;
				sys_cmd("tar -xzf %s -C %s", filepath, filedir);
			}
			sys_cmd("rm -f %s ", filepath);
		}

		

		if(pcur_ver){
			strcpy(filename, "version");
			if(read_version_file(&upgrade_version, file)<0){
				//return -1;
			}else{
				print_version_file(&upgrade_version, "upgrade program");

				if(((pcur_ver->dev_type>>8) != (upgrade_version.dev_type>>8))){
					p_printf(bin_pid,"upgrade-package is not matched this device ,  exit\n");
					upgrade_status[0].status = UPGRADE_FILE_DONOT_MATCH;
					upgrade_status[0].flag = UPGRADE_OVER;
					strcpy(upgrade_status[0].status_msg, "upgrade package is not matched this device");
					filename[-1] = 0;
					sys_cmd("rm -f %s/* ", filedir);
					return -1;
				}


				if(pcur_ver->main_version > upgrade_version.main_version){
					
					do{
																	
						if(upgrade_flag&0x20){
							p_printf(bin_pid,"\nupgrade version is less than current version , please confirm on web page :  ");
							fflush(stdout);
							upgrade_status[0].status = UPGRADE_FILE_VERSION_LOWER;
							strcpy(upgrade_status[0].status_msg, "upgrade version is less than current version , whether to continue [ y / n ] : ");
							ch = get_upgrade_input(0);
						}else{
							p_printf(bin_pid,"\nupgrade version is less than current version , whether to continue [ y / n ] : ");
							ch = getchar();
						}
						
						if(ch=='n'){
							printf("\nexit the upgrade process\n");
							upgrade_status[0].status = UPGRADE_EXIT;
							upgrade_status[0].flag = UPGRADE_OVER;
							filename[-1] = 0;
							sys_cmd("rm -f %s/* ", filedir);
							return 1;
						}else if(ch=='y'){
							printf("\ncontinue the upgrade process\n");
							break;
						}

					}while(1);
				}
			}
		}
		
		strcpy(filename, "upgrade");
		ret = access(file,X_OK);
		if (ret != 0) {
			//fprintf(stderr, "stat: can't access %s\n", file);
			strcpy(filename, "upgrade.bin");
			ret = access(file,X_OK);
		}
	
		if (ret == 0) {

			//p_printf(bin_pid,"upgrade_program : -------the upgrade bin in upgrade package exist --------- \n");

			 pid = fork();
			 if (pid < 0) {
				perror("fork failed");
				*filename = 0;
				sys_cmd("rm -rf %s ", filedir);
				upgrade_status[0].flag = UPGRADE_OVER;
				upgrade_status[0].status = UPGRADE_FAILED;
			 	return -1;
			 }else if (pid == 0){
			 
			 	len = filename - filedir;
				filedir = cpnstring(file, len);
			//	p_printf(bin_pid,"execl : ---%s, %s , 16 , %s------------- \n", file, filename, filedir);

				upgrade_flag |= 0x10;
				sprintf(argv_buf, "%d", upgrade_flag);
				execl(file, filename, argv_buf, filedir, NULL);
				free(filedir);
				//p_printf(bin_pid,"%s, exec end---------------- \n", file);
				exit(0);
			 }else{
			 	//waitpid(pid, &ret, 0);
				//p_printf(bin_pid,"child process %d exec end-, wait--%d------------- \n", pid, ret);
				return 1;
			 }
			 
		}
	

	}else{
		filename = filedir + strlen(filedir);
		//p_printf(bin_pid,"upgrade_program : ======================== \n");
	}

	*filename = 0;
	if(prepare_upgrade_files(filedir)<0){
		upgrade_status[0].flag = UPGRADE_OVER;
		upgrade_status[0].status = UPGRADE_FILE_ERROR;
		strcpy(upgrade_status[0].status_msg, "dcp3000 main board  upgrade files error");
		return -1;
	}

	
	upgrade_status[0].flag = UPGRADE_START;
	strcpy(upgrade_status[0].status_msg, "dcp3000 main board  upgrade start");

	
	//unprotect_flash(OPT_MTD_BASE_SECTOR, OPT_MTD_SECTOR_COUNT);
	//unprotect_flash(HOME_MTD_BASE_SECTOR, HOME_MTD_SECTOR_COUNT);
	
	#if 1
	while(upgrade_file = (upgrade_file_st *)list_dequeue(&upgrade_package.files)){
		
		if(upgrade_file->size>0){

			strcpy(filename, upgrade_file->name);
			
			if(upgrade_file->type == TYPE_MTD_FILE){
				#ifdef USE_NOR_FLASH
				
				len = upgrade_file->size;
				if((len>0) || (len<=upgrade_file->max_size)){

	
					fd = mtd_open(upgrade_file->target);
					if(fd<0){
						upgrade_status[0].flag = UPGRADE_OVER;
						upgrade_status[0].status = UPGRADE_FAILED;
						strcpy(upgrade_status[0].status_msg, "dcp3000 main board  access target location error");
					}

					data_prefix =  upgrade_file->flag;
					baseAddr = upgrade_file->base;
					sector_count = (len+FLASH_SECTOR_SIZE-1)/FLASH_SECTOR_SIZE;
					base_sector = baseAddr/FLASH_SECTOR_SIZE;
					
					unprotect_flash(base_sector, sector_count);

					mtd_erase_with_fn(fd, 0, sector_count, stat_erase_nor_ratio, upgrade_file);
					//erase_flash_with_fn(base_sector, sector_count, stat_erase_nor_ratio, upgrade_file);
					upgrade_package.progress_ratio += calc_mtd_erase_nor_ratio(upgrade_file->ratio);

					printf("upgrade_package.progress_ratio 'e'= %d \n", upgrade_package.progress_ratio);
					
					//erase_flash(base_sector, sector_count);
					
					mtd_write_file_with_fn(fd, 0, data_prefix, file, stat_write_nor_ratio, upgrade_file);
					//flash_write_file(baseAddr, "FPGA", file);
						
					upgrade_package.progress_ratio += calc_mtd_write_nor_ratio(upgrade_file->ratio);
					sys_cmd("rm -f %s ", file);	
					printf("upgrade_package.progress_ratio 'w' = %d \n", upgrade_package.progress_ratio);
					protect_flash(base_sector, sector_count);

					mtd_close(fd);
					
				}
				#endif
			}
			else if(upgrade_file->type == TYPE_MTD_FILE_2){
				#ifdef USE_NOR_FLASH
				
				len = upgrade_file->size;
				if((len>0) || (len<=upgrade_file->max_size)){

					data_prefix =  upgrade_file->flag;
					baseAddr = upgrade_file->base;
					sector_count = (len+FLASH_SECTOR_SIZE-1)/FLASH_SECTOR_SIZE;
					base_sector = baseAddr/FLASH_SECTOR_SIZE;
					
					unprotect_flash(base_sector, sector_count);

					//mtd_erase_with_fn(fd, 0, sector_count, stat_erase_nor_ratio, upgrade_file);
					erase_flash_with_fn(base_sector, sector_count, stat_erase_nor_ratio, upgrade_file);
					upgrade_package.progress_ratio += calc_mtd_erase_nor_ratio(upgrade_file->ratio);

					printf("upgrade_package.progress_ratio 'e'= %d \n", upgrade_package.progress_ratio);
					
					//mtd_write_file_with_fn(fd, 0, data_prefix, file, stat_write_nor_ratio, upgrade_file);
					write_file_to_flash_with_fn(baseAddr, data_prefix, file, stat_write_nor_ratio, upgrade_file);

					upgrade_package.progress_ratio += calc_mtd_write_nor_ratio(upgrade_file->ratio);
					sys_cmd("rm -f %s ", file);	
					printf("upgrade_package.progress_ratio 'w' = %d \n", upgrade_package.progress_ratio);
					protect_flash(base_sector, sector_count);
					
				}
				#endif

			}
			else if(upgrade_file->type == TYPE_FS_DIR){
				sys_cmd("rm -rf %s%s", upgrade_file->target, strrchr(file,'/'));
				sys_cmd("cp -r %s %s", file, upgrade_file->target);
				sys_cmd("rm -rf %s", file);
				upgrade_package.progress_ratio += calc_fs_cp_ratio(upgrade_file->ratio);
			}else if(upgrade_file->type == TYPE_FS_FILE){
				if(access(upgrade_file->target,F_OK)<0){
					mkdir(upgrade_file->target, 0644);
				}
				sys_cmd("mv  %s %s", file, upgrade_file->target);
				upgrade_package.progress_ratio += calc_fs_cp_ratio(upgrade_file->ratio);
			}
			upgrade_status[0].progress_ratio = upgrade_package.progress_ratio;
			printf("upgrade_package.progress_ratio = %d \n", upgrade_package.progress_ratio);
		}
		
		free_upgrade_file(upgrade_file);
		
	}

	
	system("sync"); 
	
	//protect_flash(OPT_MTD_BASE_SECTOR, OPT_MTD_SECTOR_COUNT);
	//protect_flash(HOME_MTD_BASE_SECTOR, HOME_MTD_SECTOR_COUNT);

	#endif
	
	p_printf(bin_pid,"upgrade the  programs of dcp3000 main board success\n");
	upgrade_status[0].progress_ratio = 10000;
	upgrade_status[0].flag = UPGRADE_OVER;
	upgrade_status[0].status = UPGRADE_SECCESS;
	strcpy(upgrade_status[0].status_msg, "dcp3000 main board  upgrade success");
	
	return 0;
}


int upgrade_slot_board(int slot)
{
	int ret = 0;
	u_short v_u16;
	list_st *filelist;
	char filedir[128];
	char *filename;
	struct timeval t_start; 

	gettimeofday(&t_start, NULL); 
	
	filelist = create_list(STRING_DATA);

	sprintf(filedir,"%s/slot%.2d", UPGRADE_FILES_DIR, slot);

	memset(&upgrade_status[slot+1], 0,  sizeof(upgrade_st));

	ret = traverse_upgrade_files(filelist, filedir, "", "");
	if(ret==0){
		while(!list_isEmpty(filelist)){
			filename = list_dequeue(filelist);
			ret = spi_send_file(slot, filename);
			if(ret<0){
				break;
			}
			sys_cmd("rm -f %s ", filename);
		}
		if(ret==0)
			ret = spi_write_word(slot, SLAVER_SPI_UPGRADE_ADDR, 0);
	}

	destroy_list(filelist);

	show_exectime(&t_start, "upgrade complete");

	return ret;
}

#if 1
int upgrade_main_board(void)
{
	int ret = 0;
	char file[128];
	char *filedir, *filename;
	struct timeval t_start; 

	gettimeofday(&t_start, NULL); 
	
	filedir = file;
	sprintf(file,"%s/mainborad/", UPGRADE_FILES_DIR);
	filename = &file[strlen(filedir)];
	//strcpy(filename, "dcp3000_main.tgz");

	#ifndef INDEPENDENT_UPGRADE
	if(read_version_file(&current_version, HOME_DIR"/version")<0){
		//return -1;
		pcur_ver = NULL;
	}else{
		pcur_ver = &current_version;
		print_version_file(pcur_ver, "current program");
	}
	//pcur_ver = &program_version;
	#endif

	
	if(( access(file,F_OK))<0){
		*filename = 0;
	}

	upgrade_flag |= 0x20;
	
	upgrade_status[0].flag = UPGRADE_PREPARE;
	upgrade_status[0].status = UPGRADE_NORMAL;
	upgrade_status[0].progress_ratio = 0;
	strcpy(upgrade_status[0].status_msg, "dcp3000 main board prepare  upgrade ");

	//printf("%s : %s \n", upgrade_status[0].status_msg, file);
	ret=upgrade_program(file);
	if(ret<0){
		p_printf(bin_pid,"upgrade the  programs of dcp3000 main board failed\n");
	}

	if(ret==0)
	show_exectime(&t_start, "upgrade complete");
	
	return ret;
}
#else
int upgrade_main_board(void)
{
	int ret = 0;
	u_short v_u16;
	list_st *filelist;
	char filedir[128];
	char *filename;
	struct timeval t_start; 

	gettimeofday(&t_start, NULL); 
	
	filelist = create_list(STRING_DATA);

	sprintf(filedir,"%s/mainborad", UPGRADE_FILES_DIR);

	
	ret = traverse_upgrade_files(filelist, filedir, "dcp3000", "tgz");
	if(ret==0){
		#ifndef INDEPENDENT_UPGRADE
		pcur_ver = &program_version;
		#endif
		//while(!list_isEmpty(filelist)){
		if(!list_isEmpty(filelist)){
			filename = list_dequeue(filelist);
			ret=upgrade_program(filename);
			//break;
		}else{
			ret=upgrade_program(filedir);
		}
		
		sys_cmd("rm -rf %s/* ", filedir);
	}

	destroy_list(filelist);

	if(ret<0){
		p_printf(bin_pid,"upgrade the  programs of dcp3000 main board failed\n");
	}

	if(ret==0)
	show_exectime(&t_start, "upgrade complete");
	
	return ret;
}
#endif

/*

	upgrade data : filename[20B], filelen[4B],filedata[filelen],crc16[2]
	upgrade status : upgrade rate of progress [4B]

	若有多个升级文件，则会执行多次

//*/

#ifdef INDEPENDENT_UPGRADE

int do_upgrade(int argc, char *argv[])
{
	int i, status, ret = 0;
	char *filename;
	int slot = 0, spi_run_flag;
	struct timeval t_start; 

	//bin_pid = getpid();
	//p_printf(bin_pid,"do_upgrade : pid = %d \n", bin_pid);

	#if 0
	for(i=1; i<argc; i++){
		if(argv[i][0] == '-'){
			
		}
	}
	#endif
	gettimeofday(&t_start, NULL); 
	
	if(argc>2){
		slot = atoi(argv[1]);

		if((slot&0xf) > 7){
			return -1;
		}
		
		if(slot&0x10){
			upgrade_flag = slot;
			filename = argv[2];
			//p_printf(bin_pid,"do_upgrade upgrade_flag %#x : %s  \n", upgrade_flag, filename);
			ret = upgrade_program(filename);
			if(ret<0){
				p_printf(bin_pid,"upgrade the  programs of dcp3000 main board failed\n");
			}
			//p_printf(bin_pid,"call upgrade package upgrade app end \n");
		}else{

			status = FPGA_ARMIF_READ(REG_FPGA_SLOT_CONNECT_STATUS);
			if(!check_maskbit(&status, slot)){
				p_printf(bin_pid,"upgrade the  programs of %d slot  board , the board not exist,  exit\n", slot);
				return -1;
			}else{
				init_spi_transfer(slot);
				
				start_spi_transfer_task(slot);
			}

		
			for(i=2; i<argc; i++){
				filename = argv[i];
				ret = spi_send_file(slot, filename);
				if(ret<0){
					break;
				}
				unlink(filename);
			}
			if(ret==0)
				ret = spi_write_word(slot, SLAVER_SPI_UPGRADE_ADDR, 0);

			
			end_spi_transfer_task(slot);
			//sleep(1);

			if(ret<0){
				p_printf(bin_pid,"upgrade the  programs of %d slot  board failed\n", slot);
			}else{
				p_printf(bin_pid,"upgrade the  programs of %d slot  board success\n", slot);
			}
		}
	}else{
		#ifndef INDEPENDENT_UPGRADE
		pcur_ver = &program_version;
		#endif
		if(argc>1){
			filename = argv[1];
		}else{
			filename = "/var/ftp/pub/dcp3000_main.tgz";
			if(( access(filename,F_OK))<0){
				filename = "/var/ftp/pub/";
			}
		}
		upgrade_flag = 0;
		upgrade_status[0].flag = UPGRADE_PREPARE;
		upgrade_status[0].status = UPGRADE_NORMAL;
		upgrade_status[0].progress_ratio = 0;
		
		strcpy(upgrade_status[0].status_msg, "dcp3000 main board prepare  upgrade ");
		ret = upgrade_program(filename);
		if(ret<0){
			p_printf(bin_pid,"upgrade the  programs of dcp3000 main board failed\n");
		}
	}

	if(ret==0){
		show_exectime(&t_start, "upgrade complete");
	}

	return ret;
		
}

void wait_for_upgrade(void)
{
	while((upgrade_status->flag != UPGRADE_UNDO) || (upgrade_status->flag != UPGRADE_OVER)){
		sched_yield();
		sleep(1);
	}
}

#include "fpga.h"
#include "event/eventlog.h"

shmem_obj_t *global_shm;

void open_global_shm()
{
	
	global_shm = shmem_init(GLOBAL_SHM_KEY, GLOBAL_SHM_SIZE, SHM_SYSTEMV_IF);
	upgrade_status = (upgrade_st *)((char *)global_shm + UPGRADE_STATUS_GLOBLE_SHM_OFFSET);
	sysinfo = (sysinfo_st *)((char *)global_shm + SYSINFO_GLOBLE_SHM_OFFSET);
	spi_masters = (spi_master_st *)((char *)global_shm + SPIMASTER_GLOBLE_SHM_OFFSET);
}

void close_global_shm()
{
	upgrade_status = NULL;
	shmem_destroy(global_shm);
	
}

int main(int argc, char *argv[])
{
	int result = 0;
	
	setuid(0);
	
	if(read_version_file(&current_version, HOME_DIR"/version")<0){
		//return -1;
		pcur_ver = NULL;
	}else{
		pcur_ver = &current_version;
		print_version_file(pcur_ver, "current program");
	}

	open_fpga();
	open_global_shm();
	init_eventlogs();
	open_flash();
	
	result = do_upgrade(argc, argv);

	end_eventlogs();
	close_flash();
	close_global_shm();
	close_fpga();
	
	return result;
}

#endif
