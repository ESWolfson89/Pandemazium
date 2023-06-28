// Copyright Eric Wolfson 2016-2017
// See LICENSE.txt (GPLv3)

#include "game.h"

Game::Game() {
    initGameStats();
}

void Game::initLevelMapSize(int level) {
    point size = level_map_sizes[level - 1];
    current_level_size = multPoints(size, point(SMALL_BLOCK_DIM,SMALL_BLOCK_DIM));
}

void Game::initGameStats() {
    level_increment = 1;

    score = 0;
    exp_level = 1;
    exp_points = 0;
    npcTargetFocusID = -1;
    current_level = 1;

    frames_per_second = 50;
    frame_start_timer = frames_per_second * 2;

    for (int i = 0; i < NUM_WEAPON_TYPES; ++i) {
        weapon_exp[i] = 0U;
        weapon_exp_bonus[i] = 0;
    }

    initLevelMapSize(current_level);

    quit_flag = false;
    game_paused = false;
};

void Game::checkLevelUpEvent() {
    checkPlayerExpLevels();
    checkWeaponExpLevels();
}

// Possibly advance player to next experience level...
void Game::checkPlayerExpLevels() {
    // player can't level up when dead
    if (getPlayerMob()->isDead())
        return;

    uint init_level = exp_level;
    // player can't level up beyond MAX_PLAYER_EXP_LEVEL
    if (exp_level < MAX_PLAYER_EXP_LEVEL) {
        // If player has still not reached enough exp points (next levelup value):
        // don't level up
        if (exp_points >= exp_levelup_values[init_level-1]) {
            // level up one, or multiple times if killing an enemy causes
            // exp_points to surpass two "exp_levelup_values[]" values
            for (int i = init_level; i < MAX_PLAYER_EXP_LEVEL; ++i) {
                if (exp_points >= exp_levelup_values[i-1])
                    levelUpPlayer();
                else
                    break;
            }
        }
    }
}

void Game::checkWeaponExpLevels() {
    // check for increment of weapon skill levels
    uint next_weapon_bonus_boundary = 1U;
    uint weapon_bonus_level_boundary = 1U;
    for (int i = 0; i < NUM_WEAPON_TYPES; ++i) {
         if (weapon_exp_bonus[i] < max_weapon_exp_levels[i]) {
            uint weapon_level = weapon_exp_bonus[i];

            next_weapon_bonus_boundary = getWeaponLevelupBoundary((item_type)(i+1),weapon_level);

            if (weapon_exp[i] >= next_weapon_bonus_boundary) {
                for (int j = weapon_level; j < max_weapon_exp_levels[i]; ++j) {
                    weapon_bonus_level_boundary = getWeaponLevelupBoundary((item_type)(i+1),j);
                    if (weapon_exp[i] >= weapon_bonus_level_boundary)
                        weapon_exp_bonus[i]++;
                    else
                        break;
                }
            }
        }
    }
}

void Game::levelUpPlayer() {
    exp_level++;
    // The player should have 1.05 times the previous amount of hitpoints had for each levelup
    getPlayerMob()->setMaxHP((int)((double)getPlayerMob()->getMobSuperFields()->max_hp*(1.05)) + randInt(0,exp_level));
    // The player's hitpoints should be reset to maximum if at least one levelup is performed
    getPlayerMob()->setHP(getPlayerMob()->getMobSuperFields()->max_hp);
    // Play level up sound
    sfx.playSoundEffect(sfx.getSoundEffect((int)SOUNDTYPE_LEVELUP));
}

// initialize the main menu
void Game::initMainMenu(bool reset)
{
    if (!reset)
        main_menu = menu(point(0.0,0.0),point(WINDOW_WIDTH/2.0,WINDOW_HEIGHT/2.0));
    else
        main_menu.cleanupEverything();

    main_menu.addMenuItem("New Game                        ");
    main_menu.addMenuItem("High Scores (not implemented)   ");
    main_menu.addMenuItem("How To Play (use README for now)");
    main_menu.addMenuItem("Quit Game                       ");
}

// Display a menu object
void Game::displayMenu(menu *mu)
{
    point loc;
    loc.set(mu->getLoc().x() + mu->getArea().x() - (FONT_CHAR_WIDTH*((int)mu->getMenuTitle().size()/2.0)),
            mu->getLoc().y() + mu->getArea().y()/2.0);
    gfx.addBitmapString(color_white,mu->getMenuTitle(),loc);
    for (int i = 0; i < mu->getNumMenuItems(); ++i)
    {
        loc.set(mu->getLoc().x()+mu->getArea().x() - (FONT_CHAR_WIDTH*((int)mu->getMenuItem(i).size()/2.0)),
                mu->getLoc().y()+mu->getArea().y() + i*FONT_CHAR_HEIGHT);
        gfx.addBitmapString(color_white,mu->getMenuItem(i),loc);
        if (mu->getSelectionIndex() == i)
            gfx.addBitmapCharacter(color_white,26,addPoints(loc,point(-FONT_CHAR_HEIGHT*2.0,0.0)));
    }
}

// Main starting menu traversal
void Game::traverseMainMenu(bool reset)
{
    bool exit_main_menu = false;
    do {
        frame_start_timer = SDL_GetTicks();
        evt_handler.processKey();

        if (main_menu.canChangeSelection())
        if (evt_handler.deltaKeyPressed()) {
            main_menu.incSelectionIndex(evt_handler.getDelta().y());
            main_menu.setSelectionChangeFlag(false);
        }

        if (evt_handler.selectKeyPressed()) {
            exit_main_menu = true;
            if (main_menu.getSelectionIndex() == 3)
                quit_flag = true;
            if (main_menu.getSelectionIndex() == 4 || (main_menu.getSelectionIndex() == 0 && reset))
                restartGameEvent();
        }

        main_menu.activateSelectionChangeTimer();

        gfx.clearScreen();
        gfx.renderSprite(gfx.getTexture(59),main_menu.getLoc(),main_menu.getArea(),main_menu.getArea(),0,0,SDL_FLIP_NONE,true,color_black,2);
        displayMenu(&main_menu);
        gfx.addBitmapString(color_white,"Copyright Eric Wolfson 2016-2017",point((double)WINDOW_WIDTH/3.0 - 60.0,(double)WINDOW_HEIGHT - 200.0));
        gfx.updateScreen();
        delayGame();
    } while(!exit_main_menu);
}

// Build map's maze layout
void Game::generateMap() {
    terrain_map t_generator;
    t_generator.createTerrainMap(level_map_sizes[(int)std::min(9,current_level - 1)],
                                 point(SMALL_BLOCK_DIM,SMALL_BLOCK_DIM),current_level);
    point mloc = point(0.0,0.0);
    point bsze = t_generator.getBlockSize();
    point aloc;
    point asze;
    int tid = 0;
    for (int y = 0; y < t_generator.getDim().y(); ++y) {
        for (int x = 0; x < t_generator.getDim().x(); ++x) {
            tid = t_generator.getTerrainStruct(point(x,y)).tid;
            asze = t_generator.getTerrainStruct(point(x,y)).sze;
            aloc = point(mloc.x()+(x*bsze.x()),mloc.y()+(y*bsze.y()));
            switch(t_generator.getTerrainStruct(point(x,y)).t_type) {
                case(TERRAINTYPE_WALL):
                     addWallBlockAtLocation(aloc,asze,tid);
                     break;
                case(TERRAINTYPE_LADDER):
                     addLadderAtLocation(aloc,asze,bsze,color_darkorange,LADDERTYPE_RUNGS,LADDERSNAP_CENTER);
                     break;
                case(TERRAINTYPE_LADDER_2):
                     addLadderAtLocation(aloc,asze,bsze,color_black,LADDERTYPE_POLE,LADDERSNAP_LEFT);
                     break;
                case(TERRAINTYPE_DOOR):
                     addDoorAtLocation(aloc,bsze,asze,tid,false);
                     break;
                case(TERRAINTYPE_BRICKBACKDROP_1):
                     backdrops.push_back(entity(aloc,asze,tid,(int)backdrops.size()));
                     break;
                case(TERRAINTYPE_BIGDOOR):
                     addDoorAtLocation(aloc,bsze,asze,tid,false);
                     break;
                case(TERRAINTYPE_LOCKEDDOOR):
                     addDoorAtLocation(aloc,bsze,asze,tid,true);
                     break;
                case(TERRAINTYPE_ITEMPLACEHOLDER):
                     if (roll(2))
                         addPowerup(getRandItem((int)ITEMTYPE_BASICHEALTH,(int)ITEMTYPE_SUPEREXPPOWERUP),aloc);
                     else
                         addPowerup(getRandItem((int)ITEMTYPE_GOLDNUGGET,(int)ITEMTYPE_GOBLET),aloc);
                     break;
                case(TERRAINTYPE_ITEMPLACEHOLDER2):
                     addItem(getRandItem((int)ITEMTYPE_PISTOL, (int)ITEMTYPE_LASERGUN),aloc,true);
                     break;
                case(TERRAINTYPE_KEY):
                     addPowerup((item_type)ITEMTYPE_KEYCARD1,aloc);
                     break;
                case(TERRAINTYPE_EXIT):
                     addExit(aloc,asze,tid);
                     break;
                default:
                     break;
            }
        }
    }
    t_generator.cleanupTerrainMap();
    start_loc = multPoints(t_generator.getStartBlock(),point(40.0,40.0));
    start_loc = addPoints(start_loc, point(10.0,1.0));
}

// Add NPC to npc vector
// Add Its item it might start with
// Have NPC equip item
// Set NPC modifier flags (represented on screen by the color of the NPC)
void Game::addNPC(mob_type m_type, item_type i_type, point loc, double x_delta)
{
    npcs.push_back(mob());
    npcIDCounter++;
    int index = (int)npcs.size() - 1;
    int default_hp = mob_data[(int)m_type].imsf.max_hp;
    npcs[index].setMobFields(mob_data[(int)m_type],loc,npcIDCounter,x_delta);
    npcs[index].setMobModifierType(MOBMODIFIER_NONE);
    npcs[index].setName("");
    if (current_level > 19)
    {
        npcs[index].setMaxHP(npcs[index].getMobSuperFields()->max_hp + (current_level - 15));
        npcs[index].setHP(npcs[index].getMobSuperFields()->max_hp);
    }
    if (rollPerc(current_level))
    {
        switch(randInt(0,3))
        {
        case(0):
              npcs[index].setMobModifierType(MOBMODIFIER_FAST);
              npcs[index].setName(npc_powered_name_modifiers[0]);
              break;
        case(1):
              npcs[index].setMobModifierType(MOBMODIFIER_TOUGH);
              npcs[index].setName(npc_powered_name_modifiers[1]);
              break;
        case(2):
              npcs[index].setMobModifierType(MOBMODIFIER_FASTTOUGH);
              npcs[index].setName(npc_powered_name_modifiers[2]);
              break;
        default:
              break;
        }
    }
    if (i_type != ITEMTYPE_NONE) {
        addItem(i_type, loc, true);
        checkPickupEvent(&npcs[index], &items[(int)items.size()-1]);
    }
    if (isBoss(m_type))
        npcs[index].setAggroStatus(true);
    npcs[index].setKeyDropFlag(false);
}

bool Game::isBoss(mob_type mt)
{
    if ((mt == MOB_ADVANCEDAGENT) ||
        (mt == MOB_SHADOW) ||
        (mt == MOB_SHADOWKING))
        return true;
    return false;
}

// add equippable item centered on top of wall block
// All items are weapons (so far)
// So each item has a weapon modifier flag (represented by color of weapon)
void Game::addItem(item_type i_type, point loc, bool can_be_modified)
{
    items.push_back(item());
    int index = (int)items.size() - 1;
    point dim = item_data[(int)i_type].idef.dimensions;
    point aloc = addPoints(loc,multPoints(dim,point(-0.5,-0.5)));
    aloc = addPoints(aloc,point(20.0,20.0));
    aloc.sety(loc.y()+40.0-dim.y());
    items[index].setItemFields(item_data[(int)i_type],aloc,index);
    items[index].setWeaponModifierType(WEAPONMODIFIER_NONE);
    if (rollPerc((current_level/2 + 1)) && can_be_modified)
    {
        switch(randInt(0,3))
        {
        case(0):
              items[index].setWeaponModifierType(WEAPONMODIFIER_FAST);
              break;
        case(1):
              items[index].setWeaponModifierType(WEAPONMODIFIER_DAMAGING);
              break;
        case(2):
              items[index].setWeaponModifierType(WEAPONMODIFIER_FASTDAMAGING);
              break;
        default:
              break;
        }
    }
}

// Add powerup centered on top of wall block
void Game::addPowerup(item_type i_type, point loc)
{
    powerups.push_back(item());
    int index = (int)powerups.size() - 1;
    point dim = item_data[(int)i_type].idef.dimensions;
    point aloc = addPoints(loc,multPoints(dim,point(-0.5,-0.5)));
    aloc = addPoints(aloc,point(20.0,20.0));
    aloc.sety(loc.y()+40.0-dim.y());
    powerups[index].setItemFields(item_data[(int)i_type],aloc,index);
}

// Add wall block
void Game::addWallBlockAtLocation(point loc, point sze, int tid)
{
    walls.push_back(static_entity());
    walls[(int)walls.size() - 1].setEntityFields(loc,sze,tid,(int)walls.size()-1);
    walls[(int)walls.size() - 1].setDestructable(true);
    if ((int)walls.size() > 4) {
        if (sze.y() == 15.0) {
            walls[(int)walls.size() - 1].setLoc(point(loc.x(),loc.y() + 25.0));
            walls[(int)walls.size() - 1].setColorID(0);
        }
        else
            walls[(int)walls.size() - 1].setColorID(0);
    }
}

// Add ladder segment
void Game::addLadderAtLocation(point loc, point lsize, point bsize, SDL_Color ladderTint, LadderType ladderType, LadderSnap ladderSnap) {
    ladders.push_back(Ladder());
    ladders[(int)ladders.size() - 1].setLadderFeatures(loc, lsize, bsize, (int)ladders.size() - 1, ladderTint,ladderType, ladderSnap);
}

