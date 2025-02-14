#include "app_runner.h"
#include "app_pool.h"
#include "app_mqtt.h"
#include "app_device.h"
#include "app_bt.h"
#include "log/log.h"
#include <signal.h>
#include <unistd.h>

static int is_running = 1;

void exit_handler(int sig)
{
    if (sig == SIGINT || sig == SIGTERM)
    log_info("exit_handler");
    is_running = 0;
}

int app_runner_run()
{
    /* 注册信号处理函数 */
    signal(SIGINT, exit_handler);
    signal(SIGTERM, exit_handler);

    int ret = 0;
    ret = app_pool_init(5);
    if (ret != 0) {
        log_error("app_pool_init failed");
        return -1;
    }else {
        log_info("app_pool_init success");
    }

    ret = app_mqtt_init();
    if (ret != 0) {
        log_error("app_mqtt_init failed");
        return -1;
    }else {
        log_info("app_mqtt_init success");
    }

	device_t *device = app_device_init(DEV_FILE);
	if (!device) {
        log_error("app_device_init failed");
        return -1;
    }else {
        log_info("app_device_init success");
    }

    ret = app_bt_init(device);
    if (ret != 0) {
        log_error("app_bt_init failed");
        return -1;
    }else {
        log_info("app_bt_init success");
    }

    app_device_start();

    while (is_running) {
        sleep(1);
    }

    /* 释放资源 */
    app_mqtt_close();
    app_pool_close();
    app_device_destroy();
    return 0;
}