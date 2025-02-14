CC := gcc
CFLAGS := -Wall -g -O0
VALGRIND := valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all

log := thirdparty/log/log.h thirdparty/log/log.c
log_test: $(log) test/log_test.c
	-$(CC) $^ -o $@ -Ithirdparty
	-./$@
	-rm $@

cjson := thirdparty/cJSON/cJSON.h thirdparty/cJSON/cJSON.c
cjson_test: $(cjson) $(log) test/cjson_test.c
	-$(CC) $(CFLAGS) $^ -o $@ -Ithirdparty
	-./$@
	-rm $@

app_common := app/app_common.h app/app_common.c 
app_common_test: $(log) $(app_common) test/app_common_test.c
	-$(CC) $(CFLAGS) $^ -o $@ -Iapp -Ithirdparty
	-./$@
	-rm $@

app_message := app/app_message.h app/app_message.c  
app_message_test: $(log) $(cjson) $(app_common)  $(app_message) \
					test/app_message_test.c
	-$(CC) $(CFLAGS) $^ -o $@ -Iapp -Ithirdparty
	-./$@
	-rm $@

mqtt_test : test/mqtt_test.c
	-$(CC) $(CFLAGS) $^ -o $@  -lpaho-mqtt3c
	-./$@
	-rm $@

app_mqtt := app/app_mqtt.h app/app_mqtt.c
app_mqtt_test: $(log) $(app_mqtt) test/app_mqtt_test.c
	-$(CC) $(CFLAGS) $^ -o $@ -Iapp -Ithirdparty -lpaho-mqtt3c
	-./$@
	-rm $@

app_pool := app/app_pool.c app/app_pool.h 
app_pool_test : $(log) $(app_pool) test/app_pool_test.c
	-$(CC) $(CFLAGS) $^ -o $@ -Iapp -Ithirdparty
#	-./$@
	- $(VALGRIND) ./$@
	-rm $@

app_buffer := app/app_buffer.c app/app_buffer.h
app_buffer_test : $(log) $(app_buffer) test/app_buffer_test.c
	-$(CC) $(CFLAGS) $^ -o $@ -Iapp -Ithirdparty
#	-./$@
	- $(VALGRIND) ./$@
	-rm $@

app_device := app/app_device.c app/app_device.h
app_bt := app/app_bt.c app/app_bt.h
app_serial := app/app_serial.c app/app_serial.h
app_device_test : test/app_device_test.c $(app_device) $(log) $(app_buffer) \
				$(app_message) $(app_common) $(cjson) $(app_pool) \
				$(app_mqtt) $(app_message) $(app_bt) $(app_serial)
	-$(CC) $(CFLAGS) $^ -o $@ -Iapp -Ithirdparty -lpaho-mqtt3c
	-./$@
#	- $(VALGRIND) ./$@
	-rm $@