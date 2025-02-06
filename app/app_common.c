#include <sys/time.h>
#include "stddef.h"

/**
 * @brief 获取当前时间戳-毫秒级
 * 
 * @return 当前时间戳-毫秒级
 */
long app_common_get_current_time() 
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
