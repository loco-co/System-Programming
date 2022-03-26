//2021-1/시스템프로그래밍/Simple Shell Programming/황기석
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
int get_token(char*, char**);
void redirect1(char**);
void redirect2(char**);
void redirect3(char**);
int makepipe(char**);
int main(int argc, char* argv[]) {
	while(1) {
		printf("&");
		char in[1024];
		pid_t pid;
		int bg=0;
		int i;
		if(fgets(in, 1023, stdin)==NULL){ return 0; }
		in[strlen(in)-1]='\0'; //중요

		char* arg[16];
		int arc=get_token(in, arg);
		
		if(!strcmp(arg[arc-1], "&")) { //백그라운드면
			bg=1;
			arg[arc-1]=NULL; //마지막거 제거
		 }
		if((pid=fork()) == 0) {
		//arg[i] = NULL; //널필수
		redirect1(arg);
		redirect2(arg);
		redirect3(arg);
		int pcount;
		pcount=makepipe(arg);
		if(pcount) {continue;}
		if(execvp(arg[0], arg) < 0) //child
			perror("error");
		}
		
		else { //parent
			if(!bg) { waitpid(pid, NULL, 0); }
			//printf("from p\n");
			//sleep(1);
		}
	}
}

int get_token(char* com, char** arg) {
	int i=0;
	int count=0;
	arg[0]=strtok(com, " ");
	while(arg[i] != NULL) {
		//printf("arg[%d]: %s\n", i, arg[i]);
		i++;
		arg[i]=strtok(NULL, " "); count++;
	}
	return count;
}

void redirect1(char** arg) {
	int i=0;
	int fd;
	for(i=0;arg[i]!=NULL;i++) {
		if(!strcmp(arg[i], ">")) { break; }
	}

	if(arg[i] && arg[i+1]) {
		if((fd=open(arg[i+1], O_RDWR|O_CREAT,0644))==-1) {
			perror("redi error"); return;
		}

		dup2(fd,1);
		close(fd);

		for(;arg[i]!=NULL;i++) { //뒤에거 땡기기
			arg[i]=arg[i+2];
		}
		arg[i]=NULL;
	}
}

void redirect3(char** arg) {
	int i=0;
	int fd;
	for(i=0;arg[i]!=NULL;i++) {
		if(!strcmp(arg[i], "2>")) {break;}
	}

	if(arg[i] && arg[i+1]) {
		if((fd=open(arg[i+1], O_RDWR|O_CREAT,0644))==-1) {
			perror("redi3 error"); return;
		}

		dup2(fd,2);
		close(fd);

		for(;arg[i]!=NULL;i++) {
			arg[i]=arg[i+2];
		}
		arg[i]=NULL;
	}
}

void redirect2(char** arg) {
	int i=0;
	int fd;
	for(i=0;arg[i]!=NULL;i++){
		if(!strcmp(arg[i], "<")) { break; }
	}

	if(arg[i] && arg[i+1]) {
		if((fd=open(arg[i+1],O_RDONLY))==-1) {
			perror("redi error"); return;
		}

		dup2(fd,0);
		close(fd);

		for(;arg[i]!=NULL;i++) {
			arg[i]=arg[i+2];
		}
		arg[i]=NULL;
	}
}

int makepipe(char** arg) {
	int i,j;
	int pindex[8];
	int p=1;
	int fd[2];
	for(i=0; arg[i]!=NULL; i++) {
	if(!strcmp(arg[i], "|")) {
		pindex[p]=i; p++;
	}
	}
	if(!pindex[1]) { return 0; }

	char* beforp[16];
	char* afterp[16];
	for(i=0;i<pindex[1];i++) {
		beforp[i]=arg[i];
		//printf("be:%s \n", beforp[i]);
	}
	beforp[i]=NULL;
	int k=0;
	for(j=i+1;arg[j]!=NULL;j++) {
		afterp[k]=arg[j];
		k++;
	}
	afterp[k]=NULL;

	pipe(fd);
	pid_t pid1, pid2;
	if((pid1=fork())==0) {
		dup2(fd[1], 1);
		close(fd[1]);
		close(fd[0]);

		execvp(beforp[0], beforp);
		exit(0);
	}
	//int pnum;
	//pnum=makepipe(arg,i+1,fd[0]);
	if((pid2=fork())==0) {
		dup2(fd[0], 0);
		close(fd[0]);
		close(fd[1]);
	
		execvp(afterp[0], afterp);
		exit(0);
	}
	else {
	close(fd[0]); close(fd[1]);
	waitpid(pid2,NULL,0);
	return 1;
	}
}
