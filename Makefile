
CC=gcc
CFLAGS=-Wall -Wextra -m32 -fPIC
LDFLAGS=-lm -lSDL2 -lGLEW
INJECTOR_FLAGS=

OBJS=obj/main.c.o obj/globals.c.o obj/hooks.c.o obj/util.c.o obj/settings.c.o obj/menu.c.o obj/features/movement.c.o obj/features/esp.c.o obj/features/misc.c.o
BIN=libenoc.so

.PHONY: clean all inject debug-flags debug

# -------------------------------------------

all: $(BIN)

clean:
	rm -f $(OBJS)
	rm -f $(BIN)

inject: $(BIN)
	bash ./inject.sh $(INJECTOR_FLAGS)

debug_flags:
	$(eval CFLAGS += -g)
	$(eval INJECTOR_FLAGS += debug)

debug: debug_flags all

# -------------------------------------------

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -shared -o $@ $(OBJS) $(LDFLAGS)

$(OBJS): obj/%.c.o : src/%.c
	@mkdir -p obj/features/
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)
