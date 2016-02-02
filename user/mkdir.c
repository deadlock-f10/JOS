#include <inc/lib.h>

int
mkdir(const char *path){
	int fd,r;
	struct Stat st;
	if((fd = open(path,O_MKDIR | O_EXCL | O_CREAT)) < 0)
		panic("create dir %s: %e", path,fd);
	if((r = stat(path, &st)) < 0)
		panic("stat %s: %e", path, r);
	close(fd);
	return 0;
}
void
usage(void)
{
	printf("usage: mkdir [path...]\n");
	exit();
}
void
umain(int argc, char **argv)
{
	int i;
	if (argc != 2)
		usage();
	else
	{
		char t[MAXPATHLEN];  
		strcpy(t,argv[1]);
		toAbsolutePath(t);
		mkdir(t);
	}
}

