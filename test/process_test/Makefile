CC := gcc
CFLAGS := -Wall -g -O0

fork : fork.c
	-$(CC) $(CFLAGS) $^ -o $@
	-./$@
	-rm $@

other : other.c
	-$(CC) $(CFLAGS) $^ -o $@

execve : execve.c other
	-$(CC) $(CFLAGS) $< -o $@
	-./$@
	-rm $@ $(word 2, $^)

waitpid : waitpid.c
	-$(CC) $(CFLAGS) $^ -o $@
	-./$@
	-rm $@ 

signal : signal.c
	-$(CC) $(CFLAGS) $^ -o $@
	-./$@
	-rm $@