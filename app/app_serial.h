#ifndef APP_SERIAL_H_
#define APP_SERIAL_H_

#include "app_device.h"
#include <termios.h>

/* 波特率 */
typedef enum {
    BR_9600 = B9600,
    BR_115200 = B115200
}braudrate_t;

/* 校验位 */
typedef enum {
    PARITY_NONE = 0,
    PARITY_ODD = PARENB | PARODD,
    PARITY_EVEN = PARENB
}parity_t;

/* 停止位 */
typedef enum {
    SB_1 = 0,
    SB_2 = CSTOPB
}stopbit_t;

int app_serial_init(device_t *dev);
int app_serial_set_braudrate(device_t *dev, braudrate_t baudrate);
int app_serial_set_parity(device_t *dev, parity_t parity);
int app_serial_set_stopbit(device_t *dev, stopbit_t stopbit);
int app_serial_set_block(device_t *dev, int block);
int app_serial_set_raw(device_t *dev);

#endif /* APP_SERIAL_H_ */