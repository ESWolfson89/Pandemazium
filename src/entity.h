// Copyright Eric Wolfson 2016-2017
// See LICENSE.txt (GPLv3)

#ifndef ENTITY_H_
#define ENTITY_H_

#include "point.h"
#include "globals.h"

#define NUM_TOTAL_MOBS 22
#define NUM_TOTAL_ITEMS 24
#define NUM_TOTAL_PROPS 7
#define NUM_TOTAL_PARTICLES 6

#define PLAYER_STARTING_HEALTH 100

static const std::string npc_base_names[NUM_TOTAL_MOBS] = {
    "PLAYER",
    "Fighter",
    "Soldier",
    "Captain",
    "Guard",
    "Big Guard",
    "Elite Guard",
    "Hazmat Guy",
    "Warrior",
    "Slayer",
    "Behemoth",
    "Gladiator",
    "Champion",
    "Grand Champion",
    "Unused",
    "Executioner",
    "Agent",
    "Hazmat God",
    "Unused",
    "Advanced Agent",
    "Shadow",
    "Shadow King"
};

static const int num_npc_level_name_modifiers[NUM_TOTAL_MOBS] =
{
    0,
    3,
    2,
    1,
    3,
    1,
    1,
    1,
    3,
    0,
    1,
    1,
    0,
    1,
    2
};

static const std::string npc_level_name_modifiers[NUM_TOTAL_MOBS][3] =
{
    {"","",""},
    {"Grunt","Soldier","Sergeant"},
    {"Guard","Big Guard",""},
    {"Hazmat Guy","",""},
    {"Warrior","Brute","Behemoth"},
    {"Elite Guard","",""},
    {"Executioner","High Executioner","Master Executioner"},
    {"Agent","",""},
    {"Gladiator","Champion","Grand Champion"},
    {"","",""},
    {"Hazmat God","",""},
    {"King","",""},
    {"","",""},
    {"Advanced Agent","",""},
    {"Shadow","Shadow King"}
};

static const std::string npc_powered_name_modifiers[3] =
{
    "Fast",
    "Tough",
    "Mega"
};

enum physics_type
{
    PHYSTYPE_FULL,
    PHYSTYPE_NO_HORIZ_ACC,
    PHYSTYPE_NO_HORIZ_OR_GRAV_ACC,
    PHYSTYPE_OFF
};

enum door_sizetype
{
    DOORSZE_SMALL,
    DOORSZE_BIG
};

enum door_state
{
    DOORSTATE_CLOSED,
    DOORSTATE_OPENED,
    DOORSTATE_CLOSING,
    DOORSTATE_OPENING
};

enum wall_type {
    WALLTYPE_SOLID,
    WALLTYPE_PARTIAL_TRANSPARENT
};

enum LadderType {
    LADDERTYPE_RUNGS,
    LADDERTYPE_POLE
};

enum LadderSnap {
    LADDERSNAP_CENTER,
    LADDERSNAP_RIGHT,
    LADDERSNAP_LEFT
};

enum particle_type {
    PARTICLE_FLAMESHOT,
    PARTICLE_CANNONBALL,
    PARTICLE_ROCKET,
    PARTICLE_EXPLOSION,
    PARTICLE_LASER,
    PARTICLE_SPAWNFLASH
};

enum prop_type {
    PROP_MINIBLOOD,
    PROP_LARGEBLOOD,
    PROP_MINIASH,
    PROP_LARGEASH,
    PROP_MINIASH2,
    PROP_GIBPIECE1,
    PROP_GIBPIECE2
};

enum item_category {
    ITEMCAT_NONE,
    ITEMCAT_WEAPON,
    ITEMCAT_HEALTH,
    ITEMCAT_VALUABLE,
    ITEMCAT_KEYCARD
};

enum move_type {
    MOVETYPE_STANDINGSTILL,
    MOVETYPE_ROVING
};

