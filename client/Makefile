CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11
LDFLAGS = 

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
TARGET = client

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del /F /Q $(OBJS) $(TARGET).exe 2> NUL || rm -f $(OBJS) $(TARGET)

.PHONY: all clean
