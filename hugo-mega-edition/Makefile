#all:
#	gcc hugomain.c -Wall -lSDL2 -o hugomain
#debug:
#	gcc hugomain.c -Wall -lSDL2 -g -o hugomain
#mingw:
#	gcc hugomain.c -w -Wl,-subsystem,windows -lmingw32 -lSDL2main -lSDL2 -o hugomain.exe -Wall
#laby:
#	gcc testlaby.c -Wall -lSDL2 -g -o testlaby

CXX_FLAGS := -std=c++17 -Wall $(shell pkg-config --cflags sdl2)
LD_FLAGS := $(shell pkg-config --libs sdl2)
CXX := g++

all:
	$(CXX) hugomain.c -o hugomain $(CXX_FLAGS) $(LD_FLAGS)
debug:
	$(CXX) hugomain.c -o hugomain $(CXX_FLAGS) $(LD_FLAGS) -g

clean:
	find . -name "*.o" -delete
	rm -f hugomain
