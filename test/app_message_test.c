#include "app_message.h"
#include "cJSON/cJSON.h"
#include "log/log.h"

int main(int argc, char **argv)
{
	/* 1. json -> chars */
    /* message_chars = 124XXabcd chars_len = 9 */
	char *json_str = "{\"conn_type\":1,\"id\":\"5858\",\"msg\":\"61626364\"}";
	char message_chars[100] = {0};
	int chars_len = app_message_json_to_chars(json_str, message_chars, 100);
	log_debug("message_chars: %.*s, chars_len : %d", 
                            chars_len, message_chars, chars_len);
    
    /* 2. chars -> json */
    /* json_str = {"conn_type":1,"id":"5858","msg":"61626364"} */
    char *message_json = app_message_chars_to_json(message_chars, chars_len);
    log_debug("message_json: %s", message_json);

    return 0;
}