// Add exit object
void Game::addExit(point loc, point esze, int tid)
{
    exit_loc = loc;
    static_props.push_back(static_entity());
    static_props[(int)static_props.size() - 1].setEntityFields(loc,esze,tid,(int)static_props.size() - 1);
}

// add door with necessary switches used for opening the door
// locked doors have switch on one side only
// unlocked doors have switches on both sides
void Game::addDoorAtLocation(point loc, point bsize, point sze, int tid, bool locked)
{
    double x_offset = 9.0;
    double y_offset = 12.0;

    door_sizetype dst;

    if (sze.y() == 40.0)
    {
        y_offset = 12.0;
        dst = DOORSZE_SMALL;
    }
    if (sze.y() == 80.0)
    {
        y_offset = 52.0;
        dst = DOORSZE_BIG;
    }

    doors.push_back(door());
    doors[(int)doors.size() - 1].setDoorFields(point(loc.x()+x_offset,loc.y()),sze,tid,(int)doors.size()-1,DOORSTATE_CLOSED,10.0,locked,dst);

    static_props.push_back(static_entity());

    if (locked)
        static_props[(int)static_props.size() - 1].setEntityFields(loc,point(bsize.x(),sze.y()),25,(int)static_props.size() - 1);
    else
        static_props[(int)static_props.size() - 1].setEntityFields(loc,point(bsize.x(),sze.y()),tid+1,(int)static_props.size() - 1);

    if (!locked || loc.x() > MAP_WIDTH/2.0)
    {
        switches.push_back(toggle_switch());
        switches[(int)switches.size() - 1].setSwitchFields(point(loc.x()-4.0,loc.y()+y_offset),point(18.0,28.0),21,(int)switches.size()-1,(int)doors.size() - 1);
    }
    if (!locked || loc.x() < MAP_WIDTH/2.0)
    {
        switches.push_back(toggle_switch());
        switches[(int)switches.size() - 1].setSwitchFields(point(loc.x()+bsize.x() - 13.0,loc.y()+y_offset),point(18.0,28.0),21,(int)switches.size()-1,(int)doors.size() - 1);
    }
}

// Initialize everything on level
void Game::initLevelObjects() {
    npcTargetFocusID = -1;
    // set level colors
    setWallColorTint();
    setLadderColorTint();

    backdrops.push_back(entity(point(0.0,0.0),point(WINDOW_WIDTH/2,WINDOW_HEIGHT/2),0,0));

    // create 4 wall boundaries (N,S,E,W)
    for (int i = 0; i < 4; ++i)
    {
        walls.push_back(static_entity());
    }

    // set all wall boundaries position and size
    walls[0].setEntityFields(point(0.0,0.0),point(MAP_WIDTH,40.0),1,0);
    walls[1].setEntityFields(point(0.0,MAP_HEIGHT-40.0),point(MAP_WIDTH,40.0),1,1);
    walls[2].setEntityFields(point(0.0,40.0),point(40.0,MAP_HEIGHT - 80.0),14,2);
    walls[3].setEntityFields(point(MAP_WIDTH-40.0,40.0),point(40.0,MAP_HEIGHT - 80.0),14,3);

    // create map wall layout, doors, powerups, items, exit, etc...
    generateMap();

    // Put player at bottom right, or bottom left of level to start.
    if (roll(2))
    {
        if (current_level == 1)
            player_mob.setMobFields(mob_data[(int)MOB_PLAYER],point(45.0,MAP_HEIGHT-82.0),0,1.0);
        else
            player_mob.setLoc(point(45.0,MAP_HEIGHT-82.0));

        player_mob.setXDeltaNormal(1.0);
        player_mob.setXOrientation(SDL_FLIP_NONE);
    }
    else
    {
        if (current_level == 1)
            player_mob.setMobFields(mob_data[(int)MOB_PLAYER],point(MAP_WIDTH-63.0,MAP_HEIGHT-82.0),0,-1.0);
        else
            player_mob.setLoc(point(MAP_WIDTH-63.0,MAP_HEIGHT-82.0));

        player_mob.setXDeltaNormal(-1.0);
        player_mob.setXOrientation(SDL_FLIP_HORIZONTAL);
    }

    // Create player's starting pistol
    if (current_level == 1)
    {
        addItem(ITEMTYPE_PISTOL,player_mob.getLoc(),false);
        items[(int)items.size()-1].setWeaponModifierType(WEAPONMODIFIER_NONE);
        checkPickupEvent(getPlayerMob(),&items[(int)items.size()-1]);
        getPlayerMob()->setKeyDropFlag(false);
    }

    npcIDCounter = 0;

    playerSlowTimer = 0;

    genStartingNPCs();

    time_stopped = false;

    setGlobalTint(false);

    settleMobsToGround();
}

void Game::setGlobalTint(bool tinted)
{
    if (current_level <= BOSS_LEVEL3 && tinted)
        return;

    for (int i = 0; i < NUM_TIMESTOPPED_COLOR_VARIATION; ++i)
    {
         global_tint[i] = (tinted ? time_stopped_colors[i] : color_black);
    }
}

// create an NPC
void Game::genOneNPC(mob_type m_type, bool spawn_npc)
{
    // minimum distance from player
    double min_dist = 280.0;
    point occur_loc,occur_dim,occur_center;
    int roller_times = 0;
    // find its location
    do
    {
        roller_times++;

        if (roller_times >= 100 && spawn_npc)
            return;

        if (m_type != MOB_SHADOW && m_type != MOB_SHADOWKING)
            occur_loc = point((double)randInt(80.0,MAP_WIDTH-120.0),(double)randInt(80.0,MAP_HEIGHT-120.0));
        else
            occur_loc = point((double)randInt(80.0,MAP_WIDTH-120.0),(double)randInt(80.0,MAP_HEIGHT-160.0));
        occur_dim = mob_data[(int)m_type].idef.dimensions;
        occur_center = addPoints(occur_loc,multPoints(occur_dim,point(0.5,0.5)));
    }while((isCollidingWithStaticBlocker(occur_center,occur_dim) && m_type != MOB_SHADOW && m_type != MOB_SHADOWKING) || distanceLongestAxis(occur_loc,player_mob.getLoc()) <= min_dist);
    // add it in (with possible spawn particle)
    if (spawn_npc)
        addSpawnParticle(addPoints(occur_center,point(-13.0,-13.0)));
    // The last parameter represents starting x-orientation
    addNPC(m_type,getStartingWeaponForMob(m_type),occur_loc,-1.0 + (double)(2 * randZero(1)));
    if (m_type == MOB_SOLDIER || m_type == MOB_CAPTAIN) {
        npcs[(int)npcs.size() - 1].setTextureDim(point(36.0,76.0));
    }
    else if (m_type == MOB_FIGHTER) {
        npcs[(int)npcs.size() - 1].setTextureDim(point(18.0, 38.0));
    }
    else if (m_type == MOB_BIGGUARD) {
        npcs[(int)npcs.size() - 1].setTextureDim(point(44.0, 72.0));
    }
    else if (m_type == MOB_SLAYER || m_type == MOB_BEHEMOTH || m_type == MOB_CHAMPION || m_type == MOB_GRANDCHAMPION) {
        npcs[(int)npcs.size() - 1].setTextureDim(point(64.0, 62.0));
    }
    else if (m_type == MOB_SHADOWKING) {
        npcs[(int)npcs.size() - 1].setTextureDim(point(68.0, 101.0));
    }
    else {
        npcs[(int)npcs.size() - 1].setTextureDim(npcs[(int)npcs.size()-1].getDim());
    }
    if (npcs[(int)npcs.size() - 1].getName() == "")
        npcs[(int)npcs.size() - 1].setName(npc_base_names[(int)m_type]);
    else
        npcs[(int)npcs.size() - 1].setName(npcs[(int)npcs.size()-1].getName() + " " + npc_base_names[(int)m_type]);
}

void Game::mobTeleport(mob *mb) {
    point occur_loc,occur_dim,occur_center;

    occur_loc = point((double)randInt(80.0,MAP_WIDTH-120.0),(double)randInt(80.0,MAP_HEIGHT-120.0));
    occur_dim = mob_data[(int)mb->getMobType()].idef.dimensions;
    occur_center = addPoints(occur_loc,multPoints(occur_dim,point(0.5,0.5)));

    if (!isCollidingWithStaticBlocker(occur_center,occur_dim)) {
        addSpawnParticle(addPoints(mb->getCenter(),point(-13.0,-13.0)));

        mb->setLoc(occur_loc);

        addSpawnParticle(addPoints(occur_center,point(-13.0,-13.0)));
    }
}

// This is an aesthetic particle used for making it easier to see when and where
// an NPC spawns after level creation.
void Game::addSpawnParticle(point loc) {
    particles.push_back(particle());
    int index = (int)particles.size() - 1;
    particles.back().setParticleFields(particle_data[(int)PARTICLE_SPAWNFLASH],loc,index,-1,0,PARTICLE_SPAWNFLASH,0.0,ITEMTYPE_NONE);
    particles.back().setAnimationStatus(true);
}

// Generate all NPCs present at start of level
void Game::genStartingNPCs() {
    int index;

    int num_enemies = num_starting_enemies_per_level[std::min(10,current_level)-1];

    for (int i = 0; i <= num_enemies; ++i)
        genOneNPC(getRandNPC(),false);
}

// Generate random NPC
mob_type Game::getRandNPC() {
    int roller_times = 0;
    mob_type ret_val;
    bool spawn_freq;
    do {
        roller_times++;

        ret_val = (mob_type)randInt((int)MOB_FIGHTER,(int)MOB_SHADOWKING);
        if (ret_val == MOB_SHADOW)
            spawn_freq = rollPerc(10);
        else if (ret_val == MOB_SHADOWKING)
            spawn_freq = rollPerc(5);
        else if (ret_val == MOB_ADVANCEDAGENT)
            spawn_freq = rollPerc(30);
        else if (ret_val == MOB_KING)
            spawn_freq = rollPerc(20);
        else if (ret_val == MOB_SLAYER || ret_val == MOB_BEHEMOTH || ret_val == MOB_CHAMPION || ret_val == MOB_GRANDCHAMPION)
            spawn_freq = rollPerc((int)std::min(20, mob_data[ret_val].imsf.spawn_freq + current_level - 1));
        else
            spawn_freq = rollPerc(mob_data[ret_val].imsf.spawn_freq + current_level - 1);
    } while (mob_data[ret_val].imsf.min_level > current_level || (!spawn_freq && roller_times < 50) );

    return ret_val;
}

// Generate random item in a certain range (of item_type enum)
item_type Game::getRandItem(int start_val, int end_val) {
    int roller_times = 0;
    item_type ret_val;
    do {
        roller_times++;
        ret_val = (item_type)randInt(start_val, end_val);
    } while ((!rollPerc(item_data[(int)ret_val].iisf.occur_freq) || item_data[(int)ret_val].iisf.min_level > current_level) && roller_times < 1000);
    return ret_val;
}

// function called from main.cpp
void Game::run() {
    // initialize graphics and sound
    if(gfx.initSDL() && sfx.initMixer()) {
        initMainMenu(false);
        traverseMainMenu(false);
        initLevelObjects();
        primaryGameLoop();
    }
    else {
        std::cout << "Failed to initialize... press enter to terminate.";
        std::cin.get();
        return;
    }

    // cleanup before exiting
    cleanupLevelData();
}

// Clear every level entity.
// Called when a new level needs to be constructed.
void Game::cleanupLevelData() {
    std::vector<entity>().swap(backdrops);
    std::vector<static_entity>().swap(walls);
    std::vector<static_entity>().swap(static_props);
    std::vector<Ladder>().swap(ladders);
    std::vector<toggle_switch>().swap(switches);
    std::vector<door>().swap(doors);
    std::vector<mob>().swap(npcs);
    std::vector<item>().swap(items);
    std::vector<item>().swap(powerups);
    std::vector<dynamic_entity>().swap(props);
    std::vector<particle>().swap(particles);
}

// Apply time stop flag changes
void Game::toggleTime()
{
    time_stopped = !time_stopped;

    setGlobalTint(time_stopped);

    physics_type npcphys = (time_stopped ? PHYSTYPE_NO_HORIZ_OR_GRAV_ACC : PHYSTYPE_FULL);
    physics_type miscphys = (time_stopped ? PHYSTYPE_OFF : PHYSTYPE_FULL);

    for (int i = 0; i < (int)npcs.size(); ++i)
    {
         if (npcs[i].getMobType() != MOB_SHADOW)
         {
             if (time_stopped)
                 npcs[i].setVelocity(point(0.0,0.0));
             npcs[i].togglePhysics(npcphys);
         }
    }

    for (int i = 0; i < (int)items.size(); ++i)
         items[i].togglePhysics(eitherFromPlayerOrTimeActive(items[i].getPossessionMobID()) ? PHYSTYPE_FULL : PHYSTYPE_OFF);

    for (int i = 0; i < (int)props.size(); ++i)
         props[i].togglePhysics(miscphys);

    for (int i = 0; i < (int)particles.size(); ++i)
         particles[i].togglePhysics(miscphys);
}

// apply physics for anything that moves
void Game::applyPhysicsForDynamicEntity(dynamic_entity *de)
{
    if (de->getPhysicsStatus() != PHYSTYPE_OFF)
    {
        de->saveLoc();
        applyHorizontalAcceleration(de);
        applyHorizontalResistance(de);
        applyGravity(de);
        offsetEntityLoc(de);
        applyCollisions(de);
    }
}

// trick: pass in super class and class to manipulate both
void Game::applyPhysicsForShadowEntity(mob *m, dynamic_entity *de)
{
    de->saveLoc();

    applyHorizontalAcceleration(de);
    applyHorizontalResistance(de);
    if (m->isDead() && m->getLoc().y() < MAP_HEIGHT + 100.0)
    {
        if (roll(3))
            createShadowExplosions(m);
        applyGravity(de);
    }
    offsetEntityLoc(de);
}

void Game::createShadowExplosions(mob *mb)
{
    point loc;
    loc = point(randInt(0,100),(randInt(0,100)));
    addSpawnParticle(addPoints(mb->getCenter(),point(-63.0+loc.x(),-63.0+loc.y())));
    if (roll(15))
        squirtDynamicGibParticles(mb, mob_death_gib_count[getAdjustedMobIndex(mb)].large_blood_killed_gibs, PROP_LARGEBLOOD);
    if (roll(10))
        squirtStaticGibParticles(mb, mob_death_gib_count[getAdjustedMobIndex(mb)].mini_blood_killed_gibs, PROP_MINIBLOOD,0,false);
}

