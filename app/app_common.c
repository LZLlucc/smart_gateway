#include <sys/time.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/**
 * @brief 获取当前时间戳-毫秒级
 * 
 * @return 当前时间戳-毫秒级
 */
long get_current_time() 
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

/**
 * @brief 字符数组转换为16进制字符串
 * 
 * @param chars 字符数组
 * @param chars_len 字符数组长度
 * @return char* 16进制字符串
 */
char *chars_to_hexstr(const char *chars, int chars_len)
{
    char *hexstr = (char *)malloc(chars_len * 2 + 1);
    if (NULL == hexstr) {
        return NULL;
    }

    for (int i = 0; i < chars_len; i++) {
        sprintf(hexstr + i * 2, "%02X", chars[i]);
    }

    hexstr[chars_len * 2] = '\0';

    return hexstr;
}

/**
 * @brief 16进制字符串转换为字符数组
 * 
 * @param hexstr 16进制字符串
 * @param(out) chars_len 字符数组长度的指针
 * @return char* 字符数组
 */
char *hexstr_to_chars(const char *hexstr, int *chars_len)
{
    if (!hexstr || !chars_len) {
        return NULL;
    }
    
    int len = strlen(hexstr);
    if (len % 2 != 0) {
        return NULL;
    }

    *chars_len = len / 2;
    char *chars = (char *)malloc(*chars_len);
    if (NULL == chars) {
        return NULL;
    }

    for (int i = 0; i < *chars_len; i++) {
        sscanf(hexstr + i * 2, "%2hhx", chars + i);
    }

    return chars;
}