This mod enables playback of Ogg Vorbis music files. 

This mod also comes with example playlist data, allowing drag-and-drop compatibility with
Cave Story+'s soundtracks:

The WiiWare/New soundtrack's 'Ogg' folder goes in the 'Cave Story WiiWare' folder,
the Cave Story 3D/Remastered soundtrack's 'Ogg11' folder goes in the 'Cave Story 3D' folder,
and the Famitracks soundtrack's 'ogg17' folder goes in the 'Cave Story Famitrack' folder.

To specify which soundtrack to use, change the 'playlist' option in the 'settings.ini' file.



If you want to make your own soundtrack, here are the formats the mod supports:

The first format is the one used by the WiiWare soundtrack,
where each song gets one Ogg file that repeats in its entirety.

The second is the format used by the Cave Story 3D soundtrack - each song gets two files:
'[song name]_intro.ogg' and '[song name]_loop.ogg'. As you can imagine, when a song starts playing,
it plays the intro file first, then when it ends, it continues onto the loop file.
When the loop file ends, it loops back to itself, meaning the intro never gets played again.
This allows for more complex music.



Each line of 'playlist.txt' specifies one song. Each line is broken up into one or more properties,
separated with ',' characters. The first property must be the path and filename of the music file,
relative to the playlist file. Properties after that are flags, which can be any of the following:

'loop' - Makes the song loop once it's finished playing.

'split' - Makes the song use the Cave Story 3D format described above.

'org' - This causes the song to be ignored, playing the original Org file instead.