bool Game::isCollidingWithStaticBlocker(point center, point dim) {
    for (auto it = walls.begin(); it != walls.end(); ++it) {
        if (collisionWithEntity(center, dim, &*it))
            return true;
    }

    for (auto it = doors.begin(); it != doors.end(); ++it) {
	if (collisionWithEntity(center, dim, &*it))
	    return true;
    }

    return false;
}

// Check if dynamic object "de" is colliding with a static blocking object (wall or door)
void Game::applyCollisions(dynamic_entity *de) {
    for (auto it = walls.begin(); it != walls.end(); ++it)
         processDynamicEntityWallCollision(de, &*it);

    for (auto it = doors.begin(); it != doors.end(); ++it)
         processDynamicEntityDoorCollision(de, &*it);
}

// The next 2 functions combined allow for smooth speedup, slow down movement of object

// Apply horizontal air resistance to object
void Game::applyHorizontalResistance(dynamic_entity *de)
{
    de->setVelocity(point(de->getVelocity().x()*de->horizontalVelocityDecrement(), de->getVelocity().y()));

    if (std::abs(de->getVelocity().x()) <= de->horizontalStopVelocity())
        de->setVelocity(point(0.0,de->getVelocity().y()));
}

// accelerate object horizontally via object's speedup factor
void Game::applyHorizontalAcceleration(dynamic_entity *de)
{
    if (de->getPhysicsStatus() != PHYSTYPE_NO_HORIZ_ACC && de->getPhysicsStatus() != PHYSTYPE_NO_HORIZ_OR_GRAV_ACC)
        de->incVelocity(point((de->getMaxVelocity().x()/de->speedUpFactor())*de->getXDeltaNormal(), 0.0));
}

// apply gravity to dynamic entity
void Game::applyGravity(dynamic_entity *de) {
    if (de->getPhysicsStatus() != PHYSTYPE_NO_HORIZ_OR_GRAV_ACC)
        de->incVelocity(point(0.0, GRAVITY_VELOCITY_INCREMENT));
}

// move dynamic entity
void Game::offsetEntityLoc(dynamic_entity *de) {
    de->incLoc(de->getVelocity());
}

// mob "mb" picks up item "it"
bool Game::checkPickupEvent(mob *mb, item *it) {
    if (it->getPossessionMobID() == -1 && mb->getItemCarryID() == -1) {
        it->setPossessionMobID(mb->entid());
        mb->setItemCarryID(it->entid());
        mb->setItemCarryType(it->getItemType());
        return true;
    }
    return false;
}

// mob "mb" drops item "it"
// It may go flying depending on velocity of mb
bool Game::checkDropEvent(mob *mb, item *it)
{
    if (it->getPossessionMobID() == mb->entid())
    {
        mb->setItemCarryID(-1);
        mb->setItemCarryType(ITEMTYPE_NONE);
        it->setPossessionMobID(-1);
        it->setLoc(addPoints(mb->getCenter(),multPoints(it->getDim(),point(-0.5,-0.5))));
        it->setVelocity(point(mb->getVelocity().x()*MOB_ITEM_DROP_VELOCITY_MODIFIER_X,-1.0*std::abs(mb->getVelocity().y()*MOB_ITEM_DROP_VELOCITY_MODIFIER_Y)));
        it->setAnimationStatus(false);
        it->togglePhysics(PHYSTYPE_FULL);
        return true;
    }
    return false;
}

// Check if inventory, stats related, or score related
// item was picked up. These are non-equippable items.
void Game::checkCollectPowerup() {
    if (!getPlayerMob()->isDead()) {
        for (auto it = powerups.begin(); it != powerups.end(); ++it) {
             if (collisionWithEntity(player_mob.getCenter(),player_mob.getDim(),&*it)) {
                 switch(it->getItemSuperFields()->i_category) {
                        case(ITEMCAT_HEALTH):
                             collectHealthPowerup(&*it);
                             break;
                        case(ITEMCAT_VALUABLE):
                             collectValuablePowerup(&*it);
                             break;
                        case(ITEMCAT_KEYCARD):
                             collectInventoryItem(&*it);
                             break;
                        default:
                             break;
                 }
             }
        }
    }
}

// Collect inventory item.
// So far, just keys are used in inventory
void Game::collectInventoryItem(item *itm)
{
    player_inventory.push_back(item());
    int index = (int)player_inventory.size() - 1;
    player_inventory[index].setItemFields(item_data[(int)itm->getItemType()],point(0.0,0.0),itm->entid());
    itm->setMarkForDeletion();
    sfx.playSoundEffect(sfx.getSoundEffect((int)SOUNDTYPE_COLLECT));
}

// collect health powerup, max health powerup, or experience level powerup
void Game::collectHealthPowerup(item *pwr)
{
    initial_health_fields health_stats = pwr->getItemSuperFields()->health_stats;
    if (health_stats.hp_increment > 0)
    {
        if (getPlayerMob()->getHP() < getPlayerMob()->getMobSuperFields()->max_hp)
        {
            pwr->setMarkForDeletion();
            getPlayerMob()->incHP(health_stats.hp_increment);
            sfx.playSoundEffect(sfx.getSoundEffect((int)SOUNDTYPE_HEALTH));
        }
    }
    else if (health_stats.maxhp_increment > 0)
    {
        pwr->setMarkForDeletion();
        getPlayerMob()->setMaxHP(getPlayerMob()->getMobSuperFields()->max_hp + health_stats.maxhp_increment);
        sfx.playSoundEffect(sfx.getSoundEffect((int)SOUNDTYPE_HEALTH));
    }
    else if (health_stats.explev_increment > 0)
    {
        if (exp_level < MAX_PLAYER_EXP_LEVEL)
        {
            int levelup_index = exp_level+health_stats.explev_increment-2;

            if (levelup_index > MAX_PLAYER_EXP_LEVEL - 2)
                levelup_index = MAX_PLAYER_EXP_LEVEL - 2;

            pwr->setMarkForDeletion();
            exp_points = exp_levelup_values[levelup_index];
            //sfx.playSoundEffect(sfx.getSoundEffect((int)SOUNDTYPE_HEALTH));
        }
    }
}

// Collect goblet, gold bars, etc...
void Game::collectValuablePowerup(item *pwr)
{
    score += (pwr->getItemSuperFields()->value * current_level);
    pwr->setMarkForDeletion();
    sfx.playSoundEffect(sfx.getSoundEffect((int)SOUNDTYPE_COIN));
}

// Check to see if NPC can be spawned.
// Conditions:
// 1) 1 in 800/1500 chance every frame
// 2) number of NPCs is less than maximum total that can be on the level
void Game::processLevelSpawnEvent()
{
    if (!getPlayerMob()->isDead() && roll(800) && (int)npcs.size() < max_npc_vector_size[std::min(10,current_level)-1])
    {
        genOneNPC(getRandNPC(),true);
    }
}

// Print pause string to middle of screen
void Game::executeGamePauseActions()
{
    gfx.addBitmapString(color_white,"Game Paused - press p to resume",point(WINDOW_WIDTH/2.0 - 31.0*FONT_CHAR_WIDTH/2.0, WINDOW_HEIGHT/2.0 - FONT_CHAR_HEIGHT/2.0));
    gfx.updateScreen();
}

void Game::updatePlayerTimers() {
    if (playerSlowTimer > 0) {
        playerSlowTimer -= 1;
    }
    else {
        getPlayerMob()->setMaxVelocity(mob_data[(int)MOB_PLAYER].idef.max_velocity);
    }
}

// Everything is done here, until the user quits
void Game::primaryGameLoop()
{
    while (!quit_flag)
    {
        // used for stable framerate
        frame_start_timer = SDL_GetTicks();
        // get input
        processActions();

        if (quit_flag)
            return;

        // Halt all active dynamic game functions if
        // game is paused
        if (!game_paused)
        {
            // Do most of the game work
            checkCollectPowerup();
            applyAI();
            applyPhysics();
            updatePlayerTimers();
            updateItemTimers();
            updateDoorTimers();
            updateAnimations();
            processParticleEffects();
            checkLevelUpEvent();
            //processLevelSpawnEvent();
            checkTargetIndicatorReset();
            renderTextures();
        }
        else
            executeGamePauseActions();

        // call SDL_Delay
        delayGame();
    }
}

void Game::checkTargetIndicatorReset()
{
    if (npcTargetFocusID >= 1)
    if (sqrt(distanceSquared(getPlayerMob()->getCenter(),getMobFromEntityID(npcTargetFocusID)->getCenter())) > 450.0 &&
        !isBoss(getMobFromEntityID(npcTargetFocusID)->getMobType()))
    {
        npcTargetFocusID = -1;
    }
}

// See if a particle is colliding with a mob and check for possible damage
void Game::processParticleEffects()
{
    for (int i = 0; i < (int)particles.size(); ++i)
    {
        for (int j = 0; j < (int)npcs.size(); ++j)
        {
            if (collisionWithEntity(particles[i].getCenter(), particles[i].getDim(), &(npcs[j])) &&
                particles[i].getPhysicsStatus() == PHYSTYPE_FULL)
            {
                checkDamageMobFromParticle(&particles[i],&npcs[j]);
            }
        }
        if (collisionWithEntity(particles[i].getCenter(), particles[i].getDim(), getPlayerMob()) &&
           (particles[i].getPhysicsStatus() == PHYSTYPE_FULL || particles[i].getParticleType() == PARTICLE_ROCKET))
        {
            checkDamageMobFromParticle(&particles[i],getPlayerMob());
        }
    }
}

particle_type getplTypeFromittype(item_type it_type) {
    switch (it_type) {
            case(ITEMTYPE_ROCKETLAUNCHER):
                 return PARTICLE_ROCKET;
            case(ITEMTYPE_FLAMETHROWER):
                 return PARTICLE_FLAMESHOT;
            case(ITEMTYPE_CANNON):
                 return PARTICLE_CANNONBALL;
            default:
                 break;
    }

    return PARTICLE_LASER;
}

point getParticleStartVelocity(particle_type pl_type, bool flipped) {
    point vel = particle_data[(int)pl_type].max_velocity;

    switch (pl_type) {
            case(PARTICLE_LASER):
            case(PARTICLE_ROCKET):
                 vel = flipped ? multPoints(vel,point(-1.0,0.0)) : vel;
                 break;
            case(PARTICLE_FLAMESHOT):
                 vel = flipped ? multPoints(vel,point(-1.0,1.0)) : vel;
                 break;
            case(PARTICLE_CANNONBALL):
                 vel = flipped ? multPoints(vel, point(-0.65,-0.45)) : multPoints(vel, point(0.65,-0.45));
                 break;
            default:
                 break;
    }

    return vel;
}

point getParticleInsertLoc(item *weapon, particle_type pl_type, bool flipped) 
{
    point min_loc = weapon->getLoc();
    point max_loc = weapon->getMaxLoc();

    point loc;

    switch (pl_type) {
            case(PARTICLE_LASER):
                 loc = flipped ? min_loc : point(max_loc.x()-14.0, min_loc.y());
                 break;
            case(PARTICLE_ROCKET):
                 loc = flipped ? addPoints(min_loc, point(0.0,4.0)) : point(max_loc.x()-14.0, min_loc.y()+4.0);
                 break;
            case(PARTICLE_CANNONBALL):
                 loc = flipped ? addPoints(min_loc, point(20.0,6.0)) : point(max_loc.x()-30.0, min_loc.y()+6.0);
                 break;
            case(PARTICLE_FLAMESHOT):
                 loc = flipped ? addPoints(min_loc, point(-40.0,-3.0)) : point(max_loc.x()-10.0, min_loc.y()-3.0);
                 break;
            default:
                 break;
    }

    return loc;
}

void Game::mobFireParticle(mob *mb, item *weapon) {
    particles.push_back(particle());

    initial_item_super_fields * i_fields = weapon->getItemSuperFields();

    int index = (int)particles.size() - 1;

    int hit_decrement = i_fields->wep_stats.hp_decrement;

    double knock_back = i_fields->wep_stats.knock_back;

    item_type it_type = i_fields->i_type;

    particle_type pl_type = getplTypeFromittype(it_type);

    bool flipped = (mb->getXOrientation() != SDL_FLIP_NONE);

    point ins_loc = getParticleInsertLoc(weapon, pl_type, flipped);

    particles.back().setParticleFields(particle_data[(int)pl_type], ins_loc, index, mb->entid(), hit_decrement, pl_type, knock_back, it_type);

    setParticleFlags(pl_type, flipped);

    applyMobFireKickBack(mb, it_type);

    if (weapon->getItemType() == ITEMTYPE_ROCKETLAUNCHER) {
        weapon->setAnimationStatus(true);
        weapon->setCurrentFrame(0);
    }
}

// Special power attack. Burst lots of fire in both directions // (only one NPC does this)
void Game::mobTripleFlameAttack(mob *mb) {
    for (int n = 0; n < 2; ++n)
    for (int i = 0; i < 3; ++i) {
        particles.push_back(particle());
        int index = (int)particles.size() - 1;
        point loc = addPoints(mb->getCenter(),point(-50.0*n,8.0*i));
        particles.back().setParticleFields(particle_data[(int)PARTICLE_FLAMESHOT],loc,index,mb->entid(),1,PARTICLE_FLAMESHOT,0.0,ITEMTYPE_NONE);
        setParticleFlags(PARTICLE_FLAMESHOT, (n == 1));
    }
}

// Set flags related to active flame particles
void Game::setParticleFlags(particle_type pl_type, bool flipped) {
    SDL_RendererFlip orientation = flipped ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    particles.back().setXOrientation(pl_type != PARTICLE_FLAMESHOT ? orientation : (SDL_RendererFlip)((unsigned short)orientation ^ 1));
    particles.back().setVelocity(getParticleStartVelocity(pl_type, flipped));
    particles.back().setDestructableUponCollision(pl_type != PARTICLE_CANNONBALL);
    particles.back().setAnimationStatus(pl_type == PARTICLE_FLAMESHOT);
}

