This mod enables playback of Ogg Vorbis music files.

This mod's settings.txt file supports the following options:
* playlist - The name of a folder containing a 'playlist.txt' and the Ogg music files.

Each line of the playlist file specifies one song. Each line is broken up into one or more properties, seperated with ',' characters. The first property must be the path and filename of the music file, relative to the Cave Story EXE file. Properties after that are flags, which can be any of the following:

'loop' - Makes the song loop once it's finished playing.
'split' - Enables a more dynamic form of looping by spliting the song into two files: [name]_intro.ogg, and [name]_loop.ogg. The intro file plays first, and then the loop files plays afterwards. When the loop file ends, the song loops back to the start of the loop file - the intro file never plays again.
'org' - This causes the line it's on to be ignored, and instead fall-through to the built-in Org file.
