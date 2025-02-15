#include "ota_version.h"
#include "cJSON/cJSON.h"
#include "log/log.h"
#include "ota_http.h"
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/reboot.h>

/**
 * get_file_sha - 获取文件SHA1哈希值(40位16进制字符串)
 * 
 * @filepath: 文件路径
 * 
 * Return: sha值
 * 
 * note: 相同文件返回相同的哈希值 可用于判断文件是否相同
 */
static char *get_file_sha(char *filepath)
{
    FILE *file = fopen(filepath, "rb");
    if (!file)
    {
        perror("Failed to open file");
        return NULL;
    }

    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA_CTX sha1;
    SHA1_Init(&sha1);

    const int buf_size = 32768;
    unsigned char *buffer = (unsigned char *)malloc(buf_size);
    if (!buffer)
    {
        perror("Failed to allocate memory");
        fclose(file);
        return NULL;
    }

    int bytes_read;
    while ((bytes_read = fread(buffer, 1, buf_size, file)) > 0)
    {
        SHA1_Update(&sha1, buffer, bytes_read);
    }

    SHA1_Final(hash, &sha1);
    fclose(file);
    free(buffer);

    char *output_buffer = (char *)malloc(SHA_DIGEST_LENGTH * 2 + 1);
    if (!output_buffer)
    {
        perror("Failed to allocate memory");
        return NULL;
    }

    for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
    {
        sprintf(output_buffer + (i * 2), "%02x", hash[i]);
    }

    return output_buffer;
}

/* 版本检查并更新 */
static int ota_version_check_update()
{
	/* 1.获取远程的版本信息 */
	char *json = ota_http_get_json(OTA_URL_FILEINFO);
	if (!json) {
		log_error("get remote info failed");
		return -1;
	}

	cJSON *root = cJSON_Parse(json);
	if (!root) {
		log_error("parse json filded");
		return -1;
	}
	int major = cJSON_GetObjectItem(root, "major")->valueint;
	int minor = cJSON_GetObjectItem(root, "minor")->valueint;
	int patch = cJSON_GetObjectItem(root, "patch")->valueint;
	log_debug("romote version: %d.%d.%d", major, minor, patch);
	log_info("the current version is: %d.%d.%d", 
				VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

	/* 2.比较版本 */
	if (VERSION_MAJOR > major ||
		(VERSION_MAJOR == major && VERSION_MINOR > minor) ||
		(VERSION_MAJOR == major && VERSION_MINOR == minor &&
			VERSION_PATCH >= patch)) {
		log_info("the current version is already the latest version");
		return 0;
	}
    log_info("find the least version: %d.%d.%d", major, minor, patch);

	/* 3.下载固件 */
    unlink(OTA_LOCAL_FILE_PATH); /* 删除旧的本地文件 */
    int ret = ota_http_download(OTA_URL_DOWNLOAD, OTA_LOCAL_FILE_PATH);
    if (ret != 0) {
        log_error("download failde");
        return -1;
    }

	/* 4.比较hash值 */
	char *local_hash = get_file_sha(OTA_LOCAL_FILE_PATH);
	char *remote_hash = cJSON_GetObjectItem(root, "sha1")->valuestring;
	log_debug("local_hash: %s", local_hash);
	log_debug("remote_hash: %s", remote_hash);
	if (strcmp(local_hash, remote_hash) != 0) {
		log_error("the local file is different from the remote file");
		unlink(OTA_LOCAL_FILE_PATH);
		free(local_hash);
		free(json);
		cJSON_Delete(root);
		return -1;
	}

	/* 更新成功 */
	log_info("update success");
	free(local_hash);
	free(json);
	cJSON_Delete(root);

	/* 6.重启*/
	reboot(RB_AUTOBOOT);

	return 0;
}

/* 每隔一天自动检查更新 */
void ota_version_check_update_daily()
{
	while (1) {
		ota_version_check_update();
		sleep(24 * 60 * 60);
	}
}