// First function called when a ballistic weapon is fired
// Particles are not used for this, but rather a line drawer
// that traces its path horizontally, checking for something to hit.
void Game::mobFireWeaponProjectile(mob *mb, item *weapon) {
    point projectile_drawer;

    int x_offset = ((mb->getXOrientation() == SDL_FLIP_NONE) ? 10 : -10);
    int y_offset = 0;
    int num_projectiles = 1;

    if (weapon->getItemType() == ITEMTYPE_CHAINGUN)
        y_offset = randZero(2)*4 - 4;

    if (weapon->getItemType() == ITEMTYPE_SHOTGUN) {
        num_projectiles = 5;
        y_offset = -4;
    }

    for (int i = 0; i < num_projectiles; ++i) {
        if (mb->getXOrientation() == SDL_FLIP_NONE)
            projectile_drawer.set(weapon->getMaxLoc().x(),weapon->getCenter().y()+y_offset);
        else
            projectile_drawer.set(weapon->getLoc().x(),weapon->getCenter().y()+y_offset);

        while (!projectileDrawerHitWall(projectile_drawer)) {
            if (projectileDrawerCollidingWithMob(mb,projectile_drawer))
                break;
            projectile_drawer.setx(projectile_drawer.x() + x_offset);
        }

        y_offset += 2;
    }

    applyMobFireKickBack(mb, weapon->getItemType());

    weapon->setAnimationStatus(true);
    weapon->setCurrentFrame(0);
}

// Apply knockback related to the death of a mob
void Game::applyMobDeathKnockback(mob *mb, item_type weapon) {
    if (weapon != ITEMTYPE_FLAMETHROWER) {
        mb->setMaxVelocity(multPoints(mb->getMaxVelocity(), point(25.0, 2.0)));
        if (std::abs(mb->getVelocity().y()) < 0.1)
            mb->setVelocity(point(mb->getVelocity().x(), 0.0));
        mb->setVelocity(point(mb->getVelocity().x() * 3.0, -1.5 * std::abs(mb->getVelocity().y())));
    }
}

void Game::setDeathType(mob* target, item_type weapon, int hp_decrement) {
    switch (weapon) {
        case(ITEMTYPE_NONE):
        case(ITEMTYPE_PISTOL):
        case(ITEMTYPE_REVOLVER):
        case(ITEMTYPE_CHAINGUN):
        case(ITEMTYPE_SHOTGUN):
            if ((target->getMobType() == MOB_FIGHTER || target->getMobType() == MOB_SOLDIER || target->getMobType() == MOB_CAPTAIN || target->getMobType() == MOB_GUARD) && ((int)getItemCarriedByMob(0)->getWeaponModifierType() >= WEAPONMODIFIER_DAMAGING))
                target->setMobDeathType(MOBDEATHTYPE_BURST);
            else
                target->setMobDeathType(MOBDEATHTYPE_KILLED);
            break;
        case(ITEMTYPE_FLAMETHROWER):
            target->setMobDeathType(MOBDEATHTYPE_INCINERATED);
            break;
        case(ITEMTYPE_CANNON):
            if (target->getMobType() == MOB_FIGHTER || target->getMobType() == MOB_SOLDIER || target->getMobType() == MOB_CAPTAIN || target->getMobType() == MOB_GUARD)
                target->setMobDeathType(MOBDEATHTYPE_BURST);
            else
                target->setMobDeathType(MOBDEATHTYPE_KILLED);
            break;
        case(ITEMTYPE_ROCKETLAUNCHER):
            if (target->getMobType() == MOB_FIGHTER || target->getMobType() == MOB_SOLDIER || target->getMobType() == MOB_CAPTAIN || target->getMobType() == MOB_GUARD || target->getMobType() == MOB_BIGGUARD || target->getMobType() == MOB_AGENT)
                target->setMobDeathType(MOBDEATHTYPE_EXPLODED);
            else
                target->setMobDeathType(MOBDEATHTYPE_KILLED);
            break;
        case(ITEMTYPE_LASERGUN):
            if (target->getMobType() == MOB_FIGHTER || target->getMobType() == MOB_SOLDIER || target->getMobType() == MOB_CAPTAIN || target->getMobType() == MOB_GUARD || target->getMobType() == MOB_BIGGUARD || target->getMobType() == MOB_AGENT)
                target->setMobDeathType(MOBDEATHTYPE_ELECTROEXPLODE);
            else
                target->setMobDeathType(MOBDEATHTYPE_KILLED);
            break;
        default:
            break;
    }
}

int getAdjustedMobIndex(mob* mb) {
    return (int)mb->getMobType();
}

prop_type getGibPieceTypeFromMob(mob_type mtype) {
    if (mtype != MOB_GUARD && mtype != MOB_ELITEGUARD && mtype != MOB_HAZMATGUY && mtype != MOB_HAZMATGOD) {
        return PROP_GIBPIECE1;
    }
    return PROP_GIBPIECE2;
}

int getGibTextureRow(mob* mb) {
    if (mb->getMobType() == MOB_HAZMATGUY || mb->getMobType() == MOB_HAZMATGOD)
    {
        return 2;
    }
    return (int)mb->getMobModifierType();
}

// decrement mob's health
void Game::damageMob(mob *target, int hp_dec, int causer_id, item_type weapon) {
    target->incHP(-1 * hp_dec);

    target->setAggroStatus(true);

    if (target->getHP() <= 0) {
        target->togglePhysics(PHYSTYPE_FULL);

        getOffLadder(target,false);
        target->setDeathStatus(true);
        target->setXDeltaNormal(0.0);

        if (target->getItemCarryType() != ITEMTYPE_NONE)
            checkDropEvent(target,getItemCarriedByMob(target->entid()));

        applyMobDeathKnockback(target, weapon);
        target->setCurrentFrame(2);
        target->setFrameIncrement(1);

        setDeathType(target, weapon, hp_dec);
        switch (target->getMobDeathType()) {
        case(MOBDEATHTYPE_INCINERATED):
            squirtDynamicGibParticles(target, mob_death_gib_count[getAdjustedMobIndex(target)].large_ash_incinerated_gibs, PROP_LARGEASH);
            squirtStaticGibParticles(target, mob_death_gib_count[getAdjustedMobIndex(target)].mini_ash_incinerated_gibs, PROP_MINIASH, 0, false);
            squirtStaticGibParticles(target, mob_death_gib_count[getAdjustedMobIndex(target)].mini_ash2_incinerated_gibs, PROP_MINIASH2, 0, false);
            if (target->getMobType() != MOB_PLAYER) {
                target->setMarkForDeletion();
            }


            break;
        case(MOBDEATHTYPE_ELECTROEXPLODE):
            squirtDynamicGibParticles(target, mob_death_gib_count[getAdjustedMobIndex(target)].large_ash_incinerated_gibs*2, PROP_LARGEASH);
            squirtStaticGibParticles(target, mob_death_gib_count[getAdjustedMobIndex(target)].mini_ash2_incinerated_gibs*2, PROP_MINIASH2, 0, false);
            squirtDynamicGibParticles(target, mob_death_gib_count[getAdjustedMobIndex(target)].large_blood_explode_gibs, PROP_LARGEBLOOD);
            squirtStaticGibParticles(target, mob_death_gib_count[getAdjustedMobIndex(target)].mini_blood_explode_gibs/2, PROP_MINIBLOOD, 0, true);
            squirtStaticGibParticles(target, mob_death_gib_count[getAdjustedMobIndex(target)].mini_ash_explode_gibs*2, PROP_MINIASH, 0, true);
            squirtStaticGibParticles(target, mob_death_gib_count[getAdjustedMobIndex(target)].explode_piece_gibs - 1, getGibPieceTypeFromMob(target->getMobType()), getGibTextureRow(target), true);
            if (target->getMobType() != MOB_PLAYER) {
                target->setMarkForDeletion();
            }
            break;
        case(MOBDEATHTYPE_EXPLODED):
            squirtStaticGibParticles(target, mob_death_gib_count[getAdjustedMobIndex(target)].mini_ash_explode_gibs, PROP_MINIASH, 0, true);
        case(MOBDEATHTYPE_BURST):
            squirtDynamicGibParticles(target, mob_death_gib_count[getAdjustedMobIndex(target)].large_blood_explode_gibs, PROP_LARGEBLOOD);
            squirtStaticGibParticles(target, mob_death_gib_count[getAdjustedMobIndex(target)].mini_blood_explode_gibs, PROP_MINIBLOOD, 0, true);
            squirtStaticGibParticles(target, mob_death_gib_count[getAdjustedMobIndex(target)].explode_piece_gibs, getGibPieceTypeFromMob(target->getMobType()), getGibTextureRow(target), true);
            if (target->getMobType() != MOB_PLAYER) {
                target->setMarkForDeletion();
            }
            break;
        default:
            squirtDynamicGibParticles(target, mob_death_gib_count[getAdjustedMobIndex(target)].large_blood_killed_gibs, PROP_LARGEBLOOD);
            squirtStaticGibParticles(target, mob_death_gib_count[getAdjustedMobIndex(target)].mini_blood_killed_gibs, PROP_MINIBLOOD, 0, false);
            break;
        }

        if (target->getMobType() != MOB_PLAYER && causer_id == getPlayerMob()->entid() && !getPlayerMob()->isDead())
            addPlayerExperience(target,weapon);

        playDeathSound(target->getMobType());

        if (target->dropsKey())
            dropKey(target);

        if (npcTargetFocusID == target->entid())
            npcTargetFocusID = -1;
    }
    else {
        target->setCurrentFrame(1);
        target->setFrameIncrement(-1);
        if (target->getMobType() != MOB_PLAYER)
            npcTargetFocusID = target->entid();
    }

    target->setAnimationStatus(true);
}

void Game::addPlayerExperience(mob *source, item_type wep_assoc)
{
    uint added_exp = (uint)source->getMobSuperFields()->base_exp_worth;
    exp_points += added_exp;
    if ((int)wep_assoc >= (int)ITEMTYPE_PISTOL && (int)wep_assoc <= (int)ITEMTYPE_LASERGUN)
        weapon_exp[(int)wep_assoc - 1] += added_exp;
}

void Game::dropKey(mob *mb)
{
    addPowerup((item_type)ITEMTYPE_KEYCARD1,mb->getCenter());
}

// Check damage applied to target from a particle (anything non-ballistic)
// Apply knockback if needed
void Game::checkDamageMobFromParticle(particle *prtcl, mob *target)
{
    int hp_dec = 0;
    if (prtcl->getParticleType() == PARTICLE_FLAMESHOT ||
        prtcl->getParticleType() == PARTICLE_EXPLOSION ||
        prtcl->getParticleType() == PARTICLE_ROCKET ||
        prtcl->getParticleType() == PARTICLE_LASER ||
       (prtcl->getParticleType() == PARTICLE_CANNONBALL && prtcl->getVelocity().y() != 0.0))
    {
        if (!target->isDead())
        if (((prtcl->getCreatorID() != target->entid()) && (prtcl->getCreatorID() == 0 || target->entid() == 0)) || prtcl->getParticleType() == PARTICLE_EXPLOSION)
        {
            if (!(prtcl->getParticleType() == PARTICLE_FLAMESHOT && target->getMobSuperFields()->p_type == POWERTYPE_FIREIMMUNE))
            {
                applyMobParticleKnockback(target,prtcl);
                hp_dec = prtcl->getDamageValue();
                if (prtcl->getCreatorID() == 0)
                    addBonusDamage(hp_dec,prtcl->getWeaponAssociated());
                damageMob(target,hp_dec,prtcl->getCreatorID(),prtcl->getWeaponAssociated());
                if (prtcl->getParticleType() == PARTICLE_ROCKET) {
                    squirtStaticGibParticles(target, 5, PROP_MINIBLOOD, 0, false);
                }
                else if (prtcl->getParticleType() == PARTICLE_FLAMESHOT) {
                    target->setBurningCounter(target->getBurningCounter() + 1);
                    if (target->getBurningCounter() >= 6) {
                        target->setBurningCounter(0);
                        squirtStaticGibParticles(target, 1, PROP_MINIASH, 0,false );
                    }
                }
            }

            if (prtcl->getParticleType() == PARTICLE_ROCKET)
                prtcl->setMarkForDeletion();
        }
    }
}

// A ballistic projectile hit a mob, so apply the relevant knockback to the target.
void Game::applyMobProjectileKnockback(mob *target, mob *shooter)
{
    double knock_back = getItemCarriedByMob(shooter->entid())->getItemSuperFields()->wep_stats.knock_back;

    if (time_stopped)
        knock_back /= 10.0;

    if (shooter->getXOrientation() == SDL_FLIP_NONE)
        target->incVelocity(point(knock_back,0.0));
    else
        target->incVelocity(point(-knock_back,0.0));
}

// Either a
// 1) cannonball
// 2) rocket
// 3) laser blast
// hit a mob, so apply the relevant knockback to the target.
void Game::applyMobParticleKnockback(mob *target, particle *prtcl)
{
    double knock_back = prtcl->getKnockBack();

    if (time_stopped)
        knock_back /= 10.0;

    if (prtcl->getCenter().x() <= target->getCenter().x())
        target->incVelocity(point(knock_back,0.0));
    else
        target->incVelocity(point(-knock_back,0.0));
}

// Check
// 1) how much damage shooter did to target
// 2) how much knockback to apply to target
// and then add the blood
bool Game::checkDamageMobFromProjectile(mob *shooter, mob *target)
{
    if (!target->isDead())
    if ((shooter->entid() != target->entid()) && (shooter->entid() == 0 || target->entid() == 0))
    {
        int hp_dec = getItemCarriedByMob(shooter->entid())->getItemSuperFields()->wep_stats.hp_decrement;

        item_type wep_assoc = shooter->getItemCarryType();

        if (shooter->getMobType() == MOB_PLAYER)
            addBonusDamage(hp_dec,wep_assoc);

        applyMobProjectileKnockback(target,shooter);

        damageMob(target, hp_dec, shooter->entid(), wep_assoc);

        squirtDynamicGibParticles(target,getItemCarriedByMob(shooter->entid())->getItemSuperFields()->wep_stats.large_hit_gibs, PROP_LARGEBLOOD);
        squirtStaticGibParticles(target,getItemCarriedByMob(shooter->entid())->getItemSuperFields()->wep_stats.mini_hit_gibs, PROP_MINIBLOOD, 0, false);

        return true;
    }

    return false;
}

