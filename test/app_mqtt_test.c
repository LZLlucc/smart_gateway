#include <unistd.h>
#include <sys/syscall.h>
#include "log/log.h"
#include "app_mqtt.h"

pid_t gettid() {
return syscall(SYS_gettid);
}

int recv_cb(char *datas, int datas_len)
{
    log_warn("the tid = %d, datas: %s, len = %d", gettid(), datas, datas_len);
    return 0;
}

int main(int argc, char *argv[]) 
{
    log_debug("the tid = %d", gettid());
    app_mqtt_init();
    app_mqtt_register_recv_callback(recv_cb);
    app_mqtt_publish("abcd", 4);
    sleep(50);
    log_debug("finished");
    app_mqtt_close();    
}
