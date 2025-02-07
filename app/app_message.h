#ifndef APP_MESSAGE_H_
#define APP_MESSAGE_H_

/* 数据格式转换 */
char *app_message_chars_to_json(const char *chars, int chars_len);
int app_message_json_to_chars(const char *json, char *chars_buf, int buf_len);

#endif /* APP_MESSAGE_H_ */