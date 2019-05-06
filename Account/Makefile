src = $(wildcard *.c)
targets = $(patsubst %.c, %, $(src))

CC = gcc
CFLAGS = -g -lpthread -lcrypt 

all:$(targets)

$(targets):%:%.c
	$(CC) $< -o $@ $(CFLAGS)

.PHONY:clean all
clean:
	-rm -rf $(targets) 
