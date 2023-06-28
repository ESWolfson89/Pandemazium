// Copyright Eric Wolfson 2016-2017
// See LICENSE.txt (GPLv3)

#ifndef SOUND_H_
#define SOUND_H_

#include "globals.h"

#define NUM_TOTAL_SOUNDS 29

enum sound_type
{
    SOUNDTYPE_COIN,
    SOUNDTYPE_PISTOL,
    SOUNDTYPE_REVOLVER,
    SOUNDTYPE_SHOTGUN,
    SOUNDTYPE_CHAINGUN,
    SOUNDTYPE_FLAMETHROWER,
    SOUNDTYPE_CANNON,
    SOUNDTYPE_ROCKETLAUNCHER,
    SOUNDTYPE_LASERSHOT,
    SOUNDTYPE_EXPLOSION,
    SOUNDTYPE_COLLECT,
    SOUNDTYPE_SHADOWDIE,
    SOUNDTYPE_ADVANCEDAGENTDIE,
    SOUNDTYPE_HAZMATGODDIE,
    SOUNDTYPE_GLADIATORDIE,
    SOUNDTYPE_AGENTDIE,
    SOUNDTYPE_EXECUTIONERDIE,
    SOUNDTYPE_ELITEGUARDDIE,
    SOUNDTYPE_WARRIORDIE,
    SOUNDTYPE_HAZMATDIE,
    SOUNDTYPE_GUARDDIE,
    SOUNDTYPE_FIGHTERDIE,
    SOUNDTYPE_OPENDOOR1,
    SOUNDTYPE_OPENDOOR2,
    SOUNDTYPE_LEVELUP,
    SOUNDTYPE_HEALTH,
    SOUNDTYPE_UNLOCKDOOR,
    SOUNDTYPE_NEXTLEVEL,
    SOUNDTYPE_PLAYERDIE
};

static const std::string sound_file_names[NUM_TOTAL_SOUNDS] =
{
    "coin2.wav",
    "pistol1.wav",
    "revolver1.wav",
    "shotgun1.wav",
    "chaingun1.wav",
    "flamethrower1.wav",
    "cannon1.wav",
    "rocketlaunch.wav",
    "laser1.wav",
    "explosion1.wav",
    "collect1.wav",
    "shadowdie.wav",
    "warlorddie.wav",
    "grandchampiondie.wav",
    "championdie.wav",
    "agentdie.wav",
    "executionerdie.wav",
    "eliteguarddie.wav",
    "gladiatordie.wav",
    "hazmatdie2.wav",
    "guarddie.wav",
    "humandie.wav",
    "opendoor1.wav",
    "opendoor2.wav",
    "explevelup.wav",
    "health1.wav",
    "unlockdoor.wav",
    "nextlevel.wav",
    "playerdie.wav"
};

class snd_engine
{
public:
    snd_engine();
    ~snd_engine();
    bool initMixer();
    void freeMixer();
    void playSoundEffect(Mix_Chunk *);
    Mix_Chunk* getSoundEffect(int);

private:
    Mix_Chunk* sound_effects[NUM_TOTAL_SOUNDS];
};

#endif
