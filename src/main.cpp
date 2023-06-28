// Copyright Eric Wolfson 2016-2017
// See LICENSE.txt (GPLv3)

#include "game.h"

int main(int argc, char* argv[])
{
    // create instance of game obj (contains all program data)
    Game game;
    // execute program
    game.run();
    // When "gfx_engine" instance goes out of scope, its
    // destructor is called freeing all SDL textures, the
    // renderer, window, etc... (implicitly)
    return 0;
}
