#include "ota_http.h"
#include "log/log.h"

int main(int argc, char **argv)
{
    char *json = ota_http_get_json(OTA_URL_FILEINFO);
    log_info("json: %s", json);

    int ret = ota_http_download(OTA_URL_DOWNLOAD, OTA_LOCAL_FILE_PATH);
    if (0 == ret)
        log_info("download success");
    else
        log_error("download failed");
    return 0;
}