void Game::addBonusDamage(int &hp_dec, item_type wep_assoc)
{
    hp_dec += (weapon_exp_bonus[(int)wep_assoc-1] * weapon_bonus_level_damage_multipliers[(int)wep_assoc - 1]);
}

// Did a ballistic weapon bullet hit a mob?
bool Game::projectileDrawerCollidingWithMob(mob *shooter, point projectile_drawer)
{
    for (int i = 0; i < (int)npcs.size(); ++i)
    {
        if (collisionWithEntity(projectile_drawer, point(1,1), &npcs[i]))
        {
            if (checkDamageMobFromProjectile(shooter,&npcs[i]))
               return true;
        }
    }
    if (collisionWithEntity(projectile_drawer, point(1,1), getPlayerMob()))
    {
        if (checkDamageMobFromProjectile(shooter,getPlayerMob()))
            return true;
    }
    return false;
}

// Add large blood particle props when enemy damaged by weapon that allows for these props to be created
void Game::squirtDynamicGibParticles(mob *mb, int num_particles, prop_type ptype)
{
    int index = 0;
    point loc;
    for (int i = 0; i < num_particles; ++i)
    {
        props.push_back(dynamic_entity());
        index = (int)props.size() - 1;
        loc = point((double)randZero((int)(mb->getDim().x() - 22.0)) + mb->getLoc().x(),(double)randZero((int)(mb->getDim().y() - 20.0)) + mb->getLoc().y());
        props[index].setDynamicEntityFields(prop_data[(int)ptype],loc,index);
        props[index].setAnimationSpeed(props[index].getAnimationSpeed() + randZero(5));
        props[index].setXOrientation(roll(2) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);
        props[index].setXDeltaNormal(mb->getXDeltaNormal());
        props[index].setResetFrameFlag(false);
        props[index].setAnimationStatus(true);
        props[index].setVelocity(mb->getVelocity());
        if (!mb->isDead())
            props[index].setXDeltaNormal(0.0);
    }
}

// Add miniature blood particle props when enemy damaged by weapon that allows for these props to be created
void Game::squirtStaticGibParticles(mob *mb, int num_particles, prop_type ptype, int texture_row, bool exploded)
{
    int index = 0;
    double delta_normal = 0.0;
    for (int i = 0; i < num_particles; ++i) {
        props.push_back(dynamic_entity());
        index = (int)props.size() - 1;
        delta_normal = -1.0 + (double)(2 * randZero(1));
        props.back().setDynamicEntityFields(prop_data[(int)ptype],point(mb->getCenter().x()-2.0,mb->getCenter().y()-2.0),index);
        props.back().setXOrientation(roll(2) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);
        props.back().setXDeltaNormal(delta_normal);
        props.back().setDestructableUponCollision(true);
        props.back().setVelocity(point(delta_normal*(double)randInt(1,20)/2.0,-3.6-(double)randZero(19)/5.0));
        if (exploded) {
            props.back().setVelocity(multPoints(props.back().getVelocity(), point(1.2, 1.0)));
        }

        props.back().setXDeltaNormal(0.0);
        props.back().setTextureRow(texture_row);
    }
}

// Did a ballistic weapon bullet hit a wall or door?
bool Game::projectileDrawerHitWall(point projectile_drawer) {
    if (!inRange(projectile_drawer,point(0.0,0.0),point(MAP_WIDTH,MAP_HEIGHT)))
        return true;

    return isCollidingWithStaticBlocker(projectile_drawer, point(1,1));
}

// NPC movement AI
void Game::npcMoveEvent(mob *mb)
{
    bool move_based_on_player = (sqrt(distanceSquared(getPlayerMob()->getCenter(),mb->getCenter())) < mb->getMobSuperFields()->field_of_view);

    if (move_based_on_player || mb->isAggroed())
        mb->setMoveStatus(MOVETYPE_ROVING);
    else
        mb->setMoveStatus(MOVETYPE_STANDINGSTILL);

    switch(mb->getMoveStatus())
    {
        case(MOVETYPE_STANDINGSTILL):
             mb->setXDeltaNormal(0.0);
             break;
        case(MOVETYPE_ROVING):
             if (std::abs(getPlayerMob()->getCenter().y() - mb->getCenter().y()) <= getPlayerMob()->getDim().y() )
             {
                 if (getPlayerMob()->getCenter().x() - 80.0 > mb->getCenter().x())
                     mb->setXDeltaNormal(1.0);
                 else if (getPlayerMob()->getCenter().x() + 80.0 < mb->getCenter().x())
                     mb->setXDeltaNormal(-1.0);
                 else if (roll(10) && mb->getMobType() != MOB_SHADOW && mb->getMobType() != MOB_SHADOWKING)
                     mb->setXDeltaNormal(-1.0*mb->getXDeltaNormal());
             }
             else
             {
                 if (mb->getMobType() != MOB_FIGHTER && mb->getMobType() != MOB_SOLDIER && mb->getMobType() != MOB_CAPTAIN) {
                     if (roll(200) || mb->getXDeltaNormal() == 0.0)
                         mb->setXDeltaNormal((double)(randZero(2) - 1));
                 }
                 else {
                     if (roll(500) || mb->getXDeltaNormal() == 0.0)
                         mb->setXDeltaNormal((double)(randZero(2) - 1));
                 }
             }
             if (npcJumpCondition(mb))
             {
                 mb->setVelocity(point(mb->getVelocity().x(), -1.0 * mb->getMobSuperFields()->jump_strength));
             }
             break;
        default:
             break;
    }

    if (mb->getXDeltaNormal() > 0.0)
        mb->setXOrientation(SDL_FLIP_NONE);
    if (mb->getXDeltaNormal() < 0.0 && mb->getMobSuperFields()->sprite_flips)
        mb->setXOrientation(SDL_FLIP_HORIZONTAL);
}

// Should the NPC jump?
bool Game::npcJumpCondition(mob *mb)
{
    if (!mb->getLadderStatus())
    if (!mb->getVerticalMotionFlag() && mb->getVelocity().y() == 0.0)
    {
        if (getPlayerMob()->getCenter().y() < mb->getCenter().y() - 25.0 && roll(mb->getMobSuperFields()->jump_freq_roller/2))
            return true;
        if (getPlayerMob()->getCenter().y() >= mb->getCenter().y() - 25.0 && roll(mb->getMobSuperFields()->jump_freq_roller))
            return true;
    }
    return false;

}

// If the NPC can use weapons, have it fire a weapon, or possibly pickup a weapon
void Game::npcWeaponEvent(mob *mb)
{
    if (mb->getMobSuperFields()->uses_weapons)
    {
        if (mb->getItemCarryType() == ITEMTYPE_NONE)
        {
            for (int j = 0; j < (int)items.size(); ++j)
            {
                if (collisionWithEntity(mb->getCenter(),mb->getDim(),&items[j]))
                {
                    if (checkPickupEvent(mb,&items[j]))
                        break;
                }
            }
        }
        else
        {
            if (mobFacingTarget(mb,getPlayerMob()))
                if (npcAttackCondition(mb, getPlayerMob())) {
                    weaponmodifier_type weapon_modifier = getItemCarriedByMob(mb->entid())->getWeaponModifierType();
                    if (rollPerc(mb->getMobSuperFields()->shoot_frequency * ((weapon_modifier == WEAPONMODIFIER_FAST || weapon_modifier == WEAPONMODIFIER_FASTDAMAGING) ? 2 : 1)))
                    {
                        mobFireWeapon(mb);
                    }
                }
        }
    }
}

// Should NPC detect the player?
bool npcDetectCondition(mob *mb, mob *pmb) {
    if (sqrt(distanceSquared(pmb->getCenter(),mb->getCenter())) < mb->getMobSuperFields()->field_of_view)
        return true;
    return false;
}

// Should NPC attack the player?
bool npcAttackCondition(mob *mb, mob *pmb) {
    if (!pmb->isDead())
    if (npcDetectCondition(mb, pmb) || mb->isAggroed())
        return true;

    return false;
}

// NPC special power attack event
// (so far just for one enemy)
void Game::npcPowerEvent(mob *mb)
{
    power_type p_type = mb->getMobSuperFields()->p_type;
    if (p_type != POWERTYPE_NONE)
    {
        switch(p_type)
        {
        case(POWERTYPE_TRIPLEFLAMER):
            if (!npcDetectCondition(mb,getPlayerMob()))
                mb->setAggroStatus(false);
            if (npcAttackCondition(mb,getPlayerMob()))
                mobTripleFlameAttack(mb);
            break;
        case(POWERTYPE_TELEPORT):
            if (roll(50))
                mobTeleport(mb);
            break;
        case(POWERTYPE_THROUGHWALLS):
            if (mb->getCenter().y() > getPlayerMob()->getCenter().y() + 100 || mb->getLoc().y() + mb->getDim().y() > MAP_HEIGHT)
                mb->setVelocity(point(mb->getVelocity().x(),-2.0*fabs(mb->getVelocity().x())));
            else if (mb->getCenter().y() < getPlayerMob()->getCenter().y() - 100 || mb->getLoc().y() < 0.0)
                mb->setVelocity(point(mb->getVelocity().x(),2.0*fabs(mb->getVelocity().x())));
            break;
        default:
            break;
        }
    }
}

// If NPC is colliding with player and can melee the player,
// have the NPC do collision damage to the player
void Game::npcMeleeEvent(mob *mb) {
    if (rollPerc(mb->getMobSuperFields()->melee_frequency))
    if (!getPlayerMob()->isDead())
    if (collisionWithEntity(mb->getCenter(),mb->getDim(),getPlayerMob())) {
        damageMob(getPlayerMob(),mb->getMobSuperFields()->melee_damage,mb->entid(), ITEMTYPE_NONE);
        // NPC disarms player if it has the special ability to do so.
        if (mb->getMobSuperFields()->p_type == POWERTYPE_DISARM)
        {
            if (getPlayerMob()->getItemCarryType() != ITEMTYPE_NONE)
                checkDropEvent(getPlayerMob(), getItemCarriedByMob(getPlayerMob()->entid()));
        }
        if (mb->getMobSuperFields()->p_type == POWERTYPE_SLOW)
        {
            playerSlowTimer = 100;
            getPlayerMob()->setMaxVelocity(multPoints(mob_data[(int)MOB_PLAYER].idef.max_velocity, point(0.5, 1.0)));
        }
    }
}

// apply AI for all NPCs
void Game::applyAI() 
{
    for (auto it = npcs.begin(); it != npcs.end(); ++it)
    {
        if (!it->isDead())
        {
            // move
            npcMoveEvent(&*it);
            // attack
            npcWeaponEvent(&*it);
            // special power
            npcPowerEvent(&*it);
            // hit damage player
            npcMeleeEvent(&*it);
            // ladder event
            npcLadderEvent(&*it);
            // door event
            npcDoorEvent(&*it);
        }
    }
}

// NPC open or close door
void Game::npcDoorEvent(mob *mb) {
     if (rollPerc(mb->getMobSuperFields()->door_use_frequency))
         mobDoorEvent(mb);
}

// change mob's fields pertaining to a ladder event
void Game::mobToggleLadderEvent(mob *mb) {
     for (auto it = ladders.begin(); it != ladders.end(); ++it) {
          if (collisionWithEntity(mb->getCenter(),mb->getDim(),&*it)) {
              mb->setLoc(point(it->getCenter().x() - (mb->getDim().x()/2.0),mb->getLoc().y()));
              mb->setLadderStatus(true);
              mb->setXDeltaNormal(0.0);
              mb->setVelocity(point(0.0,0.0));
              mb->setVerticalMotionFlag(false);
              break;
          }
     }
}

// MOB event related to climbing up or down ladder, or getting off
void Game::mobClimbLadderEvent(mob *mb, bool move_down)
{
    if (mb->getLadderStatus())
        mb->incLoc(point(0.0,((move_down == true) ? 1.0 : -1.0)*3.0));
    bool got_off_ladder = true;
    for (int i = 0; i < (int)ladders.size(); ++i)
    {
        if (collisionWithEntity(mb->getCenter(),mb->getDim(),&ladders[i]))
        {
            got_off_ladder = false;
            break;
        }
    }
    if (move_down || mb->getMobType() != MOB_PLAYER)
    for (int i = 0; i < (int)walls.size(); ++i)
    {
        if (collisionWithEntity(addPoints(mb->getCenter(),point(0.0,1.0)),mb->getDim(),&walls[i]))
        {
            got_off_ladder = true;
            break;
        }
    }
    if (got_off_ladder)
        getOffLadder(mb,false);
}

// NPC event related to climbing, getting off or getting on a ladder
void Game::npcLadderEvent(mob *mb)
{
    if (rollPerc(mb->getMobSuperFields()->ladder_use_frequency))
    {
        if (mb->getLadderStatus() == false)
            mobToggleLadderEvent(mb);
        else
            getOffLadder(mb,true);
    }

    if (mb->getLadderStatus())
        mobClimbLadderEvent(mb,false);
}

bool Game::eitherFromPlayerOrTimeActive(int id)
{
    return (id == getPlayerMob()->entid() || !time_stopped);
}

void Game::settleMobsToGround()
{
    for (int i = 0; i < (int)npcs.size(); ++i)
    {
        if (npcs[i].getMobType() != MOB_SHADOW && npcs[i].getMobType() != MOB_SHADOWKING)
        {
            applyPhysicsForDynamicEntity(&npcs[i]);
            while (npcs[i].getVelocity().y() != 0.0)
            {
                applyPhysicsForDynamicEntity(&npcs[i]);
            }
            npcs[i].setVelocity(point(0.0, 0.0));
        }
    }
}

