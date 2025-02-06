#include "cJSON/cJSON.h"
#include "log/log.h"
#include <stdio.h>

int main(int argc, char **argv) 
{
    /* 1.生成一个json字符串 */
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "name", "zhangsan");
    cJSON_AddNumberToObject(root, "age", 20);
    char *json = cJSON_PrintUnformatted(root);
    log_debug("json: %s", json);

    /* 2.解析json字符串 */
    cJSON *root2 = cJSON_Parse(json);
    if (root2 == NULL) {
        log_error("parse json failed");
        return -1;
    }

    cJSON *nameobj = cJSON_GetObjectItem(root2, "name");
    cJSON *ageobj = cJSON_GetObjectItem(root2, "age");
    if (nameobj == NULL || ageobj == NULL) {
        log_error("get json object failed");
        return -1;
    }

    char *name = nameobj->valuestring;
    int age = ageobj->valueint;
    log_debug("name: %s, age: %d", name, age);

    cJSON_Delete(root);
    cJSON_Delete(root2);
    cJSON_free(json);
    
    return 0;
}