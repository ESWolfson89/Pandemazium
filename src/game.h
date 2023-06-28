// Copyright Eric Wolfson 2016-2017
// See LICENSE.txt (GPLv3)

#ifndef GAME_H_
#define GAME_H_

#include "graphics.h"
#include "sound.h"
#include "input.h"
#include "entity.h"
#include "generate.h"
#include "menu.h"

#define MAX_PLAYER_EXP_LEVEL 76

#define MAX_PISTOL_EXP_LEVEL 25
#define MAX_REVOLVER_EXP_LEVEL 25
#define MAX_SHOTGUN_EXP_LEVEL 20
#define MAX_CHAINGUN_EXP_LEVEL 10
#define MAX_FLAMETHROWER_EXP_LEVEL 5
#define MAX_CANNON_EXP_LEVEL 20
#define MAX_ROCKETLAUNCHER_EXP_LEVEL 15
#define MAX_LASERGUN_EXP_LEVEL 15

#define NUM_TIMESTOPPED_COLOR_VARIATION 14

// refactor into enumerated values
static const int weapon_texture_indices[NUM_WEAPON_TYPES] =
{
    3, 33, 9, 10, 12, 32, 41, 47
};

static const int max_weapon_exp_levels[NUM_WEAPON_TYPES] =
{
    MAX_PISTOL_EXP_LEVEL,
    MAX_REVOLVER_EXP_LEVEL,
    MAX_SHOTGUN_EXP_LEVEL,
    MAX_CHAINGUN_EXP_LEVEL,
    MAX_FLAMETHROWER_EXP_LEVEL,
    MAX_CANNON_EXP_LEVEL,
    MAX_ROCKETLAUNCHER_EXP_LEVEL,
    MAX_LASERGUN_EXP_LEVEL
};

static const SDL_Color time_stopped_colors[NUM_TIMESTOPPED_COLOR_VARIATION] =
{
    {255,200,255,0},
    {235,235,235,0},
    {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0},
    {255,0,255,0},
    {0,0,0,0},
    {100,100,100,0},
    {0,200,0,0},
    {0,255,0,0},
    {135,255,196,0},
    {255,64,255,0},
    {255,64,255,0},
    {255,32,32,0}
};

static const int weapon_bonus_level_damage_multipliers[NUM_WEAPON_TYPES] =
{
    2,
    3,
    1,
    1,
    1,
    1,
    3,
    1
};

// EXTREMELY IMPORTANT
// set member variable to ms
// maze_size must have odd dimensions (i.e. odd x, odd y for this to work, otherwise it
// will yield invalid results, crash, or hang)
static point level_map_sizes[10] =
{
    point(15.0,15.0),
    point(17.0,15.0),
    point(17.0,19.0),
    point(17.0,21.0),
    point(19.0,21.0),
    point(19.0,23.0),
    point(21.0,23.0),
    point(23.0,23.0),
    point(23.0,25.0),
    point(25.0,27.0)
};

static const int num_starting_enemies_per_level[10] = {
    5,6,7,8,9,10,11,12,13,14
};

static const int max_npc_vector_size[10] =
{
    10,12,15,18,24,30,35,40,50,60
};

static const uint pistol_bonus_levelup_values[MAX_PISTOL_EXP_LEVEL] =
{
    250,
    1000,
    3000,
    7500,
    30000,
    75000,
    300000,
    600000,
    900000,
    3000000,
    6000000,
    9000000,
    30000000,
    60000000,
    90000000,
    300000000,
    600000000,
    900000000,
    1200000000,
    1500000000,
    1800000000,
    2100000000,
    2400000000,
    2700000000,
    3000000000
};

static const uint revolver_bonus_levelup_values[MAX_REVOLVER_EXP_LEVEL] =
{
    500,
    2000,
    6000,
    15000,
    60000,
    150000,
    600000,
    1200000,
    1800000,
    6000000,
    12000000,
    18000000,
    60000000,
    120000000,
    180000000,
    500000000,
    750000000,
    1000000000,
    1200000000,
    1500000000,
    1800000000,
    2100000000,
    2400000000,
    2700000000,
    3000000000
};

static const uint shotgun_bonus_levelup_values[MAX_SHOTGUN_EXP_LEVEL] =
{
    2000,
    6000,
    15000,
    60000,
    600000,
    1500000,
    3000000,
    6000000,
    18000000,
    60000000,
    120000000,
    180000000,
    500000000,
    1000000000,
    1200000000,
    1500000000,
    1800000000,
    2400000000,
    2700000000,
    3000000000
};

static const uint chaingun_bonus_levelup_values[MAX_CHAINGUN_EXP_LEVEL] =
{
    100000,
    250000,
    5000000,
    50000000,
    150000000,
    500000000,
    1000000000,
    1500000000,
    2000000000,
    3000000000
};

