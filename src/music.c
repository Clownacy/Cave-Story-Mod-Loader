#include <stdbool.h>
#include <string.h>

#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"

#include "main.h"
#include "music.h"

#define SONG_PLAY_ONCE 0
#define SONG_LOOP 1
#define SONG_NOT_SPLIT 0
#define SONG_SPLIT 2

void (*sub_41C7F0)(void) = (void(*)(void))0x41C7F0;
void __cdecl (*PlayMusic)(const int file_id) = (void __cdecl(*)(const int))0x420EE0;
void __cdecl (*PlayPreviousMusic)(void) = (void __cdecl(*)(void))0x420F50;
int* const current_music_ptr = (int* const)0x4A57F4;
int* const previous_music_ptr = (int* const)0x4A57FC;

bool intro_playing;
int current_loop_setting;
Mix_Music *music_intro, *music_loop;

const struct
{
	const char* const song_name;
	const char song_flags;
} MusicList[] = {
	{"WANPAKU", SONG_LOOP | SONG_SPLIT},
	{"ANZEN", SONG_LOOP | SONG_SPLIT},
	{"GAMEOVER", SONG_PLAY_ONCE | SONG_SPLIT},
	{"GRAVITY", SONG_LOOP | SONG_SPLIT},
	{"WEED", SONG_LOOP | SONG_SPLIT},
	{"MDOWN2", SONG_LOOP | SONG_SPLIT},
	{"FIREEYE", SONG_LOOP | SONG_SPLIT},
	{"VIVI", SONG_LOOP | SONG_SPLIT},
	{"MURA", SONG_LOOP | SONG_SPLIT},
	{"FANFALE1", SONG_PLAY_ONCE | SONG_SPLIT},
	{"GINSUKE", SONG_LOOP | SONG_SPLIT},
	{"CEMETERY", SONG_LOOP | SONG_SPLIT},
	{"PLANT", SONG_LOOP | SONG_SPLIT},
	{"KODOU", SONG_LOOP | SONG_SPLIT},
	{"FANFALE2", SONG_PLAY_ONCE | SONG_SPLIT},
	{"FANFALE3", SONG_PLAY_ONCE | SONG_SPLIT},
	{"DR", SONG_LOOP | SONG_SPLIT},
	{"ESCAPE", SONG_LOOP | SONG_SPLIT},
	{"JENKA", SONG_LOOP | SONG_SPLIT},
	{"MAZE", SONG_LOOP | SONG_SPLIT},
	{"ACCESS", SONG_LOOP | SONG_SPLIT},
	{"IRONH", SONG_LOOP | SONG_SPLIT},
	{"GRAND", SONG_LOOP | SONG_SPLIT},
	{"Curly", SONG_LOOP | SONG_SPLIT},
	{"OSIDE", SONG_LOOP | SONG_SPLIT},
	{"REQUIEM", SONG_LOOP | SONG_SPLIT},
	{"WANPAK2", SONG_LOOP | SONG_SPLIT},
	{"QUIET", SONG_LOOP | SONG_SPLIT},
	{"LASTCAVE", SONG_LOOP | SONG_SPLIT},
	{"BALCONY", SONG_LOOP | SONG_SPLIT},
	{"LASTBTL", SONG_LOOP | SONG_SPLIT},
	{"LASTBT3", SONG_LOOP | SONG_SPLIT},
	{"ENDING", SONG_LOOP | SONG_SPLIT},
	{"ZONBIE", SONG_LOOP | SONG_SPLIT},
	{"BDOWN", SONG_LOOP | SONG_SPLIT},
	{"HELL", SONG_LOOP | SONG_SPLIT},
	{"JENKA2", SONG_LOOP | SONG_SPLIT},
	{"MARINE", SONG_LOOP | SONG_SPLIT},
	{"BALLOS", SONG_LOOP | SONG_SPLIT},
	{"TOROKO", SONG_PLAY_ONCE | SONG_SPLIT},
	{"WHITE", SONG_LOOP | SONG_SPLIT}
};

void UnloadMusic(Mix_Music **music)
{
	Mix_FreeMusic(*music);
	*music = NULL;
}

void OggMusicEnded(void)
{
	if (intro_playing == true)
	{
		intro_playing = false;
		UnloadMusic(&music_intro);
		Mix_PlayMusic(music_loop, current_loop_setting);
	}
	else
	{
		UnloadMusic(&music_loop);
	}
}

