SOURCES:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SOURCES))
ELF:=Netdisk
CC:=gcc
CFLAGS = -g -Wall -I /usr/include/mysql/ -lmysqlclient
$(ELF):$(OBJS)
	gcc $^ -o $@ $(CFLAGS)
clean:
	rm -rf $(OBJS) $(ELF)