enum power_type {
    POWERTYPE_NONE,
    POWERTYPE_TRIPLEFLAMER,
    POWERTYPE_DISARM,
    POWERTYPE_FIREIMMUNE,
    POWERTYPE_TELEPORT,
    POWERTYPE_THROUGHWALLS,
    POWERTYPE_SLOW
};

enum weaponmodifier_type {
    WEAPONMODIFIER_NONE,
    WEAPONMODIFIER_FAST,
    WEAPONMODIFIER_DAMAGING,
    WEAPONMODIFIER_FASTDAMAGING
};

enum mobmodifier_type {
    MOBMODIFIER_NONE,
    MOBMODIFIER_FAST,
    MOBMODIFIER_TOUGH,
    MOBMODIFIER_FASTTOUGH
};

enum MobDeathType {
    MOBDEATHTYPE_NONE,
    MOBDEATHTYPE_KILLED,
    MOBDEATHTYPE_INCINERATED,
    MOBDEATHTYPE_EXPLODED,
    MOBDEATHTYPE_BURST,
    MOBDEATHTYPE_ELECTROEXPLODE
};

struct AnimationSequence {
    std::vector<int> sequence;
};

struct initial_dynamic_entity_fields {
    point dimensions;
    point max_velocity;
    double horizontal_velocity_decrement;
    double speed_up_factor;
    double horizontal_stop_velocity;
    int num_frames;
    int animation_speed;
    int texture_index; // This should be changed to an enum at some point
                       // To make the texture assignments easier to read
};

struct initial_weapon_fields {
    int hp_decrement;
    int mini_hit_gibs;
    int large_hit_gibs;
    double knock_back;
};

struct initial_health_fields {
    int hp_increment;
    int maxhp_increment;
    int explev_increment;
};

struct initial_item_super_fields {
    initial_weapon_fields wep_stats;
    initial_health_fields health_stats;
    item_type i_type;
    item_category i_category;
    int usability_speed;
    int value;
    bool carryable;
    int occur_freq;
    int min_level;
};

struct initial_mob_super_fields {
    mob_type m_type;
    int max_hp;
    int mini_death_gibs;
    int large_death_gibs;
    double jump_strength;
    double alternate_speed_mult_factor;
    bool uses_weapons;
    bool sprite_flips;
    int melee_damage;
    int melee_frequency;
    int shoot_frequency;
    int ladder_use_frequency;
    int door_use_frequency;
    int jump_freq_roller;
    double field_of_view;
    int base_exp_worth;
    int min_level;
    int spawn_freq;
    power_type p_type;
};

struct MobGibData {
    int mini_blood_killed_gibs;
    int large_blood_killed_gibs;
    int mini_ash_incinerated_gibs;
    int mini_ash2_incinerated_gibs;
    int large_ash_incinerated_gibs;
    int mini_blood_explode_gibs;
    int large_blood_explode_gibs;
    int mini_ash_explode_gibs;
    int explode_piece_gibs;
};

struct initial_item_fields {
    initial_dynamic_entity_fields idef;
    initial_item_super_fields iisf;
};

struct initial_mob_fields {
    initial_dynamic_entity_fields idef;
    initial_mob_super_fields imsf;
};

static const AnimationSequence player_ladder_animation_sequence = {
    {6,7,0,7}
};

static const initial_dynamic_entity_fields particle_data[NUM_TOTAL_PARTICLES] = {
    {
        point(50.0,15.0),point(15.0,0.0),0.8,1.0,1.5,3,4,13
    },
    {
        point(10.0,10.0),point(10.0,22.0),0.985,1.2,0.06,1,1,31
    },
    {
        point(14.0,5.0),point(15.0,0.0),1.0,5.0,0.0,1,1,42
    },
    {
        point(110.0,108.0),point(0.0,0.0),0.0,1.0,0.0,5,2,34
    },
    {
        point(25.0,7.0),point(18.0,0.0),1.0,5.0,0.0,1,1,48
    },
    {
        point(28.0,27.0),point(0.0,0.0),0.0,1.0,0.0,5,6,44
    }
};

