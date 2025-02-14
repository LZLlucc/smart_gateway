#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
    printf("parent process(%d) begin\n", getpid());

    pid_t pid = fork();
    if (pid < 0) {
        printf("fork error\n");
        return -1;
    } else if (pid == 0) {
        printf("child process(%d) begin\n", getpid());

        /* 通过execve执行ping ping -c 10 www.baidu.com */
        char *path = "/bin/ping";
        char *argv[] = {path, "-c", "10", "www.baidu.com", NULL};
        char *env[] = {NULL};
        execve(path, argv, env);
        printf("child process(%d) fail\n", getpid());
    } else {
        printf("parent process(%d) continue\n", getpid());

        /* 等待子进程结束 */
        int ret;
        waitpid(pid, &ret, 0);

        printf("child process(%d) end, ret = %d\n", getpid(), ret);
    }

    printf("process(%d) end\n", getpid());
    return 0;
}