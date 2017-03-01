#include <stdbool.h>
#include <string.h>

#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"

#include "mod_loader_main.h"

#define SONG_LOOP (1<<0)
#define SONG_SPLIT (1<<1)

// Variables
int* const current_music = (int* const)0x4A57F4;
int* const previous_song_last_position = (int* const)0x4A57F8;
int* const previous_music = (int* const)0x4A57FC;

// String array
const char* const * const OrgMusicList = (const char* const * const)0x4981E8;

// Functions
void (*LoadOrgMusic)(const char* const) = (void(*)(const char* const))0x41C6F0;
void (*SetOrgMusicPosition)(int) = (void(*)(int))0x41C730;
int (*GetOrgMusicPosition)(void) = (int(*)(void))0x41C770;
void (*sub_41C790)(void) = (void(*)(void))0x41C790;
void (*SetOrgVolume)(int) = (void(*)(int))0x41C7C0;
void (*sub_41C7F0)(void) = (void(*)(void))0x41C7F0;

bool intro_playing;
int current_loop_setting;
Mix_Music *music_intro, *music_loop;

const struct
{
	const char* const song_name;
	const char song_flags;
} MusicList[] = {
#ifdef SOUNDTRACK_3D
	{"data/Ogg11/WANPAKU", SONG_LOOP | SONG_SPLIT},
	{"data/Ogg11/ANZEN", SONG_LOOP | SONG_SPLIT},
	{"data/Ogg11/GAMEOVER", SONG_SPLIT},
	{"data/Ogg11/GRAVITY", SONG_LOOP | SONG_SPLIT},
	{"data/Ogg11/WEED", SONG_LOOP | SONG_SPLIT},
	{"data/Ogg11/MDOWN2", SONG_LOOP | SONG_SPLIT},
	{"data/Ogg11/FIREEYE", SONG_LOOP | SONG_SPLIT},
	{"data/Ogg11/VIVI", SONG_LOOP | SONG_SPLIT},
	{"data/Ogg11/MURA", SONG_LOOP | SONG_SPLIT},
	{"data/Ogg11/FANFALE1", SONG_SPLIT},
	{"data/Ogg11/GINSUKE", SONG_LOOP | SONG_SPLIT},
	{"data/Ogg11/CEMETERY", SONG_LOOP | SONG_SPLIT},
	{"data/Ogg11/PLANT", SONG_LOOP | SONG_SPLIT},
	{"data/Ogg11/KODOU", SONG_LOOP | SONG_SPLIT},
	{"data/Ogg11/FANFALE3", SONG_SPLIT},
	{"data/Ogg11/FANFALE2", SONG_SPLIT},
	{"data/Ogg11/DR", SONG_LOOP | SONG_SPLIT},
	{"data/Ogg11/ESCAPE", SONG_LOOP | SONG_SPLIT},
	{"data/Ogg11/JENKA", SONG_LOOP | SONG_SPLIT},
	{"data/Ogg11/MAZE", SONG_LOOP | SONG_SPLIT},
	{"data/Ogg11/ACCESS", SONG_LOOP | SONG_SPLIT},
	{"data/Ogg11/IRONH", SONG_LOOP | SONG_SPLIT},
	{"data/Ogg11/GRAND", SONG_LOOP | SONG_SPLIT},
	{"data/Ogg11/Curly", SONG_LOOP | SONG_SPLIT},
	{"data/Ogg11/OSIDE", SONG_LOOP | SONG_SPLIT},
	{"data/Ogg11/REQUIEM", SONG_LOOP | SONG_SPLIT},
	{"data/Ogg11/WANPAK2", SONG_LOOP | SONG_SPLIT},
	{"data/Ogg11/QUIET", SONG_LOOP | SONG_SPLIT},
	{"data/Ogg11/LASTCAVE", SONG_LOOP | SONG_SPLIT},
	{"data/Ogg11/BALCONY", SONG_LOOP | SONG_SPLIT},
	{"data/Ogg11/LASTBTL", SONG_LOOP},
	{"data/Ogg11/LASTBT3", SONG_LOOP | SONG_SPLIT},
	{"data/Ogg11/ENDING", SONG_LOOP | SONG_SPLIT},
	{"data/Ogg11/ZONBIE", SONG_LOOP | SONG_SPLIT},
	{"data/Ogg11/BDOWN", SONG_LOOP | SONG_SPLIT},
	{"data/Ogg11/HELL", SONG_LOOP | SONG_SPLIT},
	{"data/Ogg11/JENKA2", SONG_LOOP | SONG_SPLIT},
	{"data/Ogg11/MARINE", SONG_LOOP | SONG_SPLIT},
	{"data/Ogg11/BALLOS", SONG_LOOP | SONG_SPLIT},
	{"data/Ogg11/TOROKO", SONG_SPLIT},
	{"data/Ogg11/WHITE", SONG_LOOP | SONG_SPLIT}
#endif
#ifdef SOUNDTRACK_WIIWARE
	{"data/Ogg/WANPAKU", SONG_LOOP},
	{"data/Ogg/ANZEN", SONG_LOOP},
	{"data/Ogg/GAMEOVER", 0},
	{"data/Ogg/GRAVITY", SONG_LOOP},
	{"data/Ogg/WEED", SONG_LOOP},
	{"data/Ogg/MDOWN2", SONG_LOOP},
	{"data/Ogg/FIREEYE", SONG_LOOP},
	{"data/Ogg/VIVI", SONG_LOOP},
	{"data/Ogg/MURA", SONG_LOOP},
	{"data/Ogg/FANFALE1", 0},
	{"data/Ogg/GINSUKE", SONG_LOOP},
	{"data/Ogg/CEMETERY", SONG_LOOP},
	{"data/Ogg/PLANT", SONG_LOOP},
	{"data/Ogg/KODOU", SONG_LOOP},
	{"data/Ogg/FANFALE3", 0},
	{"data/Ogg/FANFALE2", 0},
	{"data/Ogg/DR", SONG_LOOP},
	{"data/Ogg/ESCAPE", SONG_LOOP},
	{"data/Ogg/JENKA", SONG_LOOP},
	{"data/Ogg/MAZE", SONG_LOOP},
	{"data/Ogg/ACCESS", SONG_LOOP},
	{"data/Ogg/IRONH", SONG_LOOP},
	{"data/Ogg/GRAND", SONG_LOOP},
	{"data/Ogg/Curly", SONG_LOOP},
	{"data/Ogg/OSIDE", SONG_LOOP},
	{"data/Ogg/REQUIEM", SONG_LOOP},
	{"data/Ogg/WANPAK2", SONG_LOOP},
	{"data/Ogg/QUIET", SONG_LOOP},
	{"data/Ogg/LASTCAVE", SONG_LOOP},
	{"data/Ogg/BALCONY", SONG_LOOP},
	{"data/Ogg/LASTBTL", SONG_LOOP},
	{"data/Ogg/LASTBT3", SONG_LOOP},
	{"data/Ogg/ENDING", SONG_LOOP},
	{"data/Ogg/ZONBIE", SONG_LOOP},
	{"data/Ogg/BDOWN", SONG_LOOP},
	{"data/Ogg/HELL", SONG_LOOP},
	{"data/Ogg/JENKA2", SONG_LOOP},
	{"data/Ogg/MARINE", SONG_LOOP},
	{"data/Ogg/BALLOS", SONG_LOOP},
	{"data/Ogg/TOROKO", 0},
	{"data/Ogg/WHITE", SONG_LOOP}
#endif
};