static const initial_dynamic_entity_fields prop_data[NUM_TOTAL_PROPS] = {
    {
        point(5.0,4.0),point(6.6,30.6),0.85,4.5,0.1,1,1,6
    },
    {
        point(22.0,20.0),point(6.6,35.6),0.9,4.5,0.1,6,4,7
    },
    {
        point(2.0,2.0),point(8.6,30.6),0.85,4.5,5.0,1,1,73
    },
    {
        point(22.0,20.0),point(6.6,35.6),0.9,4.5,5.0,8,4,74
    },
    {
        point(4.0,4.0),point(8.6,30.6),0.9,4.5,5.0,1,1,75
    },
    {
        point(11.0,11.0),point(20.6,30.6),0.9,4.5,5.0,1,1,76
    },
    {
        point(11.0,11.0),point(8.6,35.6),0.9,4.5,5.0,1,1,77
    }
};

static const MobGibData mob_death_gib_count[NUM_TOTAL_MOBS] = {
    {7, 1, 0, 0, 0, 0, 0, 0, 0},
    {5, 1, 3, 3, 2, 12, 2, 15, 3},
    {6, 1, 4, 4, 2, 14, 2, 18, 5},
    {7, 2, 5, 4, 2, 16, 3, 21, 7},
    {6, 1, 3, 3, 2, 15, 2, 20, 5},
    {7, 2, 5, 4, 2, 25, 6, 21, 14},
    {8, 8, 12, 8, 4,25, 2, 30, 18},
    {8, 7, 10, 4, 4, 27, 5, 30, 20},
    {9, 4, 5, 5, 5, 0, 0, 0, 0},
    {13, 6, 10, 8, 7, 0, 0, 0, 0},
    {17, 8, 15, 12, 10, 0, 0, 0, 0},
    {14, 6, 12, 7, 7, 0, 0, 0, 0},
    {17, 7, 15, 10, 9, 0, 0, 0, 0},
    {20, 8, 20, 12, 11, 0, 0, 0, 0},
    {20, 8, 20, 12, 11, 0, 0, 0, 0},
    {12, 6, 12, 6, 6, 23, 4, 35, 16},
    {10, 5, 8, 5, 5, 25, 6, 21, 14},
    {18, 6, 18, 12, 8, 0, 0, 0, 0},
    {18, 6, 18, 12, 8, 0, 0, 0, 0},
    {12, 8, 14, 7, 6, 0, 0, 0, 0},
    {16, 3, 8, 8, 6, 0, 0, 0, 0},
    {25, 6, 8, 8, 6, 0, 0, 0, 0}
};

/*

    "PLAYER",
    "Fighter",
    "Soldier",
    "Captain",
    "Guard",
    "Big Guard",
    "Elite Guard",
    "Hazmat Guy",
    "Warrior",
    "Slayer",
    "Behemoth",
    "Gladiator",
    "Champion",
    "Grand Champion",
    "Unused",
    "Executioner",
    "Agent",
    "Hazmat God",
    "Unused",
    "Advanced Agent",
    "Shadow",
    "Shadow King


*/


