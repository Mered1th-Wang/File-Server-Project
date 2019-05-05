SOURCES:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SOURCES))
ELF:=server
CC:=gcc
CFLAGS:= -pthread
$(ELF):$(OBJS)
	gcc $^ -o $@ $(CFLAGS)
clean:
	rm -rf $(OBJS) $(ELF)
