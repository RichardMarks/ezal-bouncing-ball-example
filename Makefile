# Example Makefile
CC := gcc
CFLAGS ?= -I$(HOME)/ezal/include -DDEBUG -O0 -MMD -MP -g $(shell pkg-config \
	allegro-5 allegro_main-5 allegro_font-5 allegro_ttf-5 \
	allegro_image-5 allegro_audio-5 allegro_acodec-5 --cflags)
LDFLAGS ?= -L$(HOME)/ezal/lib $(shell pkg-config \
	allegro-5 allegro_main-5 allegro_font-5 allegro_ttf-5 \
	allegro_image-5 allegro_audio-5 allegro_acodec-5 --libs) -lezal
.PHONY: clean
ezal-bouncing-ball-example: main.o
	@echo "Linking Game"
	$(CC) $^ -o $@ $(LDFLAGS)
main.o: main.c
	@echo "Compiling Game Source"
	$(CC) -c $^ -o $@ $(CFLAGS)
clean:
	@echo "Cleaning Project"
	@$(RM) ezal-bouncing-ball-example main.o main.d
