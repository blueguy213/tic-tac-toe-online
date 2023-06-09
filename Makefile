CC = arch -x86_64 gcc
CFLAGS = -g -std=c99 -Wall -fsanitize=address,undefined

all: directories server client

directories:
	@mkdir -p obj
	@mkdir -p bin

obj/%.o: src/common/%.c src/common/%.h
	$(CC) $(CFLAGS) -c $< -o $@

obj/%.o: src/server/%.c
	$(CC) $(CFLAGS) -c $< -o $@

obj/%.o: src/client/%.c
	$(CC) $(CFLAGS) -c $< -o $@

server: obj/ttts.o obj/game.o obj/player.o obj/utils.o
	$(CC) $(CFLAGS) -o bin/ttts $^

client: obj/ttt.o obj/game.o obj/player.o obj/utils.o
	$(CC) $(CFLAGS) -o bin/ttt $^

clean:
	rm -rf obj bin

.PHONY: all directories server client clean
