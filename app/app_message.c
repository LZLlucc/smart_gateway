#include <string.h>
#include <stdlib.h>
#include "log/log.h"
#include "cJSON/cJSON.h"
#include "app_common.h"

/**
 * @brief 将消息字符串转换为json格式
 * 
 * @param chars 消息字符数组
 * @param chars_len 消息字符数组长度
 * @return char* json格式消息
 */
char *app_message_chars_to_json(const char *chars, int chars_len)
{
    /* 消息格式 
     * 字符数组 : 连接类型(1) + id长度(1) + 消息体长度(1) + id + 消息体
     * json格式 : {"conn_type": 1, "id": "5858", "msg": "61626364"} - 16进制
     */

    /* 1.提取出连接类型、id长度和消息体长度 */
    int conn_type = chars[0];
    int id_len = chars[1];
    int msg_len = chars[2];

    if (chars_len != 3 + id_len + msg_len) {
        log_error("message format error");
        return NULL;
    }

    /* 2.提取出id和消息体 */
    char id[id_len];
    char msg[msg_len];
    memcpy(id, chars + 3, id_len);
    memcpy(msg, chars + 3 + id_len, msg_len);

    /* 3.将id和消息体转化为16进制 */
    char *id_hex = chars_to_hexstr(id, id_len);
    char *msg_hex = chars_to_hexstr(msg, msg_len);

    /* 4.生成json字符串 */
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "conn_type", conn_type);
    cJSON_AddStringToObject(root, "id", id_hex);
    cJSON_AddStringToObject(root, "msg", msg_hex);
    
    char *json_str = cJSON_PrintUnformatted(root);
    if (json_str == NULL) {
        log_error("create json string failed");
        return NULL;
    }

    cJSON_Delete(root);
    free(id_hex);
    free(msg_hex);

    return json_str;
}

/**
 * @brief 将json格式消息转换为字符串
 * 
 * @param json json格式消息
 * @param(out) chars_buf 字符数组缓冲区
 * @param buf_len 缓冲区长度
 * @return int 字符数组长度
 */
int app_message_json_to_chars(const char *json, char *chars_buf, int buf_len)
{
    /* 1.解析json字符串 */
    cJSON *json_str = cJSON_Parse(json);
    if (json_str == NULL) {
        log_error("parse json failed");
        return -1;
    }

    /* 2.提取出连接类型、id和消息体 */
    int conn_type = cJSON_GetObjectItem(json_str, "conn_type")->valueint;
    char *id = cJSON_GetObjectItem(json_str, "id")->valuestring;
    char *msg = cJSON_GetObjectItem(json_str, "msg")->valuestring;

    /* 3.将16进制的id和msg转化为字符数组 */
    int id_len = -1;
    int msg_len = -1;
    char *id_chars = hexstr_to_chars(id, &id_len);
    char *msg_chars = hexstr_to_chars(msg, &msg_len);

    if (id_len + msg_len > buf_len) {
        log_error("buffer is too small");
        return -1;
    }

    /* 4.拼接消息 */
    memcpy(chars_buf, &conn_type, 1);
    memcpy(chars_buf + 1, &id_len, 1);
    memcpy(chars_buf + 2, &msg_len, 1);
    memcpy(chars_buf + 3, id_chars, id_len);
    memcpy(chars_buf + 3 + id_len, msg_chars, msg_len);

    free(id_chars);
    free(msg_chars);

    return id_len + msg_len + 3;
}