static const initial_mob_fields mob_data[NUM_TOTAL_MOBS] =
{
    {
        {point(18.0,38.0),point(11.0,21.0),0.84,5.0,0.1,8,3,4},
        {MOB_PLAYER,PLAYER_STARTING_HEALTH,7,1,12.0,0.5,true,true,0,0,0,100,100,0,800.0,0,1,0,POWERTYPE_NONE}
    },
    {
        {point(14.0,32.0),point(4.0,21.0),0.84,5.0,0.1,6,3,79},
        {MOB_FIGHTER,30,5,1,12.0,0.5,true,true,5,8,5,1,2,30,175.0,3,1,100,POWERTYPE_NONE}
    },
    {
        {point(16.0,38.0),point(4.0,21.0),0.84,5.0,0.1,6,3,19},
        {MOB_SOLDIER,60,6,1,12.0,0.5,true,true,10,8,5,1,2,30,200.0,8,4,25,POWERTYPE_NONE}
    },
    {
        {point(19.0,44.0),point(4.0,21.0),0.84,5.0,0.1,6,3,19},
        {MOB_CAPTAIN,90,7,1,12.0,0.5,true,true,15,8,5,1,2,30,225.0,25,7,10,POWERTYPE_NONE}
    },
    {
        {point(22.0,36.0),point(2.5,21.0),0.84,4.5,0.1,7,3,78},
        {MOB_GUARD,65,6,1,9.0,0.5,true,true,7,7,3,1,2,20,175.0,10,1,50,POWERTYPE_NONE}
    },
    {
        {point(30.0,50.0),point(2.5,21.0),0.84,4.5,0.1,7,3,17},
        {MOB_BIGGUARD,125,7,1,9.0,0.5,true,true,12,7,3,1,2,20,175.0,75,5,10,POWERTYPE_NONE}
    },
    {
        {point(44.0,72.0),point(2.0,21.0),0.8,4.5,0.08,7,6,80},
        {MOB_ELITEGUARD,400,8,8,10.0,0.5,true,true,20,3,30,0,1,45,225.0,1500,8,2,POWERTYPE_NONE}
    },
    {
        {point(44.0,72.0),point(3.0,21.0),0.79,3.0,0.08,7,5,11},
        {MOB_HAZMATGUY,125,8,8,5.0,0.4,true,true,0,0,50,0,0,0,175.0,15,1,15,POWERTYPE_NONE}
    },
    {
        {point(64.0,62.0),point(5.5,30.0),0.75,6.0,0.15,8,4,8},
        {MOB_WARRIOR,400,9,6,12.0,0.3,false,false,25,10,0,0,1,35,400.0,100,2,10,POWERTYPE_NONE}
    },
    {
        {point(76.0,74.0),point(5.5,30.0),0.75,6.0,0.15,8,4,8},
        {MOB_SLAYER,800,9,6,12.0,0.3,false,false,35,10,0,0,1,35,400.0,750,8,12,POWERTYPE_NONE}
    },
    {
        {point(89.0,86.0),point(5.5,30.0),0.75,6.0,0.15,8,4,8},
        {MOB_BEHEMOTH,1500,9,6,12.0,0.3,false,false,60,10,0,0,1,35,400.0,7500,13,15,POWERTYPE_NONE}
    },
    {
        {point(64.0,62.0),point(6.5,30.0),0.75,6.0,0.15,8,4,36},
        {MOB_GLADIATOR,650,14,6,20.0,0.5,false,false,35,25,0,0,1,25,400.0,2000,6,6,POWERTYPE_DISARM}
    },
    {
        {point(76.0,74.0),point(6.5,30.0),0.75,6.0,0.15,8,4,36},
        {MOB_CHAMPION,1500,14,6,20.0,0.5,false,false,50,25,0,0,1,25,400.0,10000,12,5,POWERTYPE_DISARM}
    },
    {
        {point(89.0,86.0),point(6.5,30.0),0.75,6.0,0.15,8,4,36},
        {MOB_GRANDCHAMPION,2500,14,6,20.0,0.5,false,false,60,25,0,0,1,25,400.0,30000,18,4,POWERTYPE_DISARM}
    },
    {
        {point(64.0,112.0),point(10.5,25.0),0.50,1.0,0.15,8,5,40},
        {MOB_RETIREDCHAMPION,1500,18,9,15.0,5.0,false,false,45,25,0,0,1,60,500.0,15000,1000,0,POWERTYPE_DISARM}
    },
    {
        {point(46.0,67.0),point(8.0,21.0),0.85,2.5,0.08,8,7,18},
        {MOB_EXECUTIONER,600,12,8,18.0,0.8,false,true,50,15,0,0,1,70,500.0,600,4,5,POWERTYPE_SLOW}
    },
    {
        {point(29.0,44.0),point(9.0,21.0),0.83,3.0,0.09,7,5,35},
        {MOB_AGENT,350,10,5,12.0,0.5,true,true,15,10,50,1,2,45,400.0,3500,5,9,POWERTYPE_NONE}
    },
    {
        {point(60.0,79.0),point(5.5,21.0),0.50,2.0,0.1,6,9,43},
        {MOB_HAZMATGOD,2000,7,12,4.0,0.5,false,false,0,0,0,0,0,0,150.0,20000,10,1,POWERTYPE_TRIPLEFLAMER}
    },
    {
        {point(64.0,92.0),point(7.5,20.0),0.50,2.0,0.1,8,5,45},
        {MOB_KING,1000,12,8,13.0,0.5,false,false,40,25,0,0,1,800,500.0,30000,10000,0,POWERTYPE_FIREIMMUNE}
    },
    {
        {point(40.0,65.0),point(9.0,21.0),0.83,3.0,0.09,7,7,57},
        {MOB_ADVANCEDAGENT,1200,17,5,12.0,0.5,true,true,20,10,50,0,2,40,550.0,65000,15,1,POWERTYPE_TELEPORT}
    },
    {
        {point(68.0,101.0),point(3.0,3.0),0.6,1.0,0.1,3,10,58},
        {MOB_SHADOW,3500,16,3,1.0,0.8,false,true,100,18,0,0,0,0,850.0,250000,20,1,POWERTYPE_THROUGHWALLS}
    },
    {
        {point(95.0,141.0),point(3.0,3.0),0.6,1.0,0.1,3,10,58},
        {MOB_SHADOWKING,6000,25,3,1.0,0.8,false,true,200,18,0,0,0,0,850.0,1500000,25,1,POWERTYPE_THROUGHWALLS}
    }
};

