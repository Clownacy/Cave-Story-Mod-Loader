# ====================
# Global
# ====================

MOD_LOADER_VERSION = v1.5.0.1
COMMON_PATH = common
MODS_PATH = example_mods

ALT_MUSIC_USE_VORBISFILE = true
ALT_MUSIC_USE_IVORBISFILE = false
ALT_MUSIC_USE_FLAC = true
ALT_MUSIC_USE_SNDFILE = false
ALT_MUSIC_USE_OPENMPT = true
ALT_MUSIC_USE_SPC = false
ALT_MUSIC_USE_PXTONE = true
# Can be 'mini_al', 'SDL2', or 'Cubeb'
ALT_MUSIC_BACKEND = mini_al

CFLAGS = -O3 -static -Wall -Wextra -std=c99 -fno-ident
CXXFLAGS = -O3 -static -Wall -Wextra -std=c++98 -fno-ident
ALL_CFLAGS = -Isrc/$(COMMON_PATH) -D'MOD_LOADER_VERSION="$(MOD_LOADER_VERSION)"' $(CFLAGS)
ALL_CXXFLAGS = -Isrc/$(COMMON_PATH) -D'MOD_LOADER_VERSION="$(MOD_LOADER_VERSION)"' $(CXXFLAGS)

SDL2_CFLAGS = $(shell sdl2-config --cflags)
SDL2_LIBS = $(shell sdl2-config --static-libs)

obj/%.o: src/%.c
	@mkdir -p $(@D)
	@$(CC) $(ALL_CFLAGS) -o $@ $^ -c

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
	bin/mods/tsc_mbx/tsc_mbx.dll \
	bin/mods/title_background/title_background.dll \
	bin/mods/ghost_mode/ghost_mode.dll

all: $(OUTPUT)

clean:
	@rm -rf obj
	@rm -f $(OUTPUT)

# ====================
# Mod loader
# ====================

MOD_LOADER_PATH = mod_loader
MOD_LOADER_SOURCES = \
	$(COMMON_PATH)/sprintfMalloc \
	$(MOD_LOADER_PATH)/fix_door_bug \
	$(MOD_LOADER_PATH)/hooks \
	$(MOD_LOADER_PATH)/inih/ini \
	$(MOD_LOADER_PATH)/log \
	$(MOD_LOADER_PATH)/main \
	$(MOD_LOADER_PATH)/mod_list \
	$(MOD_LOADER_PATH)/patch \
	$(MOD_LOADER_PATH)/redirect_org_files \
	$(MOD_LOADER_PATH)/settings
MOD_LOADER_OBJECTS = $(addprefix obj/, $(addsuffix .o, $(MOD_LOADER_SOURCES)))

obj/$(MOD_LOADER_PATH)/inih/ini.o: src/$(MOD_LOADER_PATH)/inih/ini.c
	@mkdir -p $(@D)
	@$(CC) $(ALL_CFLAGS) -DINI_ALLOW_MULTILINE=0 -DINI_USE_STACK=0 -o $@ $^ -c

bin/mods/mod_loader.dll: $(MOD_LOADER_OBJECTS)
	@mkdir -p $(@D)
	@$(CC) $(ALL_CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS) -shared
	@strip $@ --strip-unneeded

# ====================
# Bootstrapper
# ====================

BOOTSTRAPPER_PATH = mod_loader_bootstrapper
BOOTSTRAPPER_SOURCES = \
	$(COMMON_PATH)/sprintfMalloc \
	$(BOOTSTRAPPER_PATH)/main
BOOTSTRAPPER_OBJECTS = $(addprefix obj/, $(addsuffix .o, $(BOOTSTRAPPER_SOURCES)))

bin/dsound.dll: $(BOOTSTRAPPER_OBJECTS)
	@mkdir -p $(@D)
	@$(CC) $(ALL_CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS) -shared
	@strip $@ --strip-unneeded

# ====================
# Graphics enhancement
# ====================

