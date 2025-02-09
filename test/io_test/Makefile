CC := gcc
CFLAGS := -Wall -g -O0

sys_call : sys_call.c
	-$(CC) $(CFLAGS) $^ -o $@
	-./$@
	-rm $@

std_io : std_io.c
	-$(CC) $(CFLAGS) $^ -o $@
	-./$@
	-rm $@

std_io_redirect : std_io_redirect.c
	-$(CC) $(CFLAGS) $^ -o $@
	-./$@
	-rm $@