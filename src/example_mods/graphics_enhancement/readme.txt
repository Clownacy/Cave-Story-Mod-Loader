This mod extends the game's graphical capabilities. It allows you to change the game's aspect ratio (tested at 16:9 and 21:9), adding widescreen support in the process. It also allows you to upscale the game's window beyond the original limit of 640x480. In addition, the mod allows control over the game's sprite resolution, allowing HD sprite sheets.

Also added to the game is a borderless fullscreen mode, that replaces the normal fullscreen mode, which is broken by this mod (non-standard resolutions).

This mod's settings.txt file supports the following options:
'aspect_ratio_x' - The X value of the game's aspect ratio.
'aspect_ratio_y' - The Y value of the game's aspect ratio.
'window_upscale' - What factor to increase the size of the game's window by. 1 is 240p, 2 is 480p, etc.
'sprite_resolution' - What factor to increase the game's sprite resolution by.
'black_bars' - Enables/disables drawing black bars to cover out-of-bound parts of the screen.
'disable_ironhead_black_bars' - Disables the black bars in room 0x1F (the Ironhead arean in the vanilla game). May be useful for mods that use the room for something else.
'borderless_fullscreen' - Replaces Cave Story's fullscreen mode with a borderless fullscreen implementation.
'borderless_fullscreen_auto_aspect_ratio' - When in borderless fullscreen, the game auto-detects your monitor's resolution, and chooses the most appropriate aspect ratio (overrides the above 'aspect_ratio_x' and 'aspect_ratio_y' settings).
'borderless_fullscreen_auto_window_upscale' - Like the previous option, this one automatically chooses the highest-resolution window for your monitor, improving font smoothness (overrides 'window_upscale').