static const initial_item_fields item_data[NUM_TOTAL_ITEMS] = {
    // SHOULD NEVER EXIST IN ITEM VECTOR (IN GAME OBJECT)
    {
        {point(0.0,0.0),point(0.0,0.0),0.0,0.0,0,0,0},
        {{0,0,0,0.0},{0,0,0},ITEMTYPE_NONE,ITEMCAT_NONE,0,0,true,0,1}
    },
    // real items...
    {
        {point(14.0,12.0),point(12.5,12.5),0.95,3.0,0.05,3,1,3},
        {{10,1,0,15.0},{0,0,0},ITEMTYPE_PISTOL,ITEMCAT_WEAPON,12,0,true,100, 1}
    },
    {
        {point(17.0,11.0),point(11.5,13.2),0.94,3.0,0.06,3,1,33},
        {{28,2,0,20.0},{0,0,0},ITEMTYPE_REVOLVER,ITEMCAT_WEAPON,22,0,true,100, 1}
    },
    {
        {point(27.0,12.0),point(8.5,17.5),0.95,4.5,0.06,3,2,9},
        {{13,1,0,32.0},{0,0,0},ITEMTYPE_SHOTGUN,ITEMCAT_WEAPON,36,0,true,100, 1}
    },
    {
        {point(30.0,18.0),point(4.5,18.5),0.85,6.5,0.07,4,1,10},
        {{8,1,0,25.0},{0,0,0},ITEMTYPE_CHAINGUN,ITEMCAT_WEAPON,4,0,true,100, 3}
    },
    {
        {point(33.0,12.0),point(4.5,18.5),0.85,6.5,0.07,1,1,12},
        {{1,0,0,0.0},{0,0,0},ITEMTYPE_FLAMETHROWER,ITEMCAT_WEAPON,2,0,true,100, 5}
    },
    {
        {point(29.0,21.0),point(4.0,20.0),0.8,6.5,0.07,1,1,32},
        {{35,6,2,5.0},{0,0,0},ITEMTYPE_CANNON,ITEMCAT_WEAPON,75,0,true,100, 8}
    },
    {
        {point(30.0,18.0),point(4.5,19.0),0.8,6.5,0.07,4,35,41},
        {{50,9,4,100.0},{0,0,0},ITEMTYPE_ROCKETLAUNCHER,ITEMCAT_WEAPON,115,0,true,100, 10}
    },
    {
        {point(30.0,16.0),point(3.5,12.0),0.8,6.5,0.07,1,1,47},
        {{25,0,0,50.0},{0,0,0},ITEMTYPE_LASERGUN,ITEMCAT_WEAPON,35,0,true,100, 15}
    },
    {
        {point(14.0,16.0),point(2.5,4.5),0.9,2.5,0.05,1,1,20},
        {{0,0,0,0.0},{25,0,0},ITEMTYPE_BASICHEALTH,ITEMCAT_HEALTH,0,0,false,100, 1}
    },
    {
        {point(14.0,16.0),point(2.5,4.5),0.9,2.5,0.05,1,1,37},
        {{0,0,0,0.0},{50,0,0},ITEMTYPE_ADVANCEDHEALTH,ITEMCAT_HEALTH,0,0,false,75, 1}
    },
    {
        {point(14.0,16.0),point(2.5,4.5),0.9,2.5,0.05,1,1,38},
        {{0,0,0,0.0},{100,0,0},ITEMTYPE_SUPERHEALTH,ITEMCAT_HEALTH,0,0,false,50, 6}
    },
    {
        {point(14.0,16.0),point(2.5,4.5),0.9,2.5,0.05,1,1,49},
        {{0,0,0,0.0},{1000,0,0},ITEMTYPE_ULTRAHEALTH,ITEMCAT_HEALTH,0,0,false,10, 12}
    },
    {
        {point(14.0,16.0),point(2.5,4.5),0.9,2.5,0.05,1,1,50},
        {{0,0,0,0.0},{0,1,0},ITEMTYPE_BASICPERMHEALTH,ITEMCAT_HEALTH,0,0,false,25, 1}
    },
    {
        {point(14.0,16.0),point(2.5,4.5),0.9,2.5,0.05,1,1,51},
        {{0,0,0,0.0},{0,5,0},ITEMTYPE_ADVANCEDPERMHEALTH,ITEMCAT_HEALTH,0,0,false,15, 1}
    },
    {
        {point(14.0,16.0),point(2.5,4.5),0.9,2.5,0.05,1,1,52},
        {{0,0,0,0.0},{0,10,0},ITEMTYPE_SUPERPERMHEALTH,ITEMCAT_HEALTH,0,0,false,10, 5}
    },
    {
        {point(14.0,16.0),point(2.5,4.5),0.9,2.5,0.05,1,1,53},
        {{0,0,0,0.0},{0,25,0},ITEMTYPE_ULTRAPERMHEALTH,ITEMCAT_HEALTH,0,0,false,3, 10}
    },
    {
        {point(14.0,16.0),point(2.5,4.5),0.9,2.5,0.05,1,1,54},
        {{0,0,0,0.0},{0,0,1},ITEMTYPE_BASICEXPPOWERUP,ITEMCAT_HEALTH,0,0,false,2, 10}
    },
    {
        {point(14.0,16.0),point(2.5,4.5),0.9,2.5,0.05,1,1,55},
        {{0,0,0,0.0},{0,0,2},ITEMTYPE_ADVANCEDEXPPOWERUP,ITEMCAT_HEALTH,0,0,false,0, 1000}
    },
    {
        {point(14.0,16.0),point(2.5,4.5),0.9,2.5,0.05,1,1,56},
        {{0,0,0,0.0},{0,0,3},ITEMTYPE_SUPEREXPPOWERUP,ITEMCAT_HEALTH,0,0,false,0, 1000}
    },
    {
        {point(23.0,5.0),point(2.5,7.0),0.9,2.5,0.05,1,1,27},
        {{0,0,0,0.0},{0,0,0},ITEMTYPE_GOLDNUGGET,ITEMCAT_VALUABLE,0,100,false,100, 1}
    },
    {
        {point(31.0,13.0),point(2.5,10.0),0.9,2.5,0.05,1,1,27},
        {{0,0,0,0.0},{0,0,0},ITEMTYPE_BIGNUGGET,ITEMCAT_VALUABLE,0,300,false,90, 1}
    },
    {
        {point(23.0,22.0),point(2.5,9.0),0.9,2.5,0.07,1,1,39},
        {{0,0,0,0.0},{0,0,0},ITEMTYPE_GOBLET,ITEMCAT_VALUABLE,0,1200,false,80, 1}
    },
    {
        {point(12.0,17.0),point(5.0,12.0),0.9,2.5,0.05,1,1,28},
        {{0,0,0,0.0},{0,0,0},ITEMTYPE_KEYCARD1,ITEMCAT_KEYCARD,0,0,false,0, 1}
    }
};

