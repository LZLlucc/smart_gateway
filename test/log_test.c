#include "log/log.h"

int main(int argc, char **argv) {

    /* 设置日志级别 */
    log_set_level(LOG_DEBUG);

    log_trace("trace");
    log_debug("debug");
    log_info("info");
    log_warn("warn");
    log_error("error");
    log_fatal("fatal");
    
    return 0;
}