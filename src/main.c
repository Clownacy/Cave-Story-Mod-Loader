// OGG mod for Freeware Cave Story
// Copyright © 2017 Clownacy

#include <windows.h>

#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"

int* const pause_flag_ptr = (int* const)0x49E468;
int* const current_music_ptr = (int* const)0x4A57F4;
int* const previous_music_ptr = (int* const)0x4A57FC;

enum song_loop_flag
{
	SONG_PLAY_ONCE,
	SONG_LOOP
};

const struct
{
	const char* const song_name;
	const enum song_loop_flag loop_flag;
} MusicList[] = {
	{"XXXX", SONG_PLAY_ONCE},
	{"WANPAKU", SONG_LOOP},
	{"ANZEN", SONG_LOOP},
	{"GAMEOVER", SONG_PLAY_ONCE},
	{"GRAVITY", SONG_LOOP},
	{"WEED", SONG_LOOP},
	{"MDOWN2", SONG_LOOP},
	{"FIREEYE", SONG_LOOP},
	{"VIVI", SONG_LOOP},
	{"MURA", SONG_LOOP},
	{"FANFALE1", SONG_PLAY_ONCE},
	{"GINSUKE", SONG_LOOP},
	{"CEMETERY", SONG_LOOP},
	{"PLANT", SONG_LOOP},
	{"KODOU", SONG_LOOP},
	{"FANFALE2", SONG_PLAY_ONCE},
	{"FANFALE3", SONG_PLAY_ONCE},
	{"DR", SONG_LOOP},
	{"ESCAPE", SONG_LOOP},
	{"JENKA", SONG_LOOP},
	{"MAZE", SONG_LOOP},
	{"ACCESS", SONG_LOOP},
	{"IRONH", SONG_LOOP},
	{"GRAND", SONG_LOOP},
	{"Curly", SONG_LOOP},
	{"OSIDE", SONG_LOOP},
	{"REQUIEM", SONG_LOOP},
	{"WANPAK2", SONG_LOOP},
	{"QUIET", SONG_LOOP},
	{"LASTCAVE", SONG_LOOP},
	{"BALCONY", SONG_LOOP},
	{"LASTBTL", SONG_LOOP},
	{"LASTBT3", SONG_LOOP},
	{"ENDING", SONG_LOOP},
	{"ZONBIE", SONG_LOOP},
	{"BDOWN", SONG_LOOP},
	{"HELL", SONG_LOOP},
	{"JENKA2", SONG_LOOP},
	{"MARINE", SONG_LOOP},
	{"BALLOS", SONG_LOOP},
	{"TOROKO", SONG_PLAY_ONCE},
	{"WHITE", SONG_LOOP}
};

void PatchCall(const int instruction_address, const void* const new_destination)
{
	const int relative_address = (int)new_destination - (instruction_address+5);
	WriteProcessMemory(GetCurrentProcess(), (void*)instruction_address+1, &relative_address, sizeof(void*), NULL);
}

void WriteJump(const int instruction_address, const void* const new_destination)
{
	const char jump_opcode = 0xE9;
	WriteProcessMemory(GetCurrentProcess(), (void*)instruction_address, &jump_opcode, sizeof(char), NULL);
	const int relative_address = (int)new_destination - (instruction_address+5);
	WriteProcessMemory(GetCurrentProcess(), (void*)instruction_address+1, &relative_address, sizeof(void*), NULL);
}

void PlayOggMusic(const int song_id)
{
	static Mix_Music *music;

	const char* const song_name = MusicList[song_id].song_name;
	char song_file_path[strlen(song_name)+9+4+1];
	strcpy(song_file_path, "data/Ogg/");
	strcat(song_file_path, song_name);
	strcat(song_file_path, ".ogg");

	if (music != NULL)
		Mix_FreeMusic(music);

	music = Mix_LoadMUS(song_file_path);
	Mix_PlayMusic(music, (MusicList[song_id].loop_flag == SONG_PLAY_ONCE ? 0 : -1));
}

void __cdecl PlayMusic_new(const int music_id)
{
	if (!music_id || music_id != *current_music_ptr)
	{
		*previous_music_ptr = *current_music_ptr;
		PlayOggMusic(music_id);
		*current_music_ptr = music_id;
	}
}

void __cdecl PlayPreviousMusic_new(void)
{
	PlayOggMusic(*previous_music_ptr);
	*current_music_ptr = *previous_music_ptr;
}

void __cdecl WindowFocusGained_new(void)
{
	if (!*pause_flag_ptr)
	{
		*pause_flag_ptr = 1;
		Mix_ResumeMusic();
	}
}

void __cdecl WindowFocusLost_new(void)
{
	if (*pause_flag_ptr)
	{
		*pause_flag_ptr = 0;
		Mix_PauseMusic();
	}
}

__declspec(dllexport) void init(void)
{
	// Do all init work here
	// Setup music system
	SDL_Init(SDL_INIT_AUDIO);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	Mix_Init(MIX_INIT_OGG);
	// Replace PlayMusic and PlayPreviousMusic with our custom OGG ones
	WriteJump(0x420EE0, &PlayMusic_new);
	WriteJump(0x420F50, &PlayPreviousMusic_new);
	// We also need to replace the music pausing/resuming when the window focus changes
	PatchCall(0x41330F, &WindowFocusGained_new);
	PatchCall(0x413316, &WindowFocusLost_new);
	// 60 FPS
	//const char val = 17;
	//WriteProcessMemory(GetCurrentProcess(), (void*)0x40B36F, &val, sizeof(char), NULL);
	//WriteProcessMemory(GetCurrentProcess(), (void*)0x40B3A9, &val, sizeof(char), NULL);
}
