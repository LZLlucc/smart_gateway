#ifndef APP_BT_H_
#define APP_BT_H_

#include "app_device.h"

typedef enum {
    BT_9600 = '4',
    BT_115200 = '8',
}bt_braudrate_t;

int app_bt_init(device_t *device);
int app_bt_post_read(char *datas, int len);
int app_bt_pre_write(char *datas, int len);

int app_bt_test(device_t *device);
int app_bt_rename(device_t *device, const char *new_name);
int app_bt_set_baudrate(device_t *device, bt_braudrate_t baudrate);
int app_bt_reset(device_t *device);
int app_bt_set_netid(device_t *device, const char *netid);
int app_bt_set_maddr(device_t *device, const char *maddr);

#endif /* APP_BT_H_ */