#ifndef APP_COMMON_H
#define APP_COMMON_H

long get_current_time();

/* 字符数组和16进制字符串的转换 */
char *chars_to_hexstr(const char *chars, int chars_len);
char *hexstr_to_chars(const char *hexstr, int *chars_len);


#endif /* APP_COMMON_H */
