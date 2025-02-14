#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	printf("parent process(%d) begin\n", getpid());;

	pid_t pid = fork();
	if (pid < 0) {
		printf("fork error\n");
		return -1;
	} else if (pid == 0) {
		printf("child process(%d) begin",getpid());
        char *path = "./other";
        char *argv[] = {path, "abc", NULL}; /* 最后一个元素必须为NULL */
        char *env[] = {NULL};   /* 环境变量 */
        execve(path, argv, env); /* 目标正确执行不会返回 否则返回-1 */
        printf("execve fail\n");
    } else {
        printf("parent process(%d) continue\n", getpid());
    }

    printf("process(%d) end\n", getpid());
    
	return 0;
}