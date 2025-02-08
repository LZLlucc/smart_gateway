#include "app_buffer.h"
#include "log/log.h"

int main(int argc, char **argv)
{
    /* 1.初始化缓冲区 */
    buffer_t *buffer = app_buffer_init(15);
    if (!buffer) {
        log_error("app_buffer_init failed");
        return -1;
    }

    /* 2.写入数据 */
    app_buffer_write(buffer, "abcde", 5);
    app_buffer_write(buffer, "123456", 6);
    app_buffer_write(buffer, "hello", 6);

    /* 3.读取数据 */
    char datas_1[10] = {0};
    char datas_2[2] = {0};
    char datas_3[10] = {0};
    char datas_4[10] = {0};

    app_buffer_read(buffer, datas_1, 10);
    log_debug("datas: %s", datas_1);

    app_buffer_read(buffer, datas_2, 2);
    log_debug("datas: %s", datas_2);

    app_buffer_read(buffer, datas_3, 10);
    log_debug("datas: %s", datas_3);

    app_buffer_read(buffer, datas_4, 10);
    log_debug("datas: %s", datas_4);

    /* 4.销毁缓冲区 */
    app_buffer_free(buffer);

    return 0;
}