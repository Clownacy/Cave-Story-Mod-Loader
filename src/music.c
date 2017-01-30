#include <stdbool.h>
#include <string.h>

#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"

#include "main.h"
#include "music.h"

void __cdecl (*PlayMusic)(const int file_id) = (void __cdecl(*)(const int))0x420EE0;
void __cdecl (*PlayPreviousMusic)(void) = (void __cdecl(*)(void))0x420F50;
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

bool PlayOggMusic(const int song_id)
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

	if (music == NULL)
		return false;

	Mix_PlayMusic(music, (MusicList[song_id].loop_flag == SONG_PLAY_ONCE ? 0 : -1));
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

void InitMusic(void)
{
	// Setup music system
	SDL_Init(SDL_INIT_AUDIO);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	Mix_Init(MIX_INIT_OGG);
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
	WriteRelativeAddress(0x41330F + 1, WindowFocusGained_new);
	WriteRelativeAddress(0x413316 + 1, WindowFocusLost_new);
}
