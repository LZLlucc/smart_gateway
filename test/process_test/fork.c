#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	printf("parent process(%d) begin\n", getpid());
	int val = 100;

	pid_t pid = fork();
	if (pid < 0) {
		printf("fork error\n");
		return -1;
	} else if (pid == 0) {
		val++;
		printf("child process(%d) begin, parent process = %d, val = %d, val_addr "
			   "= %p\n",
			   getpid(), getppid(), val, &val);
    } else {
        val--;
        printf("parent process(%d) continue, child process = %d, val = %d, val_addr "
               "= %p\n",
               getpid(), pid, val, &val);
    }

    printf("process(%d) end\n", getpid());

	return 0;
}