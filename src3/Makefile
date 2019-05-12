SOURCES:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SOURCES))
ELF:=server
CC:=g++
CFLAGS:= -pthread -I /usr/include/mysql/ -lmysqlclient
$(ELF):$(OBJS)
	gcc $^ -o $@ $(CFLAGS)
clean:
	rm -rf $(OBJS) $(ELF)
