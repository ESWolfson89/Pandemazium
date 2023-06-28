Pandemazium.

Copyright Eric Wolfson 2016-2023

A fast-paced 2D platformer with roguelike elements written in C++11.

Watch the game at:
https://www.youtube.com/@pandemazium1632/videos
or search "Pandemazium" on youtube

------------------------------------------

![Alt text](/screenshots/screenshot1.jpg?raw=true "Screenshot1")

![Alt text](/screenshots/screenshot3.jpg?raw=true "Screenshot3")

![Alt text](/screenshots/screenshot5.jpg?raw=true "Screenshot5")

Requires:
SDL2
SDL2 Mixer library
SDL2 Image library

Enemy color modifiers:
Green: fast
Orange: extra health
Purple: Fast and extra health

Weapon color modifiers:
Green: higher rate of fire
Red: extra damage
Purple: Higher rate of fire and extra damage

How to play:

z: jump
v: use door
a: drop or pickup weapon
x: shoot
Arrow keys: move left/right, or climb up/down ladder 

As it stands there are still a few debug print statements written to standard output stream.

Credit for sound generation goes to bfxr (bfxr.net). This allowed me to create the wav files used in this game.

------------
In the source code, I refer to "mob" as the NPC or player character. A mob object can refer to either one.

Compilation:
------------

<IMPORTANT:> textures folder and sound folder must be in executable's parent directory.

More compilation instructions to come.

------------

To be posted:

1) Dynamic linked libraries SDL2,SDL2Mixer,SDL2Image that are needed

2) How to compile in Code::Blocks under windows( I have not tested under linux, but will try to do so in the future so I can have compilation instructions for that too).

3) Remaining things I intend to add (highscores, boss, etc...)

4) There are still some bugs (levels not always connected, have unreachable parts, weapons can go flying off the map when thrown, and some enemies can sometimes go through walls).

Licensed under GNU (General Public License) GPL version 3.
See LICENSE.txt for more details.