// apply physics for all dynamic entities
void Game::applyPhysics()
{
    // player physics
    if (!getPlayerMob()->getLadderStatus())
        applyPhysicsForDynamicEntity(getPlayerMob());
    else
        applyCollisions(getPlayerMob());

    // particle physics
    for (int i = 0; i < (int)particles.size(); ++i)
    {
        applyPhysicsForDynamicEntity(&particles[i]);
    }

    // prop physics
    for (int i = 0; i < (int)props.size(); ++i)
    {
        if (!(props[i].getVelocity().y() == 0.0 && !props[i].getVerticalMotionFlag()))
            applyPhysicsForDynamicEntity(&props[i]);
    }

    // npc physics
    for (int i = 0; i < (int)npcs.size(); ++i)
    {
        if (!(isAt(npcs[i].getVelocity(),point(0.0,0.0)) && !npcs[i].getVerticalMotionFlag() && npcs[i].isDead()))
        {
            if (!npcs[i].getLadderStatus())
            {
                if (npcs[i].getMobSuperFields()->p_type != POWERTYPE_THROUGHWALLS)
                    applyPhysicsForDynamicEntity(&npcs[i]);
                else
                    applyPhysicsForShadowEntity(&npcs[i],&npcs[i]);
            }
            else
                applyCollisions(&npcs[i]);
        }
    }

    // equippable item physics
    for (int i = 0; i < (int)items.size(); ++i)
    {
        applyPhysicsForItem(&items[i]);
    }

    // powerup item physics
    for (int i = 0; i < (int)powerups.size(); ++i)
    {
        if (powerups[i].getItemType() == ITEMTYPE_KEYCARD1)
            applyPhysicsForItem(&powerups[i]);
    }

    // check particle deletion
    for (int i = 0; i < (int)particles.size(); ++i)
    {
        if (destroyParticleCondition(&particles[i]))
        {
            if (particles[i].getParticleType() == PARTICLE_CANNONBALL || particles[i].getParticleType() == PARTICLE_ROCKET)
                createExplosion(particles[i].getLoc(),particles[i].getCreatorID(),particles[i].getDamageValue(),particles[i].getKnockBack(),
                               (particles[i].getParticleType() == PARTICLE_CANNONBALL) ? ITEMTYPE_CANNON : ITEMTYPE_ROCKETLAUNCHER);
            particles.erase(particles.begin() + i);
            i--;
        }
    }

    // check npc deletion
    for (int i = 0; i < (int)npcs.size(); ++i)
    {
        if (npcs[i].getMarkForDeletion())
        {
            npcs.erase(npcs.begin() + i);
            i--;
        }
    }

    // check prop deletion
    for (int i = 0; i < (int)props.size(); ++i)
    {
        if (props[i].getMarkForDeletion())
        {
            props.erase(props.begin() + i);
            i--;
        }
    }

    // check powerup deletion (was picked up by player)
    for (int i = 0; i < (int)powerups.size(); ++i)
    {
        if (powerups[i].getMarkForDeletion())
        {
            powerups.erase(powerups.begin() + i);
            i--;
        }
    }
}

// create an explosion (as a result of cannon or rocket)
void Game::createExplosion(point loc, int cid, int dam, double kb, item_type wep_assoc)
{
    particles.push_back(particle());
    particles[(int)particles.size()-1].setParticleFields(particle_data[(int)PARTICLE_EXPLOSION],point(loc.x()-55.0,loc.y()-54.0),(int)particles.size()-1,cid,dam,PARTICLE_EXPLOSION,kb,wep_assoc);
    particles[(int)particles.size()-1].setAnimationStatus(true);
    sfx.playSoundEffect(sfx.getSoundEffect((int)SOUNDTYPE_EXPLOSION));
}

// Check if particle needs to be destroyed
// One of the following must be true:
// 1) The particle was marked for deletion
// 2) The particle's location is off the map
// 3) The particle is a stationary cannonball
// 4) The particle is a flame burst that has come to a stop
// 5) The particle is an explosion or spawn flash whose animation frame is at the end
bool Game::destroyParticleCondition(particle *prtcl)
{
    if(prtcl->getMarkForDeletion())
       return true;

    if(!inRange(prtcl->getLoc(),point(0.0,0.0),point(MAP_WIDTH,MAP_HEIGHT)) && prtcl->getParticleType() != PARTICLE_EXPLOSION)
       return true;

    if(isAt(prtcl->getVelocity(),point(0.0,0.0)) && prtcl->getParticleType() == PARTICLE_CANNONBALL)
       return true;

    if(prtcl->getVelocity().x() == 0.0 && prtcl->getParticleType() == PARTICLE_FLAMESHOT)
       return true;

    if(prtcl->getCurrentFrame() >= prtcl->getNumFrames() - 1 && (prtcl->getParticleType() == PARTICLE_EXPLOSION || prtcl->getParticleType() == PARTICLE_SPAWNFLASH))
       return true;

    return false;
}

// either
// apply physics as a dynamic entity
// ...or don't apply physics if the item is being carried, but just
//    move item along with player
void Game::applyPhysicsForItem(item *it)
{
    if (it->getPossessionMobID() == -1)
        applyPhysicsForDynamicEntity(it);
    else
    {
        SDL_RendererFlip mob_x_orientation = getMobCarryingItem(it->entid())->getXOrientation();
        point mob_center = getMobCarryingItem(it->entid())->getCenter();
        item_type i_type = it->getItemType();
        it->setXOrientation(mob_x_orientation);

        double offset_x = 2.0, offset_y = 2.0;
        if (i_type == ITEMTYPE_LASERGUN)
        {
            offset_x = 8.0;
            if (getMobCarryingItem(it->entid())->getMobType() == MOB_ADVANCEDAGENT)
            {
                offset_x = 12.0;
                offset_y = -9.0;
            }
        }

        if (mob_x_orientation == SDL_FLIP_NONE)
            it->setLoc(point(mob_center.x() - offset_x,mob_center.y() - offset_y));
        else
            it->setLoc(point(mob_center.x() - it->getDim().x() + offset_x,mob_center.y() - offset_y));
    }
}

// Is facer facing target
bool Game::mobFacingTarget(mob *facer, mob *target)
{
    return (((facer->getXOrientation() == SDL_FLIP_NONE && target->getCenter().x() > facer->getCenter().x()) ||
             (facer->getXOrientation() == SDL_FLIP_HORIZONTAL && target->getCenter().x() < facer->getCenter().x())) &&
              std::abs(facer->getCenter().y() - target->getCenter().y()) <= target->getDim().y());
}

// process non-blocking input
void Game::processActions()
{
    evt_handler.processKey();

    if (evt_handler.quitKeyPressed() && !game_paused && !getPlayerMob()->isDead())
    {
        main_menu.setMenuItemString(0,"Resume Game                     ");
        main_menu.setSelectionIndex(0);
        if (main_menu.getNumMenuItems() < 5)
            main_menu.addMenuItem("New Game                        ");
        traverseMainMenu(false);
        return;
    }

    if (evt_handler.quitKeyPressed() && getPlayerMob()->isDead())
    {
        initMainMenu(true);
        main_menu.setSelectionIndex(0);
        traverseMainMenu(true);
        return;
    }

    if (evt_handler.pauseKeyPressed() && !getPlayerMob()->isDead())
    {
        game_paused = !game_paused;
        return;
    }

    if (game_paused)
        return;

    if (evt_handler.useLevelFeaturePressed())
    {
        mobDoorEvent(getPlayerMob());
        endLevelEvent();
    }

    if (getPlayerMob()->isDead())
        return;

    //if (evt_handler.toggleCarryItemKeyPressed())
    //    toggleTime();

    // up, down, left or right arrow
    if (evt_handler.deltaKeyPressed())
    {
        getPlayerMob()->setXDeltaNormal(evt_handler.getDelta().x());
        if (!evt_handler.shiftKeyPressed())
        {
            if (getPlayerMob()->getXDeltaNormal() == 1.0)
                getPlayerMob()->setXOrientation(SDL_FLIP_NONE);
            if (getPlayerMob()->getXDeltaNormal() == -1.0)
                getPlayerMob()->setXOrientation(SDL_FLIP_HORIZONTAL);
        }
    }
    else
        getPlayerMob()->setXDeltaNormal(0.0);

    if (evt_handler.jumpKeyPressed())
    {
        if (!getPlayerMob()->getVerticalMotionFlag() && getPlayerMob()->getVelocity().y() == 0.0)
        {
            getPlayerMob()->setVerticalMotionFlag(true);
            getPlayerMob()->setVelocity(point(getPlayerMob()->getVelocity().x(),-1.0*getPlayerMob()->getMobSuperFields()->jump_strength));
        }
    }

    if (evt_handler.pickupKeyPressed())
    {
        for (int i = 0; i < (int)items.size(); ++i)
        {
            if (collisionWithEntity(player_mob.getCenter(),player_mob.getDim(),&items[i]))
            {
                if (checkPickupEvent(getPlayerMob(),&items[i]))
                    break;
                if (checkDropEvent(getPlayerMob(),&items[i]))
                    break;
            }
        }
    }

    if (evt_handler.fireKeyPressed())
    {
        if (player_mob.getItemCarryID() >= 0)
        {
            mobFireWeapon(getPlayerMob());
        }
    }

    if (evt_handler.onLadderKeyPressed() || evt_handler.downLadderKeyPressed())
    {
        if (getPlayerMob()->getLadderStatus() == false && evt_handler.onLadderKeyPressed())
        {
            mobToggleLadderEvent(getPlayerMob());
        }
        else if (getPlayerMob()->getLadderStatus() == true) {
            getPlayerMob()->setAnimationStatus(true);
            mobClimbLadderEvent(getPlayerMob(), evt_handler.downLadderKeyPressed());
        }
    }
    else if (getPlayerMob()->getLadderStatus() == true) {
        getPlayerMob()->setCurrentFrame(0);
        getPlayerMob()->setAnimationStatus(false);
    }

    if (evt_handler.offLadderKeyPressed())
    {
        getOffLadder(getPlayerMob(),true);
    }
}

// Game restarts
void Game::restartGameEvent()
{
    // If we don't do this, the player's speed will double every new game as a result
    // of the effects of the death knock back function!
    getPlayerMob()->setMaxVelocity(mob_data[(int)MOB_PLAYER].idef.max_velocity);
    getPlayerMob()->setVelocity(point(0.0,0.0));
    getPlayerMob()->setVerticalMotionFlag(true);
    // ...reset other flags...
    getPlayerMob()->setDeathStatus(false);
    getPlayerMob()->setAnimationStatus(false);
    getPlayerMob()->setLadderStatus(false);
    getPlayerMob()->setCurrentFrame(0);
    getPlayerMob()->setItemCarryID(-1);
    getPlayerMob()->setItemCarryType(ITEMTYPE_NONE);

    // erase everything
    cleanupLevelData();
    // clear player's inventory
    std::vector<item>().swap(player_inventory);

    // reset score, experience level, and experience points
    score = 0;
    exp_level = 1;
    exp_points = 0;

    for (int i = 0; i < NUM_WEAPON_TYPES; ++i)
    {
        weapon_exp[i] = 0U;
        weapon_exp_bonus[i] = 0;
    }

    current_level = 1;
    current_level_size.set(level_map_sizes[0].x()*40.0,
                           level_map_sizes[0].y()*40.0);

    // build level 1
    // player's health is reset here
    initLevelObjects();
}

// Player beats the level.
void Game::endLevelEvent()
{
    // Player's corpse cannot walk through exit door
    if (!getPlayerMob()->isDead())
    // Player is at exit door
    if (inRange(getPlayerMob()->getLoc(),exit_loc,addPoints(exit_loc,point(20.0,20.0))))
    {
        // Only entity preserved going to next level is the weapon the player is carrying
        // and its modifier flags (not used if ic_type == ITEMTYPE_NONE)
        item_type ic_type = getPlayerMob()->getItemCarryType();
        weaponmodifier_type wm_type = WEAPONMODIFIER_NONE;

        if (ic_type != ITEMTYPE_NONE)
            wm_type = getItemCarriedByMob(getPlayerMob()->entid())->getWeaponModifierType();

        getPlayerMob()->setItemCarryID(-1);
        getPlayerMob()->setItemCarryType(ITEMTYPE_NONE);

        // erase old level
        cleanupLevelData();

        //if (current_level == 1 && level_increment > 1)
        //    current_level += level_increment - 1;
        //else
        current_level += level_increment;

        current_level_size.set(level_map_sizes[(int)std::min(9,current_level - 1)].x()*40.0,
                               level_map_sizes[(int)std::min(9,current_level - 1)].y()*40.0);
        // build new level
        initLevelObjects();

        // carry over weapon player might be carrying along and set modifier
        if (ic_type != ITEMTYPE_NONE)
        {
            addItem(ic_type,point(0.0,0.0),false);
            items[(int)items.size()-1].setWeaponModifierType(wm_type);
            checkPickupEvent(getPlayerMob(),&items[(int)items.size()-1]);
        }

        sfx.playSoundEffect(sfx.getSoundEffect((int)SOUNDTYPE_NEXTLEVEL));
    }
}

// Mob activates door by pressing a switch
void Game::mobDoorEvent(mob *mb)
{
    int lfid = -1;

    if (mb->isDead())
        return;

    for (int i = 0; i < (int)switches.size(); ++i)
    {
        lfid = switches[i].getLevelFeatureID();
        if (collisionWithEntity(mb->getCenter(),mb->getDim(),&switches[i]))
        {
            // not locked
            if (!getDoorConnectedToSwitch(lfid)->isLocked())
            {
                if (getDoorConnectedToSwitch(lfid)->getDoorState() == DOORSTATE_OPENED)
                {
                    getDoorConnectedToSwitch(lfid)->setDoorState(DOORSTATE_CLOSING);
                }
                if (getDoorConnectedToSwitch(lfid)->getDoorState() == DOORSTATE_CLOSED)
                {
                    if (getDoorConnectedToSwitch(lfid)->getSizeType() == DOORSZE_SMALL)
                        sfx.playSoundEffect(sfx.getSoundEffect((int)SOUNDTYPE_OPENDOOR1));
                    else
                        sfx.playSoundEffect(sfx.getSoundEffect((int)SOUNDTYPE_OPENDOOR2));
                    getDoorConnectedToSwitch(lfid)->setDoorState(DOORSTATE_OPENING);
                }
                break;
            }
            // locked, then check if player has key (NPC's don't do this)
            else
            {
                if (mb->getMobType() == MOB_PLAYER)
                {
                    for (int j = 0; j < (int)player_inventory.size(); ++j)
                    {
                        if (player_inventory[j].getItemType() == ITEMTYPE_KEYCARD1)
                        {
                            sfx.playSoundEffect(sfx.getSoundEffect((int)SOUNDTYPE_UNLOCKDOOR));
                            player_inventory.erase(player_inventory.begin() + j);
                            getDoorConnectedToSwitch(lfid)->setLockStatus(false);
                            break;
                        }
                    }
                }
            }
        }
    }
}

