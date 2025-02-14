#include "ota_http.h"
#include "log/log.h"
#include <curl/curl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


char *ota_http_get_json(char *url)
{
    /* 1.初始化CURL */
    CURL *curl = curl_easy_init();
    if (curl == NULL) {
        log_error("curl init failed");
        return NULL;
    }

    /* 2.设置URL */
    curl_easy_setopt(curl, CURLOPT_URL, url);

    /* 3.设置回调函数 */
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, stdout);
}
int ota_http_download(char *url, char *filename);