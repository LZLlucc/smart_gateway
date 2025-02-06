CC = gcc
CFLAGS = -Wall -g -O0
TARGET = main
SRC = main.c hello.c
OBJ = $(SRC:.c=.o)
DEPS = $(SRC:.c=.d)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.d: %.c
	$(CC) -MM $< > $@

-include $(DEPS) 

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJ) $(DEPS)