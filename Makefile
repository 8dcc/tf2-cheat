
CC=gcc
CFLAGS=-Wall -Wextra -m32 -fPIC
LDFLAGS=-lm

OBJS=obj/main.c.o obj/globals.c.o obj/hooks.c.o obj/util.c.o obj/features/movement.c.o
BIN=libtf2cheat.so

.PHONY: clean all inject

# -------------------------------------------

all: $(BIN)

clean:
	rm -f $(OBJS)
	rm -f $(BIN)

inject: $(BIN)
	bash ./inject.sh

# -------------------------------------------

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -shared -o $@ $(OBJS) $(LDFLAGS)

$(OBJS): obj/%.c.o : src/%.c
	@mkdir -p obj/features/
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)
