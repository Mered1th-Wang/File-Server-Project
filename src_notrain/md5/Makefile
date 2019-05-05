SOURCES:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SOURCES))
ELF:=md5
CC:=gcc
CFLAGS:=-D DEBUG -pthread -lcrypto
$(ELF):$(OBJS)
	gcc $^ -o $@ $(CFLAGS)
clean:
	rm -rf $(OBJS) $(ELF)