class entity
{
public:
    entity();
    entity(point,point,int,int);
    void setEntityFields(point,point,int,int);
    point getLoc();
    point getDim();
    point getCenter();
    point getMaxLoc();
    point getTextureDim();
    int getTextureIndex();
    int entid();
    int getTextureRow();
    // use carefully:
    void setDim(point);
    void setTextureDim(point);
    void setLoc(point);
    void setMaxLoc();
    void setCenter();
    void setTextureRow(int);
protected:
    point loc;
    point dim;
    point center;
    point max_loc;
    point textureDim;
    int texture_index;
    int texture_row;
    int entity_id;
};

class static_entity : public entity
{
public:
    static_entity();
    void setDestructable(bool);
    void setColorID(int);
    bool isBlocking();
    bool isDestructable();
    int getColorID();
protected:
    bool blocking;
    bool destructable;
    int color_id;
};

class dynamic_entity : public entity
{
public:
    dynamic_entity();
    point getVelocity();
    point getMaxVelocity();
    point getOldLoc();
    bool getVerticalMotionFlag();
    double getXDeltaNormal();
    SDL_RendererFlip getXOrientation();
    void setXDeltaNormal(double);
    void limitVelocity();
    void setVelocity(point);
    void incVelocity(point);
    void setMaxVelocity(point);
    void setVerticalMotionFlag(bool);
    void setDynamicEntityFields(initial_dynamic_entity_fields, point, int);
    void saveLoc();
    void restrictLoc(point,point);
    void incLoc(point);
    void setXOrientation(SDL_RendererFlip);
    void setCurrentFrame(int);
    void setFrameIncrement(int);
    void setAnimationStatus(bool);
    void setResetFrameFlag(bool);
    void animate(int,int);
    void animateCustomSequence(AnimationSequence);
    void setAnimationSpeed(int);
    void togglePhysics(physics_type);
    double horizontalStopVelocity();
    double horizontalVelocityDecrement();
    double speedUpFactor();
    int getFrameIncrement();
    int getCurrentFrame();
    int getNumFrames();
    int getAnimationSpeed();
    bool isAnimating();
    bool getResetFrameFlag();
    bool destroyUponCollision();
    void setDestructableUponCollision(bool);
    bool getMarkForDeletion();
    void setMarkForDeletion();
    physics_type getPhysicsStatus();
protected:
    point velocity;
    point old_loc;
    int current_frame;
    int frame_increment;
    int animation_speed_counter;
    SDL_RendererFlip x_orientation;
    double xdelta_normal;
    bool vertical_motion_flag;
    bool horizontal_motion_flag;
    bool is_animating;
    bool reset_frame_when_done;
    bool destroy_upon_collision;
    bool mark_for_deletion;
    point max_velocity;
    int num_frames;
    int animation_speed;
    double horizontal_stop_velocity;
    double horizontal_velocity_decrement;
    double speed_up_factor;
    physics_type phys_stat;
    int animation_sequence_index;
};

