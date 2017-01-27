CC = gcc
CFLAGS = -O3 -s -shared
LIBS = -lmingw32 -lSDL2_mixer -lSDL2main -lSDL2

bin/a.dll: src/main.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)