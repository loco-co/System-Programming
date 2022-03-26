//2021/시스템프로그래밍/ls -Rl programming/황기석
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <errno.h>
#include <grp.h>

int main(int argc, char *argv[]) {
	if(argc>=2 && (access(argv[1], R_OK)<0)) { perror("ls: cannot open directory"); exit(0); }
	//권한없을때 에러처리
	int i = 1;
	while(i){
		char * cwdname;
		DIR * pdir;
		if(argc<2) { //인자가 없으면
			//현재 워킹디렉토리 얻기
			getcwd(cwdname, 1024);
			i = 0;
			//printf("name: %s", cwdnamea);
			pdir = opendir(cwdname);
			//printf("%s\n", cwdname);
		}
		else { //인자가 있으면
			cwdname = argv[i]; //차례대로 열기
			if(i>=argc) { break; }
			i++;
			//printf("%s\n", cwdname);
			if((pdir = opendir(cwdname))==NULL) {
				printf("fail");
			}
		}
		//DIR * pdir = opendir(cwdname);
		struct dirent * pentry = NULL;
		struct stat buf;
		while((pentry = readdir(pdir)) != NULL) { //디렉토리 엔트리인 파일들 읽어오기
			if(pentry->d_name[0]=='.') { continue; } //파일 이름의 첫자가 .이면 패스
			char f[128]; //임시 이름
			if(i>0) { //경로가 인자로 주어졌으면
			//printf("path: %s\n", pentry->d_name);
				strcpy(f, cwdname);
				strcat(f, "/");
				strcat(f, pentry->d_name);
				//printf("%s\n", f);
				if(lstat(f, &buf)<0) {
					printf("stat error\n"); continue;
				}
			}
			else { //현재 워킹 디렉토리이면
				if(lstat(pentry->d_name, &buf)<0) {
					printf("stat error\n"); exit(0);
				}
			}
			if(S_ISDIR(buf.st_mode)) {
				printf("d");
			}
			else if(S_ISREG(buf.st_mode)) {
				printf("-");
			}
			else if(S_ISLNK(buf.st_mode)) {
				printf("l");
			}
			else if(S_ISBLK(buf.st_mode)) {
				printf("b");
			}
			else if(S_ISCHR(buf.st_mode)) {
				printf("c");
			}
			else if(S_ISFIFO(buf.st_mode)) {
				printf("p");
			}
		if(S_IRUSR & buf.st_mode) { printf("r"); } else {printf("-");}
		if(S_IWUSR & buf.st_mode) { printf("w"); } else {printf("-");}
		if(S_IXUSR & buf.st_mode) { printf("x"); } else {printf("-");}

		if(S_IRGRP & buf.st_mode) { printf("r"); } else {printf("-");}
		if(S_IWGRP & buf.st_mode) { printf("w"); } else {printf("-");}
		if(S_IXGRP & buf.st_mode) { printf("x"); } else {printf("-");}

		if(S_IROTH & buf.st_mode) { printf("r"); } else {printf("-");}
		if(S_IWOTH & buf.st_mode) { printf("w"); } else {printf("-");}
		if(S_IXOTH & buf.st_mode) { printf("x "); } else {printf("- ");}
		
		printf("%d ", buf.st_nlink);
		//id로 이름 얻기
		struct passwd *pwd;
		struct group *g;
		pwd = getpwuid(buf.st_uid);
		g = getgrgid(buf.st_gid);
		printf("%s ", pwd->pw_name);
		printf("%s ", g->gr_name);
		
		if(S_ISCHR(buf.st_mode)||S_ISBLK(buf.st_mode)) //디바이스 파일이면 major, minor
		{ printf("%d:%d", major(buf.st_rdev), minor(buf.st_rdev)); }
		else { printf("%d	", buf.st_size); } //다른 파일은 사이즈 출력
		
		time_t mtime = buf.st_mtime; //시간 형식에 맞춰서 출력
		char buff[sizeof "2011-10-08T07:07:07Z"];
   		strftime(buff, sizeof buff, "%Y-%m-%d %H:%M", localtime(&mtime));
		printf("%s ", buff);
		printf("%s ", pentry->d_name);
		printf("\n");
		}
	}
}

