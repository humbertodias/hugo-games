#all:
#	gcc main.c -Wall -lSDL2 -o main
#debug:
#	gcc main.c -Wall -lSDL2 -g -o main
#mingw:
#	gcc main.c -w -Wl,-subsystem,windows -lmingw32 -lSDL2main -lSDL2 -o main.exe -Wall
#laby:
#	gcc testlaby.c -Wall -lSDL2 -g -o testlaby

CC_FLAGS := -std=c11 -Wall $(shell pkg-config --cflags sdl2 SDL2_mixer SDL2_image)
LD_FLAGS := $(shell pkg-config --libs sdl2 SDL2_mixer SDL2_image) -lm
CC := gcc

all:
	$(CC) main.c -o main $(CC_FLAGS) $(LD_FLAGS)
debug:
	$(CC) main.c -o main $(CC_FLAGS) $(LD_FLAGS) -g

clean:
	find . -name "*.o" -delete
	rm -f main
