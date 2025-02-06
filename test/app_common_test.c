#include "app_common.h"
#include "log/log.h"

int main()
{
    long current_time = app_common_get_current_time();
    log_info("current time: %ld", current_time);
    return 0;
}