GRAPHICS_ENHANCEMENT_PATH = $(MODS_PATH)/graphics_enhancement
GRAPHICS_ENHANCEMENT_SOURCES = \
	$(COMMON_PATH)/mod_loader \
	$(GRAPHICS_ENHANCEMENT_PATH)/common \
	$(GRAPHICS_ENHANCEMENT_PATH)/main \
	$(GRAPHICS_ENHANCEMENT_PATH)/60fps/60fps \
	$(GRAPHICS_ENHANCEMENT_PATH)/fullscreen/fullscreen \
	$(GRAPHICS_ENHANCEMENT_PATH)/remove_sprite_alignment/remove_sprite_alignment \
	$(GRAPHICS_ENHANCEMENT_PATH)/sprite_resolution/sprite_resolution \
	$(GRAPHICS_ENHANCEMENT_PATH)/upscale_window/upscale_window \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/black_bars \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/drawsprite1_centred \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/fix_subforeground_bug \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_beetle_and_basu \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_boss_explosion \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_boss_health \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_bute \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_camera \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_credits \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_exit_screen \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_fade \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_fps_counter \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_gaudi \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_inventory_screen \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_island_crash \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_loading_screen \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_map_screen \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_room_name_print \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_sand_zone_enemies \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_screen_flash \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_scrolling_clouds \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_teleport_screen \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_text_box \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_tile_drawers \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/patch_title_screen \
	$(GRAPHICS_ENHANCEMENT_PATH)/widescreen/widescreen
GRAPHICS_ENHANCEMENT_OBJECTS = $(addprefix obj/, $(addsuffix .o, $(GRAPHICS_ENHANCEMENT_SOURCES)))

bin/mods/graphics_enhancement/graphics_enhancement.dll: $(GRAPHICS_ENHANCEMENT_OBJECTS)
	@mkdir -p $(@D)
	@$(CC) $(ALL_CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS) -shared
	@strip $@ --strip-unneeded

# ====================
# Alternate music
# ====================

ALT_MUSIC_PATH = $(MODS_PATH)/alternate_music
ALT_MUSIC_SOURCES = \
	$(COMMON_PATH)/mod_loader \
	$(COMMON_PATH)/sprintfMalloc \
	$(ALT_MUSIC_PATH)/decoder \
	$(ALT_MUSIC_PATH)/decoders/predecode \
	$(ALT_MUSIC_PATH)/decoders/split \
	$(ALT_MUSIC_PATH)/main \
	$(ALT_MUSIC_PATH)/decoders/memory_file \
	$(ALT_MUSIC_PATH)/playlist

LIBVORBISFILE_LIBS = -lvorbisfile -lvorbis -logg
LIBIVORBISFILE_LIBS = -lvorbisidec -lvorbis -logg
LIBFLAC_LIBS = -lFLAC -logg
LIBSNDFILE_LIBS = -lsndfile -lspeex -lFLAC -lvorbisenc -lvorbis -logg
LIBOPENMPT_LIBS = -lopenmpt -lstdc++ -lz -lvorbisfile -lvorbis -logg
SPC_LIBS = -lstdc++
PXTONE_LIBS = 

ifeq ($(ALT_MUSIC_USE_VORBISFILE)$(ALT_MUSIC_USE_IVORBISFILE)$(ALT_MUSIC_USE_SNDFILE), truefalsefalse)
ALT_MUSIC_SOURCES += $(ALT_MUSIC_PATH)/decoders/vorbisfile
ALT_MUSIC_CFLAGS += -DUSE_VORBISFILE
ALT_MUSIC_LIBS += $(LIBVORBISFILE_LIBS)
endif

ifeq ($(ALT_MUSIC_USE_IVORBISFILE)$(ALT_MUSIC_USE_SNDFILE), truefalse)
ALT_MUSIC_SOURCES += $(ALT_MUSIC_PATH)/decoders/ivorbisfile
ALT_MUSIC_CFLAGS += -DUSE_IVORBISFILE
ALT_MUSIC_LIBS += $(LIBIVORBISFILE_LIBS)
endif

ifeq ($(ALT_MUSIC_USE_FLAC)$(ALT_MUSIC_USE_SNDFILE), truefalse)
ALT_MUSIC_SOURCES += $(ALT_MUSIC_PATH)/decoders/flac
ALT_MUSIC_CFLAGS += -DUSE_FLAC
ALT_MUSIC_LIBS += $(LIBFLAC_LIBS)
endif

