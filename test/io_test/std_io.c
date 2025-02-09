#include <stdio.h>
#include <unistd.h>

/**
 * 标准输入输出流（FILE *)
 *    FILE * stdin  标准输入流
 *    FILE * stdout 标准输出流
 *    FILE * stderr 标准错误流
 * 
 * 标准输入输出流文件描述符
 *    STDIN_FILENO  0 标准输入
 *    STDOUT_FILENO 1 标准输出
 *    STDERR_FILENO 2 标准错误
 */
int main(int argc, char **argv)
{
    /* 使用标准输入输出流读取控制台数据 */
    char input_buf[10] = {0};
    fgets(input_buf, 10, stdin);     /* 会自动在末尾添加空字符'\0'形成字符串 */
    printf("input: %s", input_buf);

    fputs("hello, world\n", stdout);
    fputs("hello, world\n", stderr);

    /* 使用标准输入输出流文件描述符写入控制台数据 */
    char input_buf1[10] = {0};
    ssize_t len = read(STDIN_FILENO, input_buf1, 10);
    printf("input2: %.*s", (int)len, input_buf1);
    write(STDOUT_FILENO, "hello, world\n", 14);
    write(STDERR_FILENO, "hello, world\n", 14);

    // char input_buf2[10] = {0};
    // fread(input_buf2, 1, 10, stdin);
    // printf("input3: %s", input_buf2);

    return 0;
}