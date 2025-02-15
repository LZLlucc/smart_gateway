#include "ota_version.h"
#include "log/log.h"

int main(int argc, char **argv)
{
    ota_version_check_update_daily();
    return 0;
}