ifeq ($(ALT_MUSIC_USE_SNDFILE), true)
ALT_MUSIC_SOURCES += $(ALT_MUSIC_PATH)/decoders/sndfile
ALT_MUSIC_CFLAGS += -DUSE_SNDFILE
ALT_MUSIC_LIBS += $(LIBSNDFILE_LIBS)
endif

ifeq ($(ALT_MUSIC_USE_OPENMPT), true)
ALT_MUSIC_SOURCES += $(ALT_MUSIC_PATH)/decoders/openmpt
ALT_MUSIC_CFLAGS += -DUSE_OPENMPT
ALT_MUSIC_LIBS += $(LIBOPENMPT_LIBS)
endif

ifeq ($(ALT_MUSIC_USE_SPC), true)
ALT_MUSIC_SOURCES += $(ALT_MUSIC_PATH)/decoders/spc
ALT_MUSIC_CFLAGS += -DUSE_SPC
ALT_MUSIC_LIBS += $(SPC_LIBS)

ALT_MUSIC_SPC_SOURCES = \
	dsp \
	SNES_SPC \
	SNES_SPC_misc \
	SNES_SPC_state \
	spc \
	SPC_DSP \
	SPC_Filter
ALT_MUSIC_OBJECTS += $(addprefix obj/$(ALT_MUSIC_PATH)/decoders/snes_spc-0.9.0/snes_spc/, $(addsuffix .o, $(ALT_MUSIC_SPC_SOURCES)))

obj/$(ALT_MUSIC_PATH)/decoders/snes_spc-0.9.0/snes_spc/%.o: src/$(ALT_MUSIC_PATH)/decoders/snes_spc-0.9.0/snes_spc/%.cpp
	@mkdir -p $(@D)
	@$(CXX) $(ALL_CXXFLAGS) -Wno-implicit-fallthrough -Wno-array-bounds -o $@ $^ -c
endif

ifeq ($(ALT_MUSIC_USE_PXTONE), true)
ALT_MUSIC_SOURCES += $(ALT_MUSIC_PATH)/decoders/pxtone
ALT_MUSIC_CFLAGS += -DUSE_PXTONE
ALT_MUSIC_LIBS += $(PXTONE_LIBS)

ALT_MUSIC_PXTONE_SOURCES = \
	pxtnDelay \
	pxtnDescriptor \
	pxtnError \
	pxtnEvelist \
	pxtnMaster \
	pxtnMem \
	pxtnOverDrive \
	pxtnPulse_Frequency \
	pxtnPulse_Noise \
	pxtnPulse_NoiseBuilder \
	pxtnPulse_Oggv \
	pxtnPulse_Oscillator \
	pxtnPulse_PCM \
	pxtnService \
	pxtnService_moo \
	pxtnText \
	pxtnUnit \
	pxtnWoice \
	pxtnWoice_io \
	pxtnWoicePTV \
	pxtoneNoise \
	shim
ALT_MUSIC_OBJECTS += $(addprefix obj/$(ALT_MUSIC_PATH)/decoders/pxtone/, $(addsuffix .o, $(ALT_MUSIC_PXTONE_SOURCES)))

obj/$(ALT_MUSIC_PATH)/decoders/pxtone/%.o: src/$(ALT_MUSIC_PATH)/decoders/pxtone/%.cpp
	@mkdir -p $(@D)
	@$(CXX) $(ALL_CXXFLAGS) -std=gnu++17 -Wno-switch -Wno-tautological-compare -Wno-sign-compare -Wno-unused-parameter -Wno-unused-value -Wno-unused-variable -Wno-missing-field-initializers -Wno-misleading-indentation -fno-strict-aliasing -o $@ $^ -c
endif

