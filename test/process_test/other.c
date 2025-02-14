#include <stdio.h>
#include <unistd.h>

int main(int arc, char **argv)
{
    printf("other(%d) begin, argv[1] = %s\n", getpid(), argv[1]);
    return 0;
}