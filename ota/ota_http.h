#ifndef OTA_HTTP_H_
#define OTA_HTTP_H_

#define OTA_URL_FILEINFO "https://192.168.55.52:8080/fileinfo.json"
#define OTA_URL_DOWNLOAD "https://192.168.55.52:8080/download/gataway"
#define OTA_LOCAL_FILE_PATH "/home/lzl/gataway.update"

char *ota_http_get_json(char *url);
int ota_http_download(char *url, char *filename);

#endif /* OTA_HTTP_H_ */