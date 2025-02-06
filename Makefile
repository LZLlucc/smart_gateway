CC := gcc
CFLAGS := -Wall -g

log := thirdparty/log/log.h thirdparty/log/log.c
log_test: $(log) test/log_test.c
	-$(CC) $^ -o $@ -Ithirdparty
	-./$@
	-rm $@

cjosn := thirdparty/cJSON/cJSON.h thirdparty/cJSON/cJSON.c
cjson_test: $(cjosn) $(log) test/cjson_test.c
	-$(CC) $(CFLAGS) $^ -o $@ -Ithirdparty
#	-./$@
#	-rm $@