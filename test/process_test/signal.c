#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

static int is_running = 1;

void use_resource(void)
{
    printf("use_recource\n");
}

void free_resource(void)
{
    printf("free_resource\n");
}

void handler(int sig)
{
    if (sig == SIGINT) {
        printf("SIGINT\n");
        is_running = 0;
    } else if (sig == SIGTERM) {
        printf("SIGTERM\n");
        is_running = 0;
    }
}

void handler(int sig) {
    if (sig == SIGINT)
    {
        printf("SIGINT\n");
        // exit(EXIT_FAILURE);
        is_running = 0;
    }
    else if (sig == SIGTERM)
    {
        printf("SIGTERM\n");
        is_running = 0;
    }
}

int main(int argc, char **argv)
{
    /* 注册 */
    signal(SIGINT, handler);
    signal(SIGTERM, handler);

    use_resource();

    while (is_running) {
        sleep(1);
        printf("work\n");
    }

    free_resource();

    return 0;
}
