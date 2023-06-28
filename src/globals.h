// Copyright Eric Wolfson 2016-2017
// See LICENSE.txt (GPLv3)

#ifndef GLOBALS_H_
#define GLOBALS_H_

#include "SDL2/include/SDL.h"
#include "SDL2/include/SDL_image.h"
#include "SDL2/include/SDL_mixer.h"
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <fstream>
#include <cstdlib>

#define uint unsigned int

#define WINDOW_WIDTH 1200.0
#define WINDOW_HEIGHT 720.0

#define NUM_TOTAL_TEXTURES 81
#define NUM_TOTAL_ENTITY_TYPES 59

#define FONT_CHAR_WIDTH 16
#define FONT_CHAR_HEIGHT 16

#define FONT_COLUMNS 16
#define FONT_ROWS 16

#define GRAVITY_VELOCITY_INCREMENT 0.76

#define MOB_ITEM_DROP_VELOCITY_MODIFIER_X 1.5
#define MOB_ITEM_DROP_VELOCITY_MODIFIER_Y 1.2

#define SMALL_BLOCK_DIM 40.0
#define LARGE_WALL_BLOCK_SIZE 80.0
#define GIANT_WALL_BLOCK_SIZE 120.0

#define MAP_WIDTH (current_level_size.x())
#define MAP_HEIGHT (current_level_size.y())

#define BOSS_LEVEL1 8
#define BOSS_LEVEL2 15
#define BOSS_LEVEL3 22
#define MAX_KEY_LEVEL 39

#define NUM_WEAPON_TYPES 8

static const SDL_Color color_darkgray {50,50,50,0};
static const SDL_Color color_white = {255,255,255,0};
static const SDL_Color color_darkorange = {255,128,0,0};
static const SDL_Color color_lightorange = {255,64,0,0};
static const SDL_Color color_green = {0,255,0,0};
static const SDL_Color color_purple = {255,0,255,0};
static const SDL_Color color_red = {255,0,0,0};
static const SDL_Color color_yellow = {255,255,0,0};
static const SDL_Color color_black = {0,0,0,0};
static const SDL_Color color_gray = {128,128,128,0};
static const SDL_Color color_blue = { 0,72,255,0 };
static const SDL_Color color_nearblack = { 1,1,1,0 };

enum mob_type
{
    MOB_PLAYER,
    MOB_FIGHTER,
    MOB_SOLDIER,
    MOB_CAPTAIN,
    MOB_GUARD,
    MOB_BIGGUARD,
    MOB_ELITEGUARD,
    MOB_HAZMATGUY,
    MOB_WARRIOR,
    MOB_SLAYER,
    MOB_BEHEMOTH,
    MOB_GLADIATOR,
    MOB_CHAMPION,
    MOB_GRANDCHAMPION,
    MOB_RETIREDCHAMPION,
    MOB_EXECUTIONER,
    MOB_AGENT,
    MOB_HAZMATGOD,
    MOB_KING,
    MOB_ADVANCEDAGENT,
    MOB_SHADOW,
    MOB_SHADOWKING
};

enum item_type
{
    ITEMTYPE_NONE,
    ITEMTYPE_PISTOL,
    ITEMTYPE_REVOLVER,
    ITEMTYPE_SHOTGUN,
    ITEMTYPE_CHAINGUN,
    ITEMTYPE_FLAMETHROWER,
    ITEMTYPE_CANNON,
    ITEMTYPE_ROCKETLAUNCHER,
    ITEMTYPE_LASERGUN,
    ITEMTYPE_BASICHEALTH,
    ITEMTYPE_ADVANCEDHEALTH,
    ITEMTYPE_SUPERHEALTH,
    ITEMTYPE_ULTRAHEALTH,
    ITEMTYPE_BASICPERMHEALTH,
    ITEMTYPE_ADVANCEDPERMHEALTH,
    ITEMTYPE_SUPERPERMHEALTH,
    ITEMTYPE_ULTRAPERMHEALTH,
    ITEMTYPE_BASICEXPPOWERUP,
    ITEMTYPE_ADVANCEDEXPPOWERUP,
    ITEMTYPE_SUPEREXPPOWERUP,
    ITEMTYPE_GOLDNUGGET,
    ITEMTYPE_BIGNUGGET,
    ITEMTYPE_GOBLET,
    ITEMTYPE_KEYCARD1
};

enum time_stop_color_types
{
    TIMESTOPCT_PLAYER,
    TIMESTOPCT_NPCS,
    TIMESTOPCT_SHADOWS,
    TIMESTOPCT_ITEMS,
    TIMESTOPCT_POWERUPS,
    TIMESTOPCT_PARTICLES,
    TIMESTOPCT_PROPS,
    TIMESTOPCT_EDGEWALLS,
    TIMESTOPCT_BLOCKWALLS,
    TIMESTOPCT_LADDERS,
    TIMESTOPCT_STATICPROPS,
    TIMESTOPCT_DOORS,
    TIMESTOPCT_SWITCHES,
    TIMESTOPCT_BACKDROP
};

#endif