class item : public dynamic_entity
{
public:
    item();
    int getPossessionMobID();
    int getHoldTimer();
    bool getUsability();
    void setPossessionMobID(int);
    void setItemFields(initial_item_fields, point, int);
    void setUsability(bool);
    void incUsageTimer();
    void incHoldTimer();
    void resetHoldTimer();
    void setWeaponModifierType(weaponmodifier_type);
    weaponmodifier_type getWeaponModifierType();
    item_type getItemType();
    item_category getItemCategory();
    initial_item_super_fields * getItemSuperFields();
private:
    int possession_mob_id;
    int usage_timer;
    // unused by most items
    int hold_timer;
    bool can_use;
    weaponmodifier_type wm_type;
    initial_item_super_fields ii_sfields;
};

class mob : public dynamic_entity
{
public:
    mob();
    bool movingFast();
    bool isDead();
    void setMobFields(initial_mob_fields, point, int, double);
    void setMoveFastFlag(bool);
    void setItemCarryID(int);
    void setItemCarryType(item_type);
    void setDeathStatus(bool);
    void setHP(int);
    void incHP(int);
    void setMoveStatus(move_type);
    void setAggroStatus(bool);
    void setLadderStatus(bool);
    void incTilt();
    void setTilt(double);
    void setMaxHP(int);
    void setKeyDropFlag(bool);
    void setMobModifierType(mobmodifier_type);
    void setName(std::string);
    void setDangerLevel(int);
    void setMobDeathType(MobDeathType);
    void setBurningCounter(int);
    bool getLadderStatus();
    bool dropsKey();
    mobmodifier_type getMobModifierType();
    move_type getMoveStatus();
    item_type getItemCarryType();
    bool isAggroed();
    int getBurningCounter();
    int getHP();
    int getDangerLevel();
    int getItemCarryID();
    double getTilt();
    std::string getName();
    mob_type getMobType();
    MobDeathType getMobDeathType();
    initial_mob_super_fields * getMobSuperFields();
private:
    bool moving_fast;
    bool is_dead;
    bool aggroed;
    bool on_ladder;
    bool drops_key;
    move_type move_status;
    double tilt;
    int hitpoints;
    int dangerLevel;
    int item_carry_id;
    item_type item_carry_type;
    initial_mob_super_fields im_sfields;
    mobmodifier_type mm_type;
    std::string name;
    MobDeathType mob_death_type;
    int burningCounter;
};

