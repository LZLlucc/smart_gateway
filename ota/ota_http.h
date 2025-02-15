#ifndef OTA_HTTP_H_
#define OTA_HTTP_H_

#define OTA_URL_FILEINFO "http://192.168.55.52:8000/fileinfo.json"
#define OTA_URL_DOWNLOAD "http://192.168.55.52:8000/download/gateway"
#define OTA_LOCAL_FILE_PATH                                                    \
	"/home/lzl/workshop/linux/smart_gateway/gateway.update" /* test */

char *ota_http_get_json(char *url);
int ota_http_download(char *url, char *filename);

#endif /* OTA_HTTP_H_ */