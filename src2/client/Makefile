SRCS:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SRCS))
ELF:=client
CC:=gcc
CFLAGS:=-g -lcrypt 
$(ELF):$(OBJS)
	gcc $^ -o $@ $(CFLAGS)
.PHONY:clean
clean:
	rm -rf $(ELF) $(OBJS)
