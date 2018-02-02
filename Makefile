MOD_LOADER_VERSION = v1.3+ prototype
COMMON_PATH = src/common

CC = gcc
CFLAGS = -O3 -s -static -Wall -Wextra -std=c11 -Wno-unused-parameter -fomit-frame-pointer -fno-ident -D'MOD_LOADER_VERSION="$(MOD_LOADER_VERSION)"'
LIBS = -I$(COMMON_PATH)

SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LDFLAGS := $(shell sdl2-config --static-libs)

MOD_LOADER_HELPER_OBJECT = bin/mod_loader_helper.o

MOD_LOADER_PATH = src/mod_loader
MOD_LOADER_SOURCES = \
	$(COMMON_PATH)/sprintfMalloc.c \
	$(MOD_LOADER_PATH)/fix_door_bug.c \
	$(MOD_LOADER_PATH)/log.c \
	$(MOD_LOADER_PATH)/main.c \
	$(MOD_LOADER_PATH)/mod_list.c \
	$(MOD_LOADER_PATH)/patch.c \
	$(MOD_LOADER_PATH)/redirect_org_files.c \
	$(MOD_LOADER_PATH)/settings.c \
	$(MOD_LOADER_PATH)/inih/ini.c

GRAPHICS_ENHANCEMENT_PATH = src/example_mods/graphics_enhancement
GRAPHICS_ENHANCEMENT_FILES = \
	$(GRAPHICS_ENHANCEMENT_PATH)/common.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/main.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/borderless_fullscreen/borderless_fullscreen.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/remove_sprite_alignment/remove_sprite_alignment.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/sprite_resolution/sprite_resolution.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/upscale_window/upscale_window.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/black_bars.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/drawsprite1_centred.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/fix_subforeground_bug.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_beetle_and_basu.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_boss_explosion.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_boss_health.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_bute.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_camera.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_credits.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_exit_screen.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_fade.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_fps_counter.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_gaudi.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_inventory_screen.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_island_crash.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_loading_screen.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_map_screen.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_room_name_print.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_screen_flash.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_scrolling_clouds.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_teleport_screen.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_text_box.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_tile_drawers.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_title_screen.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/widescreen.c

all: $(MOD_LOADER_HELPER_OBJECT) bin/mod_loader_patcher.exe bin/dsound.dll bin/mods/mod_loader.dll bin/mods/60fps/60fps.dll bin/mods/ogg_music/ogg_music.dll bin/mods/sdl_controller_input/sdl_controller_input.dll bin/mods/wasd_input/wasd_input.dll bin/mods/ikachan_cursor/ikachan_cursor.dll bin/mods/debug_save/debug_save.dll bin/mods/graphics_enhancement/graphics_enhancement.dll bin/mods/3ds_hud/3ds_hud.dll bin/mods/disable_image_protection/disable_image_protection.dll

$(MOD_LOADER_HELPER_OBJECT): $(COMMON_PATH)/mod_loader.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $^ $(LIBS)

bin/mod_loader_patcher.exe: src/exe_patcher/main.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

bin/dsound.dll: src/mod_loader_bootstrapper/main.c $(COMMON_PATH)/sprintfMalloc.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) -shared

bin/mods/mod_loader.dll: $(MOD_LOADER_SOURCES)
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) -shared -DINI_ALLOW_MULTILINE=0 -DINI_USE_STACK=0

bin/mods/60fps/60fps.dll: $(MOD_LOADER_HELPER_OBJECT) src/example_mods/60fps/main.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) -shared

bin/mods/ogg_music/ogg_music.dll: $(MOD_LOADER_HELPER_OBJECT) src/example_mods/ogg_music/main.c $(COMMON_PATH)/sprintfMalloc.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) -shared -lcubeb -lole32 -lavrt -lwinmm -luuid -lstdc++ -lvorbisfile -lvorbis -logg

bin/mods/sdl_controller_input/sdl_controller_input.dll: $(MOD_LOADER_HELPER_OBJECT) src/example_mods/sdl_controller_input/main.c $(COMMON_PATH)/sprintfMalloc.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) -shared $(SDL_CFLAGS) $(SDL_LDFLAGS)

bin/mods/wasd_input/wasd_input.dll: $(MOD_LOADER_HELPER_OBJECT) src/example_mods/wasd_input/main.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) -shared

bin/mods/ikachan_cursor/ikachan_cursor.dll: $(MOD_LOADER_HELPER_OBJECT) src/example_mods/ikachan_cursor/main.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) -shared

bin/mods/debug_save/debug_save.dll: $(MOD_LOADER_HELPER_OBJECT) src/example_mods/debug_save/main.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) -shared

bin/mods/graphics_enhancement/graphics_enhancement.dll: $(MOD_LOADER_HELPER_OBJECT) $(GRAPHICS_ENHANCEMENT_FILES)
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) -shared

bin/mods/3ds_hud/3ds_hud.dll: $(MOD_LOADER_HELPER_OBJECT) src/example_mods/3ds_hud/main.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) -shared

bin/mods/disable_image_protection/disable_image_protection.dll: $(MOD_LOADER_HELPER_OBJECT) src/example_mods/disable_image_protection/main.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) -shared
