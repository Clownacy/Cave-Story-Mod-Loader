MOD_LOADER_VERSION = v1.4
COMMON_PATH = src/common

ALT_MUSIC_USE_VORBISFILE = true
ALT_MUSIC_USE_FLAC = true
ALT_MUSIC_USE_SNDFILE = false
ALT_MUSIC_USE_OPENMPT = true
# Can be 'mini_al', 'SDL2', or 'Cubeb'
ALT_MUSIC_BACKEND = mini_al

CFLAGS = -O3 -static -Wall -Wextra -std=c99 -fno-ident
ALL_CFLAGS = -I$(COMMON_PATH) -D'MOD_LOADER_VERSION="$(MOD_LOADER_VERSION)"' $(CFLAGS)

SDL2_CFLAGS = $(shell sdl2-config --cflags)
SDL2_LIBS = $(shell sdl2-config --static-libs)

MOD_LOADER_HELPER_OBJECT = bin/mod_loader_helper.o

MOD_LOADER_PATH = src/mod_loader
MOD_LOADER_SOURCES = \
	$(COMMON_PATH)/sprintfMalloc.c \
	$(MOD_LOADER_PATH)/fix_door_bug.c \
	$(MOD_LOADER_PATH)/hooks.c \
	$(MOD_LOADER_PATH)/inih/ini.c \
	$(MOD_LOADER_PATH)/log.c \
	$(MOD_LOADER_PATH)/main.c \
	$(MOD_LOADER_PATH)/mod_list.c \
	$(MOD_LOADER_PATH)/patch.c \
	$(MOD_LOADER_PATH)/redirect_org_files.c \
	$(MOD_LOADER_PATH)/settings.c

MODS_PATH = src/example_mods

GRAPHICS_ENHANCEMENT_PATH = $(MODS_PATH)/graphics_enhancement
GRAPHICS_ENHANCEMENT_FILES = \
	$(GRAPHICS_ENHANCEMENT_PATH)/common.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/main.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/60fps/60fps.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/fullscreen/fullscreen.c \
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
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_sand_zone_enemies.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_screen_flash.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_scrolling_clouds.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_teleport_screen.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_text_box.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_tile_drawers.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_title_screen.c \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/widescreen.c

ALT_MUSIC_PATH = $(MODS_PATH)/alternate_music
ALT_MUSIC_SOURCES = \
	$(COMMON_PATH)/sprintfMalloc.c \
	$(ALT_MUSIC_PATH)/decoder.c \
	$(ALT_MUSIC_PATH)/decoder_predecode.c \
	$(ALT_MUSIC_PATH)/decoder_split.c \
	$(ALT_MUSIC_PATH)/main.c \
	$(ALT_MUSIC_PATH)/memory_file.c \
	$(ALT_MUSIC_PATH)/playlist.c

LIBVORBISFILE_LIBS = -lvorbisfile -lvorbis -logg
LIBFLAC_LIBS = -lFLAC -logg
LIBSNDFILE_LIBS = -lsndfile -lspeex -lFLAC -lvorbisenc -lvorbis -logg
LIBOPENMPT_LIBS = -lopenmpt -lstdc++ -lz -lvorbisfile -lvorbis -logg

ifeq ($(ALT_MUSIC_USE_VORBISFILE)$(ALT_MUSIC_USE_SNDFILE), truefalse)
ALT_MUSIC_SOURCES += $(ALT_MUSIC_PATH)/decoder_vorbisfile.c
ALT_MUSIC_CFLAGS += -DUSE_VORBISFILE
ALT_MUSIC_LIBS += $(LIBVORBISFILE_LIBS)
endif

ifeq ($(ALT_MUSIC_USE_FLAC)$(ALT_MUSIC_USE_SNDFILE), truefalse)
ALT_MUSIC_SOURCES += $(ALT_MUSIC_PATH)/decoder_flac.c
ALT_MUSIC_CFLAGS += -DUSE_FLAC
ALT_MUSIC_LIBS += $(LIBFLAC_LIBS)
endif

ifeq ($(ALT_MUSIC_USE_SNDFILE), true)
ALT_MUSIC_SOURCES += $(ALT_MUSIC_PATH)/decoder_sndfile.c
ALT_MUSIC_CFLAGS += -DUSE_SNDFILE
ALT_MUSIC_LIBS += $(LIBSNDFILE_LIBS)
endif

ifeq ($(ALT_MUSIC_USE_OPENMPT), true)
ALT_MUSIC_SOURCES += $(ALT_MUSIC_PATH)/decoder_openmpt.c
ALT_MUSIC_CFLAGS += -DUSE_OPENMPT
ALT_MUSIC_LIBS += $(LIBOPENMPT_LIBS)
endif

ifeq ($(ALT_MUSIC_BACKEND), mini_al)
ALT_MUSIC_SOURCES += $(ALT_MUSIC_PATH)/backend_mini_al.c
else ifeq ($(ALT_MUSIC_BACKEND), SDL2)
ALT_MUSIC_SOURCES += $(ALT_MUSIC_PATH)/backend_SDL2.c
ALT_MUSIC_CFLAGS += $(SDL2_CFLAGS)
ALT_MUSIC_LIBS += $(SDL2_LIBS)
else ifeq ($(ALT_MUSIC_BACKEND), Cubeb)
ALT_MUSIC_SOURCES += $(ALT_MUSIC_PATH)/backend_cubeb.c
ALT_MUSIC_LIBS += -lcubeb -lole32 -lavrt -lwinmm -luuid -lstdc++
endif