void UnloadMusic(Mix_Music **music)
{
	Mix_HaltMusic();
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
		// Play split-Ogg music (Cave Story 3D)
		// Get filenames
		const char* const song_name = MusicList[song_id - 1].song_name;
		char song_base_file_path[strlen(song_name)+1];
		strcpy(song_base_file_path, song_name);
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
		current_loop_setting = (MusicList[song_id - 1].song_flags & SONG_LOOP) ? -1 : 0;

		// Play intro
		Mix_PlayMusic(music_intro, 0);
	}
	else
	{
		// Play single-Ogg music (Cave Story WiiWare)
		const char* const song_name = MusicList[song_id - 1].song_name;
		char song_file_path[strlen(song_name)+4+1];
		strcpy(song_file_path, song_name);
		strcat(song_file_path, ".ogg");

		music_loop = Mix_LoadMUS(song_file_path);

		if (music_loop == NULL)
			return false;

		intro_playing = false;

		Mix_PlayMusic(music_loop, (MusicList[song_id - 1].song_flags & SONG_LOOP) ? -1 : 0);
	}

	return true;
}

void PlayOrgMusic(const int music_id)
{
	*previous_song_last_position = GetOrgMusicPosition();
	sub_41C7F0();
	LoadOrgMusic(OrgMusicList[music_id]);
	SetOrgVolume(100);
	SetOrgMusicPosition(0);
	sub_41C790();
}

void PlayPreviousOrgMusic(void)
{
	sub_41C7F0();
	LoadOrgMusic(OrgMusicList[*previous_song_last_position]);
	SetOrgMusicPosition(*previous_song_last_position);
	SetOrgVolume(100);
	sub_41C790();
}

void __cdecl PlayMusic_new(const int music_id)
{
	if (music_id == 0 || music_id != *current_music)
	{
		*previous_music = *current_music;
		if (PlayOggMusic(music_id))
		{
			// Ogg music played successfully,
			// silence any org music that might be playing
			PlayOrgMusic(0);
		}
		else
		{
			// Ogg music failed to play,
			// play Org instead
			PlayOrgMusic(music_id);
		}
		*current_music = music_id;
	}
}

void __cdecl PlayPreviousMusic_new(void)
{
	if (PlayOggMusic(*previous_music))
	{
		// Ogg music played successfully,
		// silence any org music that might be playing
		PlayOrgMusic(0);
	}
	else
	{
		// Ogg music failed to play,
		// play Org instead
		PlayPreviousOrgMusic();
	}
	*current_music = *previous_music;
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

void __cdecl FadeMusic_new(void)
{
	int* const music_fade_flag = (void*)0x4A4E10;

	*music_fade_flag = 1;
	intro_playing = false;	// A bit of a hack, but we can't have a new song kick in just because we faded out
	Mix_FadeOutMusic(1000 * 5);
}

void InitMod(void)
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
	WriteRelativeAddress(0x412BD6 + 1, WindowFocusLost_new);
	WriteRelativeAddress(0x412C06 + 1, WindowFocusGained_new);
	// Patch fading
	WriteRelativeAddress(0x40D84F + 1, FadeMusic_new);
	WriteRelativeAddress(0x42438A + 1, FadeMusic_new);
}