// Fall off, or jump off ladder.
// If jump off -> perform normal jump
// If fall off -> set velocity to (0.0,0.0)
void Game::getOffLadder(mob *mb, bool jump_off)
{
    if (mb->getLadderStatus() == true) {
        mb->setCurrentFrame(0);
        mb->setAnimationStatus(false);
        mb->setLadderStatus(false);
        mb->setVerticalMotionFlag(true);
        mb->setVelocity(point(0.0,0.0));
        // Needed to smoothly get off ladder
        applyCollisions(mb);
        if (jump_off)
            mb->setVelocity(point(mb->getVelocity().x(),-1.0*mb->getMobSuperFields()->jump_strength));
    }
}

// Play weapon's fire sound
void Game::playFireSound(item_type i_type)
{
    switch(i_type) {
    case(ITEMTYPE_PISTOL):
        sfx.playSoundEffect(sfx.getSoundEffect((int)SOUNDTYPE_PISTOL));
        break;
    case(ITEMTYPE_REVOLVER):
        sfx.playSoundEffect(sfx.getSoundEffect((int)SOUNDTYPE_REVOLVER));
        break;
    case(ITEMTYPE_SHOTGUN):
        sfx.playSoundEffect(sfx.getSoundEffect((int)SOUNDTYPE_SHOTGUN));
        break;
    case(ITEMTYPE_CHAINGUN):
        sfx.playSoundEffect(sfx.getSoundEffect((int)SOUNDTYPE_CHAINGUN));
        break;
    case(ITEMTYPE_FLAMETHROWER):
        break;
    case(ITEMTYPE_CANNON):
        sfx.playSoundEffect(sfx.getSoundEffect((int)SOUNDTYPE_CANNON));
        break;
    case(ITEMTYPE_ROCKETLAUNCHER):
        sfx.playSoundEffect(sfx.getSoundEffect((int)SOUNDTYPE_ROCKETLAUNCHER));
        break;
    case(ITEMTYPE_LASERGUN):
        sfx.playSoundEffect(sfx.getSoundEffect((int)SOUNDTYPE_LASERSHOT));
        break;
    default:
        break;
    }
}

// Play NPC's death sound
void Game::playDeathSound(mob_type m_type)
{
    switch(m_type) {
    case(MOB_FIGHTER):
    case(MOB_SOLDIER):
    case(MOB_CAPTAIN):
        sfx.playSoundEffect(sfx.getSoundEffect((int)SOUNDTYPE_FIGHTERDIE));
        break;
    case(MOB_GUARD):
    case(MOB_BIGGUARD):
        sfx.playSoundEffect(sfx.getSoundEffect((int)SOUNDTYPE_GUARDDIE));
        break;
    case(MOB_HAZMATGUY):
        sfx.playSoundEffect(sfx.getSoundEffect((int)SOUNDTYPE_HAZMATDIE));
        break;
    case(MOB_WARRIOR):
    case(MOB_SLAYER):
        sfx.playSoundEffect(sfx.getSoundEffect((int)SOUNDTYPE_WARRIORDIE));
        break;
    case(MOB_ELITEGUARD):
        sfx.playSoundEffect(sfx.getSoundEffect((int)SOUNDTYPE_ELITEGUARDDIE));
        break;
    case(MOB_EXECUTIONER):
        sfx.playSoundEffect(sfx.getSoundEffect((int)SOUNDTYPE_EXECUTIONERDIE));
        break;
    case(MOB_AGENT):
        sfx.playSoundEffect(sfx.getSoundEffect((int)SOUNDTYPE_AGENTDIE));
        break;
    case(MOB_ADVANCEDAGENT):
        sfx.playSoundEffect(sfx.getSoundEffect((int)SOUNDTYPE_ADVANCEDAGENTDIE));
        break;
    case(MOB_SHADOW):
    case(MOB_SHADOWKING):
        sfx.playSoundEffect(sfx.getSoundEffect((int)SOUNDTYPE_SHADOWDIE));
        break;
    case(MOB_GLADIATOR):
    case(MOB_CHAMPION):
        sfx.playSoundEffect(sfx.getSoundEffect((int)SOUNDTYPE_GLADIATORDIE));
        break;
    case(MOB_BEHEMOTH):
    case(MOB_GRANDCHAMPION):
    case(MOB_HAZMATGOD):
        sfx.playSoundEffect(sfx.getSoundEffect((int)SOUNDTYPE_HAZMATGODDIE));
        break;
    case(MOB_PLAYER):
        sfx.playSoundEffect(sfx.getSoundEffect((int)SOUNDTYPE_PLAYERDIE));
        break;
    default:
        break;
    }
}

// Kick back mob based on kick back strength of weapon (kb)
void applyMobFireKickBack(mob *mb, item_type it_type) {
    point kb;

    switch(it_type) {
           case(ITEMTYPE_SHOTGUN):
                kb = point(8.0,-1.0);
                break;
           case(ITEMTYPE_CHAINGUN):
                kb = point(1.0*(double)randInt(1,10)/2.5,0.0);
                break;
           case(ITEMTYPE_CANNON):
                kb = point(17.0, -2.5);
                break;
           case(ITEMTYPE_ROCKETLAUNCHER):
                kb = point(20.0, 0.0);
                break;
           case(ITEMTYPE_LASERGUN):
                kb = point(25.0, 0.0);
                break;
           default:
                return;
    }

    if (mb->getXOrientation() == SDL_FLIP_NONE)
        mb->incVelocity(multPoints(kb,point(-1.0,1.0)));
    else
        mb->incVelocity(kb);
}

// When a mob (player or NPC) chooses to fire a weapon, this function is called.
// The player will fire either a projectile weapon:
// 1) pistol
// 2) revolver
// 3) shotgun
// 4) chaingun
// ...or a weapon that spawns a particle of some kind
// 5) flamethrower
// 6) cannon
// 7) rocket launcher
// 8) laser gun
// Key methods called are
// 1) mobFireWeaponProjectileTravel
// 2) mobFireWeapon<PARTICLENAME>Travel (i.e. ROCKET, LASER, etc...)
// 3) The mob who does the firing can be pushed in the opposite firing direction
//    based on a kickback.
// Some weapons animate, others don't
void Game::mobFireWeapon(mob *mb) {
    item * weapon = getItemCarriedByMob(mb->entid());

    if (weapon != NULL) {
        if (weapon->getUsability()) {
            playFireSound(weapon->getItemType());

            switch(weapon->getItemType()) {
                   case(ITEMTYPE_PISTOL):
                   case(ITEMTYPE_REVOLVER):
                   case(ITEMTYPE_SHOTGUN):
                   case(ITEMTYPE_CHAINGUN):
                        mobFireWeaponProjectile(mb, weapon);
                        break;
                   case(ITEMTYPE_FLAMETHROWER):
                   case(ITEMTYPE_CANNON):
                   case(ITEMTYPE_ROCKETLAUNCHER):
                   case(ITEMTYPE_LASERGUN):
                        mobFireParticle(mb, weapon);
                        break;
                   default:
                        break;
            }

            weapon->setUsability(false);
        }
    }
}

// Only can be called if mob with id "entid" is carrying a weapon.
// Return the item carried by the mob with entity id "entid".
item * Game::getItemCarriedByMob(int entid)
{
    std::vector <item> :: iterator itr = std::find_if(items.begin(), items.end(), [entid](item &itm)
    {
        return entid == itm.getPossessionMobID();
    });
    return &*itr;
}

// Each switch has an associated door based on a "feature id". Given this switch's feature
// id, find the corresponding door in the door vector
door * Game::getDoorConnectedToSwitch(int entid)
{
    std::vector <door> :: iterator itr = std::find_if(doors.begin(), doors.end(), [entid](door &dr)
    {
        return entid == dr.entid();
    });
    return &*itr;
}

mob* Game::getMobFromEntityID(int entid)
{
    std::vector <mob> ::iterator itr = std::find_if(npcs.begin(), npcs.end(), [entid](mob& mb)
    {
        return entid == mb.entid();
    });
    return &*itr;
}

// Given the id of an item, get the mob (player or npc) carrying it.
mob * Game::getMobCarryingItem(int entid)
{
    for (int i = 0; i < (int)npcs.size(); ++i)
    {
        if (npcs[i].getItemCarryID() == entid)
            return &npcs[i];
    }
    return getPlayerMob();
}

// Keep game running at consistent 50 fps.
// Delay less when needed to keep up with the constant framerate.
void Game::delayGame()
{
    if((1000/frames_per_second)>(SDL_GetTicks()-frame_start_timer))
    {
        SDL_Delay((1000/frames_per_second)-(SDL_GetTicks()-frame_start_timer));
    }
}

SDL_Color Game::getLadderColor()
{
    return (!time_stopped || current_level <= BOSS_LEVEL3 ? color_ladder_tint : global_tint[(int)TIMESTOPCT_LADDERS]);
}

SDL_Color Game::getWallColor(int i)
{
    if (walls[i].getColorID() == 0)
        return color_blue;
    if (walls[i].getColorID() == 1)
        return color_lightorange;

    return color_black;
}

// This function needs to be refactored.
// Update camera, and render everything on the screen.
void Game::renderTextures()
{
    gfx.updateCamera(getPlayerMob()->getCenter(),current_level_size);
    gfx.clearScreen();
    for (int i = 0; i < numBackdrops(); ++i)
    {
        gfx.renderSprite(gfx.getTexture(backdrops[i].getTextureIndex()),backdrops[i].getLoc(),backdrops[i].getTextureDim(),backdrops[i].getDim(),0,0,SDL_FLIP_NONE,(i == 0),global_tint[(int)TIMESTOPCT_BACKDROP],2);
    }
    for (int i = 0; i < (int)static_props.size(); ++i)
    {
        gfx.renderSprite(gfx.getTexture(static_props[i].getTextureIndex()),static_props[i].getLoc(),static_props[i].getTextureDim(),static_props[i].getDim(),0,0,SDL_FLIP_NONE,false,global_tint[(int)TIMESTOPCT_STATICPROPS],2);
    }
    for (int i = 0; i < (int)switches.size(); ++i)
    {
        gfx.renderSprite(gfx.getTexture(switches[i].getTextureIndex()),switches[i].getLoc(),switches[i].getTextureDim(),switches[i].getDim(),0,0,SDL_FLIP_NONE,false,global_tint[(int)TIMESTOPCT_SWITCHES],2);
    }
    for (int i = 0; i < (int)doors.size(); ++i)
    {
        gfx.renderSprite(gfx.getTexture(doors[i].getTextureIndex()),doors[i].getLoc(),doors[i].getTextureDim(),doors[i].getDim(),0,0,SDL_FLIP_NONE,false,global_tint[(int)TIMESTOPCT_DOORS],2);
    }
    for (int i = 0; i < (int)numWallBlocks(); ++i)
    {
        gfx.renderSprite(gfx.getTexture(walls[i].getTextureIndex()),walls[i].getLoc(),walls[i].getTextureDim(),walls[i].getDim(),0,0,SDL_FLIP_NONE,false,getWallColor(i),2);
    }
    for (int i = 0; i < (int)ladders.size(); ++i) {
        gfx.renderSprite(gfx.getTexture(ladders[i].getTextureIndex()),ladders[i].getLoc(),ladders[i].getTextureDim(),ladders[i].getDim(),0,0,SDL_FLIP_NONE,false, ladders[i].getLadderTint(), 2);
    }
    for (int i = 0; i < (int)npcs.size(); ++i)
    {
        if (npcs[i].getMobSuperFields()->p_type != POWERTYPE_THROUGHWALLS)
        {
            gfx.renderSprite(gfx.getTexture(npcs[i].getTextureIndex()),npcs[i].getLoc(),npcs[i].getTextureDim(),npcs[i].getDim(),npcs[i].getCurrentFrame(),getMobModRow(&npcs[i]),npcs[i].getXOrientation(),false,getMobTint(&npcs[i]),2);
            // Make sure the NPC's weapon texture is placed directly *in front* of the NPC texture.
            if (npcs[i].getItemCarryType() != ITEMTYPE_NONE)
                gfx.renderSprite(gfx.getTexture(getItemCarriedByMob(npcs[i].entid())->getTextureIndex()),
                                                getItemCarriedByMob(npcs[i].entid())->getLoc(),
                                                getItemCarriedByMob(npcs[i].entid())->getTextureDim(),getItemCarriedByMob(npcs[i].entid())->getDim(),
                                                getItemCarriedByMob(npcs[i].entid())->getCurrentFrame(),getWeaponModRow(getItemCarriedByMob(npcs[i].entid())),
                                                getItemCarriedByMob(npcs[i].entid())->getXOrientation(),false,getMobTint(&npcs[i]), 2);
            printNPCHealthBar(&npcs[i]);
        }
    }
    for (int i = 0; i < (int)props.size(); ++i)
    {
        gfx.renderSprite(gfx.getTexture(props[i].getTextureIndex()),props[i].getLoc(),props[i].getTextureDim(),props[i].getDim(),props[i].getCurrentFrame(),props[i].getTextureRow(),props[i].getXOrientation(),false,global_tint[(int)TIMESTOPCT_PROPS],2);
    }
    gfx.renderSprite(gfx.getTexture(player_mob.getTextureIndex()),player_mob.getLoc(),player_mob.getTextureDim(),player_mob.getDim(),player_mob.getCurrentFrame(),0,player_mob.getXOrientation(),false,global_tint[(int)TIMESTOPCT_PLAYER],2);
    // Make sure the player's weapon texture is placed directly *in front* of the player texture.
    if (getPlayerMob()->getItemCarryType() != ITEMTYPE_NONE)
        gfx.renderSprite(gfx.getTexture(getItemCarriedByMob(getPlayerMob()->entid())->getTextureIndex()),
                                        getItemCarriedByMob(getPlayerMob()->entid())->getLoc(),
                                        getItemCarriedByMob(getPlayerMob()->entid())->getTextureDim(),getItemCarriedByMob(getPlayerMob()->entid())->getDim(),
                                        getItemCarriedByMob(getPlayerMob()->entid())->getCurrentFrame(),getWeaponModRow(getItemCarriedByMob(getPlayerMob()->entid())),
                                        getItemCarriedByMob(getPlayerMob()->entid())->getXOrientation(),false,global_tint[(int)TIMESTOPCT_ITEMS],2);
    for (int i = 0; i < (int)items.size(); ++i)
    {
        // All equipped weapons already were rendered. Render the remaining weapons here.
        if (items[i].getPossessionMobID() == -1)
            gfx.renderSprite(gfx.getTexture(items[i].getTextureIndex()),items[i].getLoc(),items[i].getTextureDim(),items[i].getDim(),items[i].getCurrentFrame(),getWeaponModRow(&items[i]),items[i].getXOrientation(),false,global_tint[(int)TIMESTOPCT_ITEMS],2);
    }
    for (int i = 0; i < (int)powerups.size(); ++i)
    {
        gfx.renderSprite(gfx.getTexture(powerups[i].getTextureIndex()),powerups[i].getLoc(),powerups[i].getTextureDim(),powerups[i].getDim(),powerups[i].getCurrentFrame(),0,powerups[i].getXOrientation(),false,global_tint[(int)TIMESTOPCT_POWERUPS],2);
    }
    if (true) // current_level >= BOSS_LEVEL3)
    {
        for (int i = 0; i < (int)npcs.size(); ++i)
        {
            if (npcs[i].getMobSuperFields()->p_type == POWERTYPE_THROUGHWALLS)
            {
                gfx.renderSprite(gfx.getTexture(npcs[i].getTextureIndex()),npcs[i].getLoc(),npcs[i].getTextureDim(),npcs[i].getDim(),npcs[i].getCurrentFrame(),getMobModRow(&npcs[i]),npcs[i].getXOrientation(),false,global_tint[(int)TIMESTOPCT_SHADOWS],2);
                printNPCHealthBar(&npcs[i]);
            }
        }
    }
    for (int i = 0; i < (int)particles.size(); ++i)
         gfx.renderSprite(gfx.getTexture(particles[i].getTextureIndex()),particles[i].getLoc(),particles[i].getTextureDim(),particles[i].getDim(),particles[i].getCurrentFrame(),0,particles[i].getXOrientation(),false,global_tint[(int)TIMESTOPCT_PARTICLES],2);
    // Render status area at top (health, experience, score, level)
    gfx.addBitmapCharacter(color_red,3,point(4.0,4.0));
    gfx.addBitmapString(color_white,int2String(getPlayerMob()->getHP()) + "/" + int2String(getPlayerMob()->getMobSuperFields()->max_hp),point(25.0,4.0));
    gfx.addBitmapCharacter(color_darkorange,(int)'E',point(4.0,20.0));
    gfx.addBitmapString(color_white,int2String(exp_level) + "/" + uint2String(exp_points),point(25.0,20.0));
    gfx.addBitmapCharacter(color_green,(int)'$',point(4.0,36.0));
    gfx.addBitmapString(color_white,int2String(score),point(25.0,36.0));
    gfx.addBitmapCharacter(color_white,(int)'L',point(4.0,52.0));
    gfx.addBitmapString(color_white,int2String(current_level),point(25.0,52.0));
    // Show inventory right below status area
    for (int i = 0; i < (int)player_inventory.size(); ++i)
    {
        gfx.renderSprite(gfx.getTexture(player_inventory[i].getTextureIndex()),point(2.0 + 12.0*i, 36.0),player_inventory[i].getTextureDim(),player_inventory[i].getDim(),0,0,SDL_FLIP_NONE,true,color_black,1);
    }
    // If the player is dead, print "Game Over", but keep everything else running until restart (by pressing v)
    if (player_mob.isDead())
    {
        gfx.addBitmapString(color_white,"Game Over! Press ESC to restart",point(WINDOW_WIDTH/2.0 - FONT_CHAR_WIDTH*31.0/2.0, WINDOW_HEIGHT/2.0 - FONT_CHAR_HEIGHT/2.0));
    }
    renderWeaponSkillPanel();
    renderNPCNameStatusIndicator();
    // call SDL_RenderPresent
    gfx.updateScreen();
}

