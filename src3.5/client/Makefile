SRCS:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SRCS))
ELF:=client
CC:=gcc
CFLAGS:=-g -lcrypt -lpthread -I /usr/include/mysql -lmysqlclient
$(ELF):$(OBJS)
	gcc $^ -o $@ $(CFLAGS)
.PHONY:clean
clean:
	rm -rf $(ELF) $(OBJS)
