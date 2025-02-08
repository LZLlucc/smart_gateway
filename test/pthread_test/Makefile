CC = gcc
CFLAGS = -Wall -g -O0 -lpthread

create: create.c
	$(CC) $(CFLAGS) $^ -o $@
	-./$@
	-rm $@

sum_sync: sum_sync.c
	$(CC) $(CFLAGS) $^ -o $@
	-./$@
	-rm $@

sum_async: sum_async.c
	$(CC) $(CFLAGS) $^ -o $@
	-./$@
	-rm $@

cancel : cancel.c
	$(CC) $(CFLAGS) $^ -o $@
	-./$@
	-rm $@

mutex : metex.c
	$(CC) $(CFLAGS) $^ -o $@
	-./$@
	-rm $@

mqueue : mqueue.c
	$(CC) $(CFLAGS) $^ -o $@
	-./$@
	-rm $@