class particle : public dynamic_entity
{
public:
    particle();
    void setParticleFields(initial_dynamic_entity_fields, point, int, int, int, particle_type, double, item_type);
    particle_type getParticleType();
    item_type getWeaponAssociated();
    int getCreatorID();
    int getDamageValue();
    double getKnockBack();
private:
    int creator_id;
    int damage_value;
    double knock_back;
    particle_type p_type;
    item_type weapon_associated;
};

class door : public static_entity
{
public:
    door();
    door_state getDoorState();
    double useSpeed();
    void setDoorFields(point,point,int,int,door_state,double,bool,door_sizetype);
    void setDoorState(door_state);
    void activateDoor();
    void changeLoc(point);
    void setLockStatus(bool);
    bool isLocked();
    door_sizetype getSizeType();
private:
    door_sizetype d_sizetype;
    door_state current_state;
    double use_speed;
    point closed_loc;
    point opened_loc;
    bool locked;
};

class toggle_switch : public static_entity
{
public:
    toggle_switch();
    int getLevelFeatureID();
    void setSwitchFields(point,point,int,int,int);
private:
    int level_feature_id;
};

class wall : public static_entity {
public:
    wall();
    SDL_Color getTint();
    wall_type getWallType();
    void setWallFeatures(point,point,int,int,SDL_Color,wall_type);
private:
    SDL_Color tint;
    wall_type wl_type;
};

class Ladder : public static_entity {
    public:
        Ladder();
        SDL_Color getLadderTint();
        LadderType getLadderType();
        LadderSnap getLadderSnap();
        void setLadderFeatures(point, point, point, int, SDL_Color, LadderType, LadderSnap);
    private:
        SDL_Color ladderTint;
        LadderType ladderType;
        LadderSnap ladderSnap;
};

bool isDoorStationary(door_state);

bool collisionWithEntity(point, point, entity *);

void checkMarkForDeletion(dynamic_entity *);

void processDynamicEntityWallCollision(dynamic_entity *, static_entity *);

void processDynamicEntityDoorCollision(dynamic_entity *, door *);

int getLadderTexture(LadderType);

#endif
