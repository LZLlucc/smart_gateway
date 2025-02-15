#include "ota_http.h"
#include "log/log.h"
#include <curl/curl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    char *json_buff = (char *)userdata;

    /* 数据的长度 */
    int datas_len = size * nmemb;

    /* 将数据写入容器 */
    memcpy(json_buff, ptr, datas_len);
    json_buff[datas_len] = '\0';

    return datas_len;

}

/* 获取json数据 */
char *ota_http_get_json(char *url)
{
    /* 1.初始化CURL */
    CURL *curl = curl_easy_init();
    if (!curl) {
        log_error("curl init failed");
        return NULL;
    }

    /* 2.设置URL */
    curl_easy_setopt(curl, CURLOPT_URL, url);

    /* 3.设置接收响应数据的回调函数 */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

    /* 4.指定接收响应数据的容器 传给回调函数 一旦接收到数据 将数据写入容器 */
    char *json_buff = (char *)malloc(1024); /* 假定数据长度不会超过1024 */
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)json_buff);

    /* 5.执行请求 */
    CURLcode res = curl_easy_perform(curl);

    /* 6.释放资源 */
    if (res != CURLE_OK) {
        log_error("ota_get_http failed: %s", curl_easy_strerror(res));
        free(json_buff);
        curl_easy_cleanup(curl);
        return NULL;
    }

    curl_easy_cleanup(curl);
    return json_buff;
}

/* 下载文件 */
int ota_http_download(char *url, char *filename)
{
    /* 初始化CURL */
    CURL *curl = curl_easy_init();
    if (curl == NULL) {
        log_error("curl init failed");
        return -1;
    }

    /* 设置URL */
    curl_easy_setopt(curl, CURLOPT_URL, url);

    /* 设置接收数据的回调函数 */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite);

    /* 指定得到返回文件的回调函数 即文件指针 */
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        log_error("open %s failed", filename);
        return -1;
    }
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)file);

    /* 执行请求 */
    CURLcode res = curl_easy_perform(curl);

    /* 释放资源 */
    if (res != CURLE_OK) {
        log_error("ota_get_http failed: %s", curl_easy_strerror(res));
        fclose(file);
        curl_easy_cleanup(curl);
        return -1;
    }

    fclose(file);
    curl_easy_cleanup(curl);

    return 0;
}