bool PlayOggMusic(const int song_id)
{
	// Kill current music
	UnloadMusic(&music_intro);
	UnloadMusic(&music_loop);

	if (song_id == 0)
	{
		// Song is XXXX, which doesn't have a file in either Ogg soundtrack
		return true;
	}

	if (MusicList[song_id - 1].song_flags & SONG_SPLIT)
	{
		// Play split Ogg music (Cave Story 3D)
		// Get filenames
		const char* const song_name = MusicList[song_id - 1].song_name;
		char song_base_file_path[strlen(song_name)+11+1];
		strcpy(song_base_file_path, "data/Ogg11/");
		strcat(song_base_file_path, song_name);
		char song_intro_file_path[strlen(song_base_file_path)+6+4+1];
		char song_loop_file_path[strlen(song_base_file_path)+5+4+1];
		strcpy(song_intro_file_path, song_base_file_path);
		strcpy(song_loop_file_path, song_base_file_path);
		strcat(song_intro_file_path, "_intro.ogg");
		strcat(song_loop_file_path, "_loop.ogg");

		// Load intro and loop
		music_intro = Mix_LoadMUS(song_intro_file_path);
		music_loop = Mix_LoadMUS(song_loop_file_path);

		if (music_intro == NULL && music_loop == NULL)
			return false;

		intro_playing = true;
		current_loop_setting = MusicList[song_id - 1].song_flags & SONG_LOOP ? -1 : 0;

		// Play intro
		Mix_PlayMusic(music_intro, 0);
	}
	else
	{
		// Play single Ogg music (Cave Story WiiWare)
		const char* const song_name = MusicList[song_id - 1].song_name;
		char song_file_path[strlen(song_name)+9+4+1];
		strcpy(song_file_path, "data/Ogg/");
		strcat(song_file_path, song_name);
		strcat(song_file_path, ".ogg");

		music_loop = Mix_LoadMUS(song_file_path);

		if (music_loop == NULL)
			return false;

		intro_playing = false;

		Mix_PlayMusic(music_loop, MusicList[song_id - 1].song_flags & SONG_LOOP ? -1 : 0);
	}
	return true;
}

void __cdecl PlayMusic_new(const int music_id)
{
	if (!music_id || music_id != *current_music_ptr)
	{
		*previous_music_ptr = *current_music_ptr;
		if (!PlayOggMusic(music_id))
			PlayMusic(music_id);
		*current_music_ptr = music_id;
	}
}

void __cdecl PlayPreviousMusic_new(void)
{
	if (!PlayOggMusic(*previous_music_ptr))
		PlayPreviousMusic();
	*current_music_ptr = *previous_music_ptr;
}

void __cdecl WindowFocusGained_new(void)
{
	Mix_ResumeMusic();
	sub_41C7F0();	// The instruction we hijacked to get here
}

void __cdecl WindowFocusLost_new(void)
{
	Mix_PauseMusic();
	sub_41C7F0();	// The instruction we hijacked to get here
}

void InitMusic(void)
{
	// Setup music system
	SDL_Init(SDL_INIT_AUDIO);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	Mix_Init(MIX_INIT_OGG);
	Mix_HookMusicFinished(OggMusicEnded);
	// Replace PlayMusic and PlayPreviousMusic with our custom Ogg ones
	WriteRelativeAddress(0x40D833 + 1, PlayMusic_new);
	WriteRelativeAddress(0x40F756 + 1, PlayMusic_new);
	WriteRelativeAddress(0x40FE81 + 1, PlayMusic_new);
	WriteRelativeAddress(0x40FE96 + 1, PlayMusic_new);
	WriteRelativeAddress(0x40FEAB + 1, PlayMusic_new);
	WriteRelativeAddress(0x40FEC0 + 1, PlayMusic_new);
	WriteRelativeAddress(0x40FECC + 1, PlayMusic_new);
	WriteRelativeAddress(0x41038C + 1, PlayMusic_new);
	WriteRelativeAddress(0x41D3FF + 1, PlayMusic_new);
	WriteRelativeAddress(0x424330 + 1, PlayMusic_new);
	WriteRelativeAddress(0x4243DF + 1, PlayPreviousMusic_new);
	// We also need to replace the music pausing/resuming when the window focus changes
	WriteRelativeAddress(0x412C06 + 1, WindowFocusGained_new);
	WriteRelativeAddress(0x412BD6 + 1, WindowFocusLost_new);
}
