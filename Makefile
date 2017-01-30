CC = gcc
CFLAGS = -O3 -s -shared
LIBS = -lmingw32 -lSDL2_mixer -lSDL2main -lSDL2
OBJECTS = obj/main.o obj/60fps.o obj/input.o obj/music.o

all: $(OBJECTS) bin/a.dll

obj/main.o: src/main.c
	$(CC) -c $(CFLAGS) -o $@ $^

obj/60fps.o: src/60fps.c
	$(CC) -c $(CFLAGS) -o $@ $^

obj/input.o: src/input.c
	$(CC) -c $(CFLAGS) -o $@ $^

obj/music.o: src/music.c
	$(CC) -c $(CFLAGS) -o $@ $^

bin/a.dll: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
