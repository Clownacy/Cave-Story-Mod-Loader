CC = gcc
CFLAGS = -O3 -s -shared
LIBS = -Isrc/Common

all: bin/mod_loader.dll bin/mods/60fps.dll bin/mods/wasd_input.dll bin/mods/ogg_music_wiiware.dll bin/mods/ogg_music_3d.dll

bin/mod_loader.dll: src/mod_loader/main.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

bin/mods/60fps.dll: src/60fps/main.c bin/mod_loader.dll
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

bin/mods/wasd_input.dll: src/wasd_input/main.c bin/mod_loader.dll
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

bin/mods/ogg_music_wiiware.dll: src/ogg_music/main.c bin/mod_loader.dll
	$(CC) $(CFLAGS) -DSOUNDTRACK_WIIWARE -o $@ $^ $(LIBS) -lSDL2_mixer -lSDL2

bin/mods/ogg_music_3d.dll: src/ogg_music/main.c bin/mod_loader.dll
	$(CC) $(CFLAGS) -DSOUNDTRACK_3D -o $@ $^ $(LIBS) -lSDL2_mixer -lSDL2