ifeq ($(ALT_MUSIC_BACKEND), mini_al)
ALT_MUSIC_SOURCES += $(ALT_MUSIC_PATH)/playback/mini_al
else ifeq ($(ALT_MUSIC_BACKEND), SDL2)
ALT_MUSIC_SOURCES += $(ALT_MUSIC_PATH)/playback/SDL2
ALT_MUSIC_CFLAGS += $(SDL2_CFLAGS)
ALT_MUSIC_LIBS += $(SDL2_LIBS)
else ifeq ($(ALT_MUSIC_BACKEND), Cubeb)
ALT_MUSIC_SOURCES += $(ALT_MUSIC_PATH)/playback/cubeb
ALT_MUSIC_LIBS += -lcubeb -lole32 -lavrt -lwinmm -luuid -lstdc++
endif

ALT_MUSIC_OBJECTS += $(addprefix obj/, $(addsuffix .o, $(ALT_MUSIC_SOURCES)))

obj/$(ALT_MUSIC_PATH)/%.o: src/$(ALT_MUSIC_PATH)/%.c
	@mkdir -p $(@D)
	@$(CC) $(ALT_MUSIC_CFLAGS) $(ALL_CFLAGS) -o $@ $^ -c

bin/mods/alternate_music/alternate_music.dll: $(ALT_MUSIC_OBJECTS)
	@mkdir -p $(@D)
	@$(CC) $(ALT_MUSIC_CFLAGS) $(ALL_CFLAGS) -o $@ $^ -shared $(LDFLAGS) $(ALT_MUSIC_LIBS) $(LIBS)
	@strip $@ --strip-unneeded

# ====================
# SDL controller input
# ====================

SDL_CONTROLLER_INPUT_PATH = $(MODS_PATH)/sdl_controller_input
SDL_CONTROLLER_INPUT_SOURCES = \
	$(COMMON_PATH)/mod_loader \
	$(COMMON_PATH)/sprintfMalloc \
	$(SDL_CONTROLLER_INPUT_PATH)/main
SDL_CONTROLLER_INPUT_OBJECTS = $(addprefix obj/, $(addsuffix .o, $(SDL_CONTROLLER_INPUT_SOURCES)))

bin/mods/sdl_controller_input/sdl_controller_input.dll: $(SDL_CONTROLLER_INPUT_OBJECTS)
	@mkdir -p $(@D)
	@$(CC) $(SDL2_CFLAGS) $(ALL_CFLAGS) -o $@ $^ -shared $(LDFLAGS) $(SDL2_LIBS) $(LIBS)
	@strip $@ --strip-unneeded

# ====================
# WASD input
# ====================

WASD_INPUT_PATH = $(MODS_PATH)/wasd_input
WASD_INPUT_SOURCES = \
	$(COMMON_PATH)/mod_loader \
	$(WASD_INPUT_PATH)/main
WASD_INPUT_OBJECTS = $(addprefix obj/, $(addsuffix .o, $(WASD_INPUT_SOURCES)))

bin/mods/wasd_input/wasd_input.dll: $(WASD_INPUT_OBJECTS)
	@mkdir -p $(@D)
	@$(CC) $(ALL_CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS) -shared
	@strip $@ --strip-unneeded

# ====================
# Ikachan cursor
# ====================

IKACHAN_CURSOR_PATH = $(MODS_PATH)/ikachan_cursor
IKACHAN_CURSOR_SOURCES = \
	$(COMMON_PATH)/mod_loader \
	$(IKACHAN_CURSOR_PATH)/main
IKACHAN_CURSOR_OBJECTS = $(addprefix obj/, $(addsuffix .o, $(IKACHAN_CURSOR_SOURCES)))

bin/mods/ikachan_cursor/ikachan_cursor.dll: $(IKACHAN_CURSOR_OBJECTS)
	@mkdir -p $(@D)
	@$(CC) $(ALL_CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS) -shared
	@strip $@ --strip-unneeded

# ====================
# Debug save
# ====================

DEBUG_SAVE_PATH = $(MODS_PATH)/debug_save
DEBUG_SAVE_SOURCES = \
	$(COMMON_PATH)/mod_loader \
	$(DEBUG_SAVE_PATH)/main
DEBUG_SAVE_OBJECTS = $(addprefix obj/, $(addsuffix .o, $(DEBUG_SAVE_SOURCES)))

bin/mods/debug_save/debug_save.dll: $(DEBUG_SAVE_OBJECTS)
	@mkdir -p $(@D)
	@$(CC) $(ALL_CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS) -shared
	@strip $@ --strip-unneeded

