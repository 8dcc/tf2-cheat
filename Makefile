
CC=gcc
CFLAGS=-Wall -Wextra -Ofast -m32 -fPIC
LDFLAGS=-lm -lSDL2 -lGLEW
INJECTOR_FLAGS=

MAIN_OBJS=main.c.o sdk.c.o globals.c.o hooks.c.o util.c.o settings.c.o menu.c.o playerlist.c.o
DEPS_OBJS=cJSON/cJSON.c.o
FEATURES_OBJS=esp.c.o chams.c.o aim/aimbot.c.o aim/meleebot.c.o aim/medigun.c.o antiaim.c.o movement.c.o misc.c.o visuals.c.o prediction.c.o auto_detonate_stickies.c.o

OBJS=$(addprefix obj/, $(MAIN_OBJS)) \
     $(addprefix obj/dependencies/, $(DEPS_OBJS)) \
     $(addprefix obj/features/, $(FEATURES_OBJS))

BIN=libenoch.so

.PHONY: clean all inject debug-flags debug

# -------------------------------------------

all: $(BIN)

clean:
	rm -f $(OBJS)
	rm -f $(BIN)

inject: $(BIN)
	bash ./inject.sh $(INJECTOR_FLAGS)

debug-flags:
	$(eval CFLAGS += -g)
	$(eval INJECTOR_FLAGS += debug)

debug: debug-flags all

# -------------------------------------------

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -shared -o $@ $(OBJS) $(LDFLAGS)

$(OBJS): obj/%.c.o : src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)