OUTPUT = \
	bin/dsound.dll \
	bin/mods/mod_loader.dll \
	bin/mods/alternate_music/alternate_music.dll \
	bin/mods/sdl_controller_input/sdl_controller_input.dll \
	bin/mods/wasd_input/wasd_input.dll \
	bin/mods/ikachan_cursor/ikachan_cursor.dll \
	bin/mods/debug_save/debug_save.dll \
	bin/mods/graphics_enhancement/graphics_enhancement.dll \
	bin/mods/3ds_hud/3ds_hud.dll \
	bin/mods/disable_image_protection/disable_image_protection.dll \
	bin/mods/tsc_nonod/tsc_nonod.dll \
	bin/mods/tsc_mbx/tsc_mbx.dll

all: $(OUTPUT)

$(MOD_LOADER_HELPER_OBJECT): $(COMMON_PATH)/mod_loader.c
	@mkdir -p $(@D)
	@$(CC) $(ALL_CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS) -c

bin/dsound.dll: src/mod_loader_bootstrapper/main.c $(COMMON_PATH)/sprintfMalloc.c
	@mkdir -p $(@D)
	@$(CC) $(ALL_CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS) -shared
	@strip $@ --strip-unneeded

bin/mods/mod_loader.dll: $(MOD_LOADER_SOURCES)
	@mkdir -p $(@D)
	@$(CC) $(ALL_CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS) -shared -DINI_ALLOW_MULTILINE=0 -DINI_USE_STACK=0
	@strip $@ --strip-unneeded

bin/mods/alternate_music/alternate_music.dll: $(MOD_LOADER_HELPER_OBJECT) $(ALT_MUSIC_SOURCES)
	@mkdir -p $(@D)
	@$(CC) $(ALT_MUSIC_CFLAGS) $(ALL_CFLAGS) -o $@ $^ -shared $(LDFLAGS) $(ALT_MUSIC_LIBS) $(LIBS)
	@strip $@ --strip-unneeded

bin/mods/sdl_controller_input/sdl_controller_input.dll: $(MOD_LOADER_HELPER_OBJECT) src/example_mods/sdl_controller_input/main.c $(COMMON_PATH)/sprintfMalloc.c
	@mkdir -p $(@D)
	@$(CC) $(SDL2_CFLAGS) $(ALL_CFLAGS) -o $@ $^ -shared $(LDFLAGS) $(SDL2_LIBS) $(LIBS)
	@strip $@ --strip-unneeded

bin/mods/wasd_input/wasd_input.dll: $(MOD_LOADER_HELPER_OBJECT) src/example_mods/wasd_input/main.c
	@mkdir -p $(@D)
	@$(CC) $(ALL_CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS) -shared
	@strip $@ --strip-unneeded

bin/mods/ikachan_cursor/ikachan_cursor.dll: $(MOD_LOADER_HELPER_OBJECT) src/example_mods/ikachan_cursor/main.c
	@mkdir -p $(@D)
	@$(CC) $(ALL_CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS) -shared
	@strip $@ --strip-unneeded

bin/mods/debug_save/debug_save.dll: $(MOD_LOADER_HELPER_OBJECT) src/example_mods/debug_save/main.c
	@mkdir -p $(@D)
	@$(CC) $(ALL_CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS) -shared
	@strip $@ --strip-unneeded

bin/mods/graphics_enhancement/graphics_enhancement.dll: $(MOD_LOADER_HELPER_OBJECT) $(GRAPHICS_ENHANCEMENT_FILES)
	@mkdir -p $(@D)
	@$(CC) $(ALL_CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS) -shared
	@strip $@ --strip-unneeded

bin/mods/3ds_hud/3ds_hud.dll: $(MOD_LOADER_HELPER_OBJECT) src/example_mods/3ds_hud/main.c
	@mkdir -p $(@D)
	@$(CC) $(ALL_CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS) -shared
	@strip $@ --strip-unneeded

bin/mods/disable_image_protection/disable_image_protection.dll: $(MOD_LOADER_HELPER_OBJECT) src/example_mods/disable_image_protection/main.c
	@mkdir -p $(@D)
	@$(CC) $(ALL_CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS) -shared
	@strip $@ --strip-unneeded

bin/mods/tsc_mbx/tsc_mbx.dll: $(MOD_LOADER_HELPER_OBJECT) src/example_mods/tsc_mbx/main.c
	@mkdir -p $(@D)
	@$(CC) $(ALL_CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS) -shared
	@strip $@ --strip-unneeded

bin/mods/tsc_nonod/tsc_nonod.dll: $(MOD_LOADER_HELPER_OBJECT) src/example_mods/tsc_nonod/main.c
	@mkdir -p $(@D)
	@$(CC) $(ALL_CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS) -shared
	@strip $@ --strip-unneeded

clean:
	@rm -f $(MOD_LOADER_HELPER_OBJECT) $(OUTPUT)
