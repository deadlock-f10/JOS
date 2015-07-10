#include <inc/lib.h>
void
usage(void)
{
	printf("usage: cd [file...]\n");
	exit();
}

void cd(char *path){
	int r;
	struct Stat st ;
	if((r = stat(path,&st)) < 0)
	{
		cprintf("cd : invalid path %s\n",path);
		return ;
	}
	if(!st.st_isdir)
	{
		cprintf("%s is not a directory\n",path);
		return ;
	}
	strcpy(cur_dir,path);
}
void goback(){
	int i;
	for(i=strlen(cur_dir)-2 ; i > 0; i++)
		if(cur_dir[i] != '/')
			cur_dir[i] = '\0';
}
void
umain(int argc, char **argv)
{
	if(strcmp(argv[1],".") == 0)
	{
		exit();
		ipc_send(0,0,NULL,0);
	}
	else if(strcmp(argv[1],"..") == 0)
		goback();
	else{
		if(argv[1][0] == '/'){
			cd(argv[1]);
		}
		else{
			char path[MAXPATHLEN];
			strcpy(path,argv[1]);
			toAbsolutePath(path);
			cd(path);
		}
	}
//	cprintf("in cd :cur_dir %s",cur_dir);
/*	sys_page_alloc(thisenv->env_id,PFTEMP,PTE_SYSCALL);
	strcpy((char *)PFTEMP , cur_dir);
	ipc_send(sh_envid,1,PFTEMP,PTE_SYSCALL);*/
}

