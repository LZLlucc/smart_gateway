#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

/**
 * 将输入到控制台的数据重定向到文件
 */
int main(int argc, char **argv)
{
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    open("log.txt", O_CREAT | O_WRONLY, 0644);
    open("log_err.txt", O_CREAT | O_WRONLY, 0644);

    /* 使用输出输入流 */
    char input1[10] = {0};
    fgets(input1, 10, stdin);
    printf("input1: %s", input1);

    fputs("stdout_1\n", stdout);
    fflush(stdout);                 /* 刷新缓冲区 */

    fputs("stderr_1\n", stderr);

    /* 使用文件描述符 */
    char input2[10] = {0};
    ssize_t len = read(STDIN_FILENO, input2, 10);
    printf("input2: %.*s", (int)len, input2);

    // fflush(NULL);
    write(STDOUT_FILENO, "stdout_2\n", 10);
    write(STDERR_FILENO, "stderr_2\n", 10);

    return 0;

}