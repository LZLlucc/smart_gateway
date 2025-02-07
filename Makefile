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

app_common := app/app_common.h app/app_common.c 
app_common_test: $(log) $(app_common) test/app_common_test.c
	-$(CC) $(CFLAGS) $^ -o $@ -Iapp -Ithirdparty
	-./$@
	-rm $@

app_message := app/app_message.h app/app_message.c  
app_message_test: $(log) $(cjosn) $(app_common)  $(app_message) \
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