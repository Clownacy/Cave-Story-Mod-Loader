This mod enables playback of music files in various formats, including Ogg Vorbis, FLAC,
XM, IT, and MOD.

This mod also comes with example playlist data, allowing drag-and-drop compatibility with
the New, Remastered, Famitracks, Ridiculon, and SNES soundtracks:

To specify which soundtrack to use, change the 'playlist' option in the 'settings.ini' file.



If you want to make your own soundtrack, here are the formats the mod supports:

The first format is the one used by the WiiWare soundtrack,
where each song gets one file that repeats in its entirety.

The second one only applies to Ogg Vorbis and FLAC files, and is the format used by the Cave Story
3D soundtrack - each song gets two files: '[song name]_intro.ogg' and '[song name]_loop.ogg'. As
you can imagine, when a song starts playing, it plays the intro file first, then when it ends, it
continues onto the loop file. When the loop file ends, it loops back to itself, meaning the intro
never gets played again. This allows for more complex music.



Each line of 'playlist.txt' specifies one song. Each line is broken up into one or more properties,
separated with ',' characters. The first property must be the path and filename of the music file,
relative to the playlist file. Properties after that are flags, which can be any of the following:

'loop' - Makes the song loop once it's finished playing.

'org' - This causes the song to be ignored, playing the original Org file instead.
