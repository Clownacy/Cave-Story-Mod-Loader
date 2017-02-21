CC = gcc
CFLAGS = -O3 -s -shared
LIBS = -Isrc/common

SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LDFLAGS := $(shell sdl2-config --libs)

WIDESCREEN_FILES = \
	src/example_mods/widescreen/common.c \
	src/example_mods/widescreen/drawsprite1_centred.c \
	src/example_mods/widescreen/fix_subforeground_bug.c \
	src/example_mods/widescreen/main.c \
	src/example_mods/widescreen/patch_beetle_and_basu.c \
	src/example_mods/widescreen/patch_boss_explosion.c \
	src/example_mods/widescreen/patch_boss_health.c \
	src/example_mods/widescreen/patch_camera.c \
	src/example_mods/widescreen/patch_exit_screen.c \
	src/example_mods/widescreen/patch_fade.c \
	src/example_mods/widescreen/patch_inventory_screen.c \
	src/example_mods/widescreen/patch_island_crash.c \
	src/example_mods/widescreen/patch_loading_screen.c \
	src/example_mods/widescreen/patch_map_screen.c \
	src/example_mods/widescreen/patch_room_name_print.c \
	src/example_mods/widescreen/patch_screen_flash.c \
	src/example_mods/widescreen/patch_scrolling_clouds.c \
	src/example_mods/widescreen/patch_teleport_screen.c \
	src/example_mods/widescreen/patch_text_box.c \
	src/example_mods/widescreen/patch_tile_drawers.c \
	src/example_mods/widescreen/patch_title_screen.c

all: bin/mod_loader.dll bin/mods/60fps.dll bin/mods/ogg_music_wiiware.dll bin/mods/ogg_music_3d.dll bin/mods/sdl_controller_input.dll bin/mods/wasd_input.dll bin/mods/ikachan_cursor.dll bin/mods/debug_save.dll bin/mods/widescreen.dll

bin/mod_loader.dll: src/mod_loader/main.c src/mod_loader/patch.c src/mod_loader/fix_door_bug.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

bin/mods/60fps.dll: src/example_mods/60fps/main.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

bin/mods/ogg_music_wiiware.dll: src/example_mods/ogg_music/main.c
	$(CC) $(CFLAGS) -DSOUNDTRACK_WIIWARE -o $@ $^ $(LIBS) $(SDL_CFLAGS) $(SDL_LDFLAGS) -lSDL2_mixer

bin/mods/ogg_music_3d.dll: src/example_mods/ogg_music/main.c
	$(CC) $(CFLAGS) -DSOUNDTRACK_3D -o $@ $^ $(LIBS) $(SDL_CFLAGS) $(SDL_LDFLAGS) -lSDL2_mixer

bin/mods/sdl_controller_input.dll: src/example_mods/sdl_controller_input/main.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) -lSDL2

bin/mods/wasd_input.dll: src/example_mods/wasd_input/main.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

bin/mods/ikachan_cursor.dll: src/example_mods/ikachan_cursor/main.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

bin/mods/debug_save.dll: src/example_mods/debug_save/main.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

bin/mods/widescreen.dll: $(WIDESCREEN_FILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
