// Copyright Eric Wolfson 2016-2017
// See LICENSE.txt (GPLv3)

#include "sound.h"

snd_engine::snd_engine()
{
    for (int i = 0; i < NUM_TOTAL_SOUNDS; ++i)
    {
        sound_effects[i] = NULL;
    }
}

snd_engine::~snd_engine()
{
    std::cout << "Free mixer\n";
    freeMixer();
}

bool snd_engine::initMixer()
{
    //Initialize SDL_mixer
    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
    {
        return false;
    }

    // initialize all sounds
    for (int i = 0; i < NUM_TOTAL_SOUNDS; ++i)
    {
        sound_effects[i] = Mix_LoadWAV(&("Debug\\sounds\\" + sound_file_names[i])[0]);

        if (sound_effects[i] == NULL)
            return false;
    }

    Mix_Volume(-1,36);

    return true;
}

void snd_engine::freeMixer()
{
    for (int i = 0; i < NUM_TOTAL_SOUNDS; ++i)
    {
        if (sound_effects[i] != NULL)
        {
            Mix_FreeChunk(sound_effects[i]);
            sound_effects[i] = NULL;
        }
    }

    Mix_Quit();
}

void snd_engine::playSoundEffect(Mix_Chunk *seff)
{
    Mix_PlayChannel( -1, seff, 0 );
}

Mix_Chunk *snd_engine::getSoundEffect(int i)
{
    return sound_effects[i];
}