static const uint flamethrower_bonus_levelup_values[MAX_FLAMETHROWER_EXP_LEVEL] =
{
    2500000,
    100000000,
    1000000000,
    2000000000,
    3000000000
};

static const uint cannon_bonus_levelup_values[MAX_CANNON_EXP_LEVEL] =
{
    10000,
    20000,
    30000,
    50000,
    500000,
    1500000,
    5000000,
    5000000,
    15000000,
    50000000,
    150000000,
    250000000,
    500000000,
    1000000000,
    1200000000,
    1500000000,
    1800000000,
    2400000000,
    2700000000,
    3000000000
};

static const uint rocketlauncher_bonus_levelup_values[MAX_ROCKETLAUNCHER_EXP_LEVEL] =
{
    50000,
    500000,
    1500000,
    5000000,
    10000000,
    100000000,
    250000000,
    500000000,
    1000000000,
    1200000000,
    1500000000,
    1800000000,
    2400000000,
    2700000000,
    3000000000
};

static const uint lasergun_bonus_levelup_values[MAX_LASERGUN_EXP_LEVEL] =
{
    500000,
    1000000,
    2500000,
    5000000,
    10000000,
    100000000,
    250000000,
    500000000,
    1000000000,
    1200000000,
    1500000000,
    1800000000,
    2400000000,
    2700000000,
    3000000000
};

static const uint exp_levelup_values[MAX_PLAYER_EXP_LEVEL - 1] =
{
    10,
    25,
    100,
    250,
    350,
    500,
    1000,
    2000,
    3000,
    4500,
    6000,
    7500,
    10000,
    20000,
    30000,
    45000,
    60000,
    75000,
    100000,
    200000,
    300000,
    400000,
    500000,
    600000,
    700000,
    800000,
    900000,
    1000000,
    2000000,
    3000000,
    4000000,
    5000000,
    6000000,
    7000000,
    8000000,
    9000000,
    10000000,
    20000000,
    30000000,
    40000000,
    50000000,
    60000000,
    70000000,
    80000000,
    90000000,
    100000000,
    200000000,
    300000000,
    400000000,
    500000000,
    600000000,
    700000000,
    800000000,
    900000000,
    1000000000,
    1100000000,
    1200000000,
    1300000000,
    1400000000,
    1500000000,
    1600000000,
    1700000000,
    1800000000,
    1900000000,
    2000000000,
    2100000000,
    2200000000,
    2300000000,
    2400000000,
    2500000000,
    2600000000,
    2700000000,
    2800000000,
    2900000000,
    3000000000
};

class Game
{
public:
    Game();
    void run();
    void initLevelObjects();
    void initGameStats();
    void initLevelMapSize(int);
    void primaryGameLoop();
    void pollInput();
    void processActions();
    void renderTextures();
    void renderWeaponSkillPanel();
    void updateAnimations();
    void delayGame();
    void cleanupLevelData();
    void applyAI();
    void settleMobsToGround();
    void applyPhysics();
    void applyPhysicsForItem(item *);
    void applyPhysicsForDynamicEntity(dynamic_entity*);
    void applyPhysicsForShadowEntity(mob *, dynamic_entity*);
    void applyHorizontalAcceleration(dynamic_entity*);
    void applyHorizontalResistance(dynamic_entity*);
    void applyGravity(dynamic_entity*);
    void applyCollisions(dynamic_entity*);
    void offsetEntityLoc(dynamic_entity*);
    void createShadowExplosions(mob *);
    void dropKey(mob *);
    void npcWeaponEvent(mob *);
    void npcPowerEvent(mob *);
    void npcMeleeEvent(mob *);
    void npcMoveEvent(mob *);
    void npcDoorEvent(mob *);
    void npcLadderEvent(mob *);
    void mobDoorEvent(mob *);
    void mobClimbLadderEvent(mob *,bool);
    void mobToggleLadderEvent(mob *);
    void updateItemTimers();
    void restartGameEvent();
    void mobTripleFlameAttack(mob *);
    void mobTeleport(mob *);
    void mobFireWeaponProjectile(mob*, item*);
    void mobFireParticle(mob*, item*);
    void mobFireWeapon(mob*);
    void checkForParticleCompression(dynamic_entity*);
    void squirtStaticGibParticles(mob *, int, prop_type, int, bool);
    void squirtDynamicGibParticles(mob *, int, prop_type);
    void addPlayerExperience(mob *, item_type);
    void processParticleEffects();
    void setParticleFlags(particle_type, bool);
    void printTargetIndicator(mob *);
    void addBonusDamage(int &, item_type);
    mob * getMobCarryingItem(int);
    mob * getMobFromEntityID(int);
    item * getItemCarriedByMob(int);
    bool isBoss(mob_type);
    bool checkPickupEvent(mob *, item *);
    bool checkDropEvent(mob *, item *);
    bool checkDamageMobFromProjectile(mob *, mob *);
    bool npcJumpCondition(mob *);
    void checkDamageMobFromParticle(particle *, mob *);
    void checkPlayerExpLevels();
    void checkWeaponExpLevels();
    void checkLevelUpEvent();
    void levelUpPlayer();
    void damageMob(mob *, int, int, item_type);
    void setDeathType(mob*, item_type, int);
    bool projectileDrawerCollidingWithMob(mob *, point);
    bool projectileDrawerHitWall(point);
    bool mobFacingTarget(mob *, mob *);
    bool eitherFromPlayerOrTimeActive(int);
    int numBackdrops();
    int numWallBlocks();
    mob* getPlayerMob();
    void addNPC(mob_type, item_type, point, double);
    void addItem(item_type, point, bool);
    void addPowerup(item_type, point);
    void getOffLadder(mob *, bool);
    void printNPCHealthBar(mob *);
    void checkCollectPowerup();
    void collectHealthPowerup(item *);
    void collectValuablePowerup(item *);
    void collectInventoryItem(item *);
    void updateDoorTimers();
    bool destroyParticleCondition(particle *);
    bool isCollidingWithStaticBlocker(point,point);
    void checkStaticEntityDestruction();
    void createExplosion(point,int,int,double, item_type);
    void applyMobDeathKnockback(mob *, item_type);
    void applyMobProjectileKnockback(mob *,mob *);
    void applyMobParticleKnockback(mob *, particle *);
    door* getDoorConnectedToSwitch(int);
    void endLevelEvent();
    void toggleTime();
    void addTestStartingNPCs();
    // generator:
    void generateMap();
    void addMaze(point,point,point,int);
    void addWallBlockAtLocation(point,point,int);
    void addLadderAtLocation(point,point,point,SDL_Color,LadderType,LadderSnap);
    void addDoorAtLocation(point,point,point,int,bool);
    void addExit(point,point,int);
    //void addSwitchAtLocation(point,point,int);

