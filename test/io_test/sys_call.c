#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

/**
 * 利用系统调用实现文件复制 并在控制台输出文件内容
 */
int main(int argc, char **argv)
{
    int fd1 = open("love.txt", O_RDONLY);
    if (-1 == fd1) {
        perror("open");
        return -1;
    }

    int fd2 = open("love_copy.txt", O_CREAT | O_WRONLY | O_APPEND, 0644);
    if (-1 == fd2) {
        perror("open");
        return -1;
    }

    char buf[1024] = {0};
    int len = 0;
    /* 边读边写 */
    while ((len = read(fd1, buf, sizeof(buf))) > 0) {
        write(fd2, buf, len);
        write(STDOUT_FILENO, buf, len);
    }

    close(fd1);
    close(fd2);

    return 0;
}