void Game::renderWeaponSkillPanel()
{
    int y_iter = 0;
    int damage_bonus = 0;
    point tile_dim;
    point tile_loc;
    point str_loc;
    for (int i = 0; i < NUM_WEAPON_TYPES; ++i) {
         if (weapon_exp_bonus[i] > 0) {
             damage_bonus = weapon_exp_bonus[i] * weapon_bonus_level_damage_multipliers[i];
             tile_dim = item_data[i+1].idef.dimensions;
             tile_loc = multPoints(point(0.5,0.5),point((double)WINDOW_WIDTH - 94.0 + (33.0 - tile_dim.x()),8.0 + (y_iter * 24.0) + (18.0 - tile_dim.y())));
             str_loc = point((double)WINDOW_WIDTH - 54.0,8.0 + (y_iter * 24.0));
             gfx.renderSprite(gfx.getTexture(weapon_texture_indices[i]),tile_loc,tile_dim,tile_dim,0,0,SDL_FLIP_NONE,true,color_black,1);
             gfx.addBitmapString(color_white,"+" + int2String(damage_bonus),str_loc);
             y_iter++;
         }
    }
}

void Game::renderNPCNameStatusIndicator()
{
    std::string name_ind;
    SDL_Color color;
    point loc;
    if (npcTargetFocusID >= 1)
    {
        name_ind = getMobFromEntityID(npcTargetFocusID)->getName() + " " + int2String(getMobFromEntityID(npcTargetFocusID)->getHP()) + "/" + int2String(getMobFromEntityID(npcTargetFocusID)->getMobSuperFields()->max_hp);
        loc = point(WINDOW_WIDTH/2.0-16.0*(double)((int)name_ind.size()/2),8.0);
        switch(getMobFromEntityID(npcTargetFocusID)->getMobModifierType())
        {
            case(MOBMODIFIER_NONE):
                color = color_white;
                break;
            case(MOBMODIFIER_FAST):
                color = color_green;
                break;
            case(MOBMODIFIER_TOUGH):
                color = color_darkorange;
                break;
            case(MOBMODIFIER_FASTTOUGH):
                color = color_purple;
                break;
            default:
                break;
        }
        gfx.addBitmapString(color,name_ind,loc);
    }
}

// If an NPC is damaged (and not dead), a health bar is displayed right above the NPC sprite.
// If an NPC is not damaged, no health bar is displayed.
// The bar is scaled to the length of the sprite and color changes from green-yellow-orange-red smoothly.
void Game::printNPCHealthBar(mob *mb)
{
    if (mb->getHP() < mb->getMobSuperFields()->max_hp && !mb->isDead()) {
        double hp_ratio = (double)mb->getHP()/(double)mb->getMobSuperFields()->max_hp;
        Uint8 shade = (Uint8)((int)(hp_ratio*255.0));
        gfx.drawRectangle({(Uint8)(255-(shade/3)),shade,0,0},addPoints(mb->getLoc(),point(0.0,-3.0)),point(hp_ratio*mb->getDim().x(),2.0));
    }
}

// Update and configure animation counters and flags for all dynamic entities
void Game::updateAnimations() {
    for (auto it = particles.begin(); it != particles.end(); ++it) {
         if (it->getParticleType() == PARTICLE_SPAWNFLASH || it->getPhysicsStatus() == PHYSTYPE_FULL)
             it->animate(0,it->getNumFrames()-1);
    }

    for (auto it = props.begin(); it != props.end(); ++it) {
         if (it->getPhysicsStatus() == PHYSTYPE_FULL)
             it->animate(0, it->getNumFrames()-1);
    }

    for (auto it = items.begin(); it != items.end(); ++it) {
         if (eitherFromPlayerOrTimeActive(it->getPossessionMobID()))
             it->animate(0, it->getNumFrames()-1);
    }

    for (auto it = npcs.begin(); it != npcs.end(); ++it)
         it->animate(0, it->getNumFrames()-1);

    if (!getPlayerMob()->getLadderStatus()) {
        getPlayerMob()->animate(0,getPlayerMob()->getNumFrames()-3);
    }
    else {
        getPlayerMob()->animateCustomSequence(player_ladder_animation_sequence);
    }
}

// Used for rate of fire and determining when the next shot can be fired by each
// weapon. Only items affected by this are weapons.
void Game::updateItemTimers()
{
    for (auto it = items.begin(); it != items.end(); ++it) {
         if (eitherFromPlayerOrTimeActive(it->getPossessionMobID()))
             it->incUsageTimer();
    }
}

// Move door to next open position (up), close position (down),
// or possibly dock door at final closed or open position.
void Game::updateDoorTimers()
{
    for (auto it = doors.begin(); it != doors.end(); ++it)
         it->activateDoor();
}

// Wall blocks include level boundaries (indices 0-3)...
// or square wall blocks (or clusters)
int Game::numWallBlocks()
{
    return (int)walls.size();
}

// Always 1
int Game::numBackdrops()
{
    return (int)backdrops.size();
}

// get instance of the player object
mob* Game::getPlayerMob()
{
    return &player_mob;
}

// set wall texture color mod for this level
// 0 -> orange
// 1 -> red
// 2 -> yellow
void Game::setWallColorTint()
{
    switch(3)
    {
        case(0):
            color_wall_tint.r = randInt(100,148);
            color_wall_tint.g = randInt(15,79);
            color_wall_tint.b = 0;
            break;
        case(1):
            color_wall_tint.r = randInt(128,255);
            color_wall_tint.g = 0;
            color_wall_tint.b = 0;
            break;
        case(2):
            color_wall_tint.r = randInt(220,255);
            color_wall_tint.g = randInt(175,200);
            color_wall_tint.b = 0;
            break;
        default:
            color_wall_tint.r = 0;
            color_wall_tint.g = 0;
            color_wall_tint.b = 255;
            break;
    }
}

// set ladder texture color mod for this level
// 0 -> orange
// 1 -> red
// 2 -> yellow
void Game::setLadderColorTint()
{
    switch(0)
    {
        case(0):
            color_ladder_tint.r = randInt(127,255);
            color_ladder_tint.g = randInt(48,128);
            color_ladder_tint.b = 0;
            break;
        case(1):
            color_ladder_tint.r = randInt(128,255);
            color_ladder_tint.g = 0;
            color_ladder_tint.b = 0;
            break;
        case(2):
            color_ladder_tint.r = randInt(220,255);
            color_ladder_tint.g = randInt(175,200);
            color_ladder_tint.b = 0;
            break;
        default:
            color_ladder_tint.r = randInt(100,200);
            color_ladder_tint.g = color_ladder_tint.r;
            color_ladder_tint.b = color_ladder_tint.g;
            break;
    }
}

SDL_Color getMobTint(mob *mb) {
    if (mb->getMobDeathType() != MOBDEATHTYPE_INCINERATED) 
    {
        return color_black;
    }
    else 
    {
        return color_nearblack;
    }
}

// int to string
std::string int2String(int i)
{
    std::stringstream ss;
    ss << i;
    return ss.str();
}

// unsigned int to string
std::string uint2String(uint ui)
{
    std::stringstream ss;
    ss << ui;
    return ss.str();
}

// Return the weapon that the NPC spawns with
item_type getStartingWeaponForMob(mob_type mt)
{
    switch(mt)
    {
    case(MOB_FIGHTER):
    case(MOB_SOLDIER):
    case(MOB_CAPTAIN):
    case(MOB_WARRIOR):
    case(MOB_SLAYER):
    case(MOB_BEHEMOTH):
    case(MOB_GLADIATOR):
    case(MOB_CHAMPION):
    case(MOB_GRANDCHAMPION):
    case(MOB_EXECUTIONER):
    case(MOB_HAZMATGOD):
    case(MOB_KING):
    case(MOB_SHADOW):
    case(MOB_SHADOWKING):
        return ITEMTYPE_NONE;
    case(MOB_GUARD):
        return (item_type)(randInt(1,3));
    case(MOB_BIGGUARD):
        return (item_type)(randInt(2,3));
    case(MOB_HAZMATGUY):
        return ITEMTYPE_FLAMETHROWER;
    case(MOB_ELITEGUARD):
        return ITEMTYPE_CHAINGUN;
    case(MOB_AGENT):
        return (item_type)randInt(0,4);
    case(MOB_ADVANCEDAGENT):
        return ITEMTYPE_LASERGUN;
    default:
        break;
    }
    return ITEMTYPE_NONE;
}

// Is the weapon
// 1) normal
// 2) fast (x2 rate of fire)
// 3) damaging (x2 damage)
// 4) damaging and fast
int getWeaponModRow(item *it) {
    return (int)(it->getWeaponModifierType());
}

// Is the NPC
// 1) normal
// 2) fast (x2 max speed)
// 3) tough (x2 starting health)
// 4) tough and fast
int getMobModRow(mob *mb) {
    return (int)(mb->getMobModifierType());
}

int getWeaponLevelupBoundary(item_type wep, int index) {
    switch(wep) {
        case(ITEMTYPE_PISTOL):
             return pistol_bonus_levelup_values[index];
        case(ITEMTYPE_REVOLVER):
             return revolver_bonus_levelup_values[index];
        case(ITEMTYPE_SHOTGUN):
             return shotgun_bonus_levelup_values[index];
        case(ITEMTYPE_CHAINGUN):
             return chaingun_bonus_levelup_values[index];
        case(ITEMTYPE_FLAMETHROWER):
             return flamethrower_bonus_levelup_values[index];
        case(ITEMTYPE_CANNON):
             return cannon_bonus_levelup_values[index];
        case(ITEMTYPE_ROCKETLAUNCHER):
             return rocketlauncher_bonus_levelup_values[index];
        case(ITEMTYPE_LASERGUN):
             return lasergun_bonus_levelup_values[index];
        default:
             break;
    }

    return 1;
}