    void processLevelSpawnEvent();

    void checkTargetIndicatorReset();
    void renderNPCNameStatusIndicator();

    void genStartingNPCs();
    void genOneNPC(mob_type,bool);
    void addSpawnParticle(point);
    mob_type getRandNPC();
    item_type getRandItem(int,int);

    void playFireSound(item_type);
    void playDeathSound(mob_type);

    void updatePlayerTimers();

    void initMainMenu(bool);
    void traverseMainMenu(bool);
    void displayMenu(menu *mu);

    void setWallColorTint();
    void setLadderColorTint();
    void setGlobalTint(bool);

    void executeGamePauseActions();

    SDL_Color getLadderColor();
    SDL_Color getWallColor(int);

private:
    gfx_engine gfx;
    snd_engine sfx;
    input evt_handler;
    std::vector<entity> backdrops;
    std::vector<static_entity> walls;
    std::vector<static_entity> static_props;
    std::vector<Ladder> ladders;
    std::vector<toggle_switch> switches;
    std::vector<door> doors;
    std::vector<mob> npcs;
    std::vector<item> items;
    std::vector<item> powerups;
    std::vector<dynamic_entity> props;
    std::vector<particle> particles;
    std::vector<item> player_inventory;
    mob player_mob;
    //mob test_knight;
    bool quit_flag;
    bool game_ended;
    bool game_paused;
    bool time_stopped;
    point exit_loc;
    point start_loc;
    int score;
    uint exp_points;
    uint weapon_exp[NUM_WEAPON_TYPES];
    int exp_level;
    int weapon_exp_bonus[NUM_WEAPON_TYPES];
    int current_level;
    int npcTargetFocusID;
    int level_increment;
    point current_level_size;
    Uint32 frames_per_second;
    Uint32 frame_start_timer;
    menu main_menu;
    SDL_Color global_tint[NUM_TIMESTOPPED_COLOR_VARIATION];
    SDL_Color color_wall_tint;
    SDL_Color color_ladder_tint;
    int npcIDCounter;
    int playerSlowTimer;
};

void applyMobFireKickBack(mob *, item_type);

std::string int2String(int);
std::string uint2String(uint);

item_type getStartingWeaponForMob(mob_type);
int getWeaponModRow(item *);
int getMobModRow(mob *);
int getWeaponLevelupBoundary(item_type,int);

particle_type getplTypeFromittype(item_type);

point getParticleStartVelocity(particle_type, bool);

point getParticleInsertLoc(item *, particle_type, bool);

bool npcDetectCondition(mob *, mob *);
bool npcAttackCondition(mob *, mob *);
bool isBossLevel(int);

mob_type getBossFromLevel(int);

SDL_Color getMobTint(mob*);

int getAdjustedMobIndex(mob*);

prop_type getGibPieceTypeFromMob(mob_type);

int getGibTextureRow(mob*);


#endif
