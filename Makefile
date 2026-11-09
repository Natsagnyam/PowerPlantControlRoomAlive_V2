CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude -D_DEFAULT_SOURCE
LDFLAGS = -lpthread -lm

TARGET = bbb_gen_ctrl
SRCS = src/main.c src/generator.c src/sysfs_led.c src/pss_filter.c src/telemetry.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)

.PHONY: all clean