# ====================
# 3DS HUD
# ====================

3DS_HUD_PATH = $(MODS_PATH)/3ds_hud
3DS_HUD_SOURCES = \
	$(COMMON_PATH)/mod_loader \
	$(3DS_HUD_PATH)/main
3DS_HUD_OBJECTS = $(addprefix obj/, $(addsuffix .o, $(3DS_HUD_SOURCES)))

bin/mods/3ds_hud/3ds_hud.dll: $(3DS_HUD_OBJECTS)
	@mkdir -p $(@D)
	@$(CC) $(ALL_CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS) -shared
	@strip $@ --strip-unneeded

# ====================
# Disable image protection
# ====================

DISABLE_IMAGE_PROTECTION_PATH = $(MODS_PATH)/disable_image_protection
DISABLE_IMAGE_PROTECTION_SOURCES = \
	$(COMMON_PATH)/mod_loader \
	$(DISABLE_IMAGE_PROTECTION_PATH)/main
DISABLE_IMAGE_PROTECTION_OBJECTS = $(addprefix obj/, $(addsuffix .o, $(DISABLE_IMAGE_PROTECTION_SOURCES)))

bin/mods/disable_image_protection/disable_image_protection.dll: $(DISABLE_IMAGE_PROTECTION_OBJECTS)
	@mkdir -p $(@D)
	@$(CC) $(ALL_CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS) -shared
	@strip $@ --strip-unneeded

# ====================
# TSC MBX
# ====================

TSC_MBX_PATH = $(MODS_PATH)/tsc_mbx
TSC_MBX_SOURCES = \
	$(COMMON_PATH)/mod_loader \
	$(TSC_MBX_PATH)/main
TSC_MBX_OBJECTS = $(addprefix obj/, $(addsuffix .o, $(TSC_MBX_SOURCES)))

bin/mods/tsc_mbx/tsc_mbx.dll: $(TSC_MBX_OBJECTS)
	@mkdir -p $(@D)
	@$(CC) $(ALL_CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS) -shared
	@strip $@ --strip-unneeded

# ====================
# TSC no-NOD
# ====================

TSC_NONOD_PATH = $(MODS_PATH)/tsc_mbx
TSC_NONOD_SOURCES = \
	$(COMMON_PATH)/mod_loader \
	$(TSC_NONOD_PATH)/main
TSC_NONOD_OBJECTS = $(addprefix obj/, $(addsuffix .o, $(TSC_NONOD_SOURCES)))

bin/mods/tsc_nonod/tsc_nonod.dll: $(TSC_NONOD_OBJECTS)
	@mkdir -p $(@D)
	@$(CC) $(ALL_CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS) -shared
	@strip $@ --strip-unneeded

# ====================
# Title background
# ====================

TITLE_BACKGROUND_PATH = $(MODS_PATH)/title_background
TITLE_BACKGROUND_SOURCES = \
	$(COMMON_PATH)/mod_loader \
	$(TITLE_BACKGROUND_PATH)/main
TITLE_BACKGROUND_OBJECTS = $(addprefix obj/, $(addsuffix .o, $(TITLE_BACKGROUND_SOURCES)))

bin/mods/title_background/title_background.dll: $(TITLE_BACKGROUND_OBJECTS)
	@mkdir -p $(@D)
	@$(CC) $(ALL_CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS) -shared
	@strip $@ --strip-unneeded

# ====================
# Ghost mode
# ====================

GHOST_MODE_PATH = $(MODS_PATH)/ghost_mode
GHOST_MODE_SOURCES = \
	$(COMMON_PATH)/mod_loader \
	$(GHOST_MODE_PATH)/main \
	$(GHOST_MODE_PATH)/in \
	$(GHOST_MODE_PATH)/out
GHOST_MODE_OBJECTS = $(addprefix obj/, $(addsuffix .o, $(GHOST_MODE_SOURCES)))

bin/mods/ghost_mode/ghost_mode.dll: $(GHOST_MODE_OBJECTS)
	@mkdir -p $(@D)
	@$(CC) $(ALL_CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS) -shared
	@strip $@ --strip-unneeded
