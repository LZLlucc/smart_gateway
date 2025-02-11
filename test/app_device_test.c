#include "app_device.h"
#include "app_pool.h"
#include "app_mqtt.h"
#include "app_bt.h"
#include <unistd.h>

int main(int argc, char **argv) 
{
    app_pool_init(5);
    app_mqtt_init();

    device_t *device = app_device_init("test_dev_file");
    app_bt_init(device);
    app_device_start();
    sleep(10000);

    // app_mqtt_close();
    // app_pool_close();
    // app_device_close();
    return 0;
}