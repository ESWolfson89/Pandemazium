// Copyright Eric Wolfson 2016-2017
// See LICENSE.txt (GPLv3)

#include "generate.h"

maze::maze() {}

// create grid full of trapped open cells (see header for visual)
void maze::initMaze(point ms) {
    // Store maze dimensions (odd x, odd y) into maze size member variable
    maze_size = ms;
    // Resize 2D vector of maze_unit_type to maze_size.x() by maze_size.y()
    // and initialize all values to MAZE_WALL
    // maze_size.x() is used as outer dimension so that array access goes by [x][y] so that
    // x comes first.
    maze_array.resize(maze_size.x(), std::vector<maze_unit_type>(maze_size.y(), MAZE_WALL));

    // make maze have trapped MAZE_EMPTY tiles
    // i.e. make maze look like:
    // #########
    // # # # # #
    // #########
    // # # # # #-> (odd x)
    // #########
    // # # # # #
    // #########
    //     |
    //     V
    //   odd y
    // where "#" is MAZE_WALL and
    // where " " is MAZE_EMPTY
    for (int x = 0; x < maze_size.x(); ++x)
    for (int y = 0; y < maze_size.y(); ++y)
    {
        // modulo condition that yields empty maze tile positions
        if (x % 2 != 0 && y % 2 != 0)
            maze_array[x][y] = MAZE_EMPTY;
        else
            maze_array[x][y] = MAZE_WALL;
    }
    // make point(0,0) be a MAZE_WALL (blocked wall tile) to finish the job.
    // since the modulo condition ignores (0,0)
    maze_array[0][0] = MAZE_WALL;
}

// Clear and free memory of maze_array
void maze::cleanupMaze() {
    std::vector < std::vector < maze_unit_type > >().swap(maze_array);
}

// num guaranteed maze floors are num maze floors before carving is done.
// i.e. pretend we start with a maze of dimensions (9,7)
    // #########
    // # # # # #
    // #########
    // # # # # #
    // #########
    // # # # # #
    // #########
// Then, this function returns 12 representing 12 MAZE_EMPTY tiles
int maze::getGuaranteedNumMazeFloors() {
    return (int) ( ( maze_size.x() - 1) / 2) * (int) ( ( maze_size.y() - 1) / 2);
}

void maze::digIntoMazeWall(maze_dig_direction &direction,
                           point &selected, point &knock,
                           point u, point d, point l, point r) {

    while (!isUnitTrapped(selected)) {

        direction = pickDigDirection();

        if (direction == DIG_UP && u.y() > 0) {
            selected = knock = u;
            knock.sety(u.y() + 1);
        }
        else if (direction == DIG_DOWN && d.y() < maze_size.y() - 1) {
            selected = knock = d;
            knock.sety(d.y() - 1);
        }
        else if (direction == DIG_LEFT && l.x() > 0) {
            selected = knock = l;
            knock.setx(l.x() + 1);
        }
        else if (direction == DIG_RIGHT && r.x() < maze_size.x() - 1) {
            selected = knock = r;
            knock.setx(r.x() - 1);
        }
    }
}

void maze::setAdjacentPoints(point current_dig, point &u, point &d, point &l, point &r) {
    l.set(current_dig.x() - 2,current_dig.y());
    r.set(current_dig.x() + 2,current_dig.y());
    u.set(current_dig.x(),current_dig.y() - 2);
    d.set(current_dig.x(),current_dig.y() + 2);
}

bool maze::anyAdjacentTrapped(point current_dig, point u, point d, point l, point r) {
    return (isUnitTrapped(u) && u.y() > 0) || (isUnitTrapped(d) && d.y() < maze_size.y() - 1) ||
           (isUnitTrapped(l) && l.x() > 0) || (isUnitTrapped(r) && r.x() < maze_size.x() - 1);
}

// use stack method to create maze (one path solution)
void maze::build()
{
    std::stack <point> unit_stack;
    point current_dig = point(randZero((int)( (maze_size.x() - 1) / 2) - 1) * 2 + 1,randZero((int)( (maze_size.y() - 1) / 2) - 1) * 2 + 1);
    start_dig = current_dig;
    int num_visited = 1;
    point selected, knock, new_dig, u, d, l, r;
    maze_dig_direction direction;
    while (num_visited < getGuaranteedNumMazeFloors() ) {
        setAdjacentPoints(current_dig,u,d,l,r);
        if (anyAdjacentTrapped(current_dig,u,d,l,r)) {
            digIntoMazeWall(direction, selected, knock, u, d, l, r);
            maze_array[knock.x()][knock.y()] = MAZE_EMPTY;
            unit_stack.push(current_dig);
            current_dig = selected;
            num_visited++;
        }
        else {
            new_dig = unit_stack.top();
            unit_stack.pop();
            current_dig = new_dig;
        }
    }
}

// test for trapped cell (used by build function for determining whether or
// not we should dig here.
bool maze::isUnitTrapped(point p) {
    point a;

    for (int x = p.x() - 1; x <= p.x() + 1; ++x)
    for (int y = p.y() - 1; y <= p.y() + 1; ++y)
    {
        a.set(x,y);

        if (x != p.x() || y != p.y())
        if (inRange(a,point(0.0,0.0),point(maze_size.x()-1,maze_size.y()-1)))
        if (maze_array[x][y] != MAZE_WALL)
            return false;

    }
    return true;
}

maze_dig_direction maze::pickDigDirection() {
    return (maze_dig_direction)randInt(0,3);
}

// get unit
maze_unit_type maze::getUnit(point p) {
    return maze_array[p.x()][p.y()];
}

// set unit to...
void maze::setUnit(point p, maze_unit_type mut) {
    maze_array[p.x()][p.y()] = mut;
}

point maze::getMazeSize() {
    return maze_size;
}

terrain_map::terrain_map()
{
    dim = point(10,10);
    block_size = point(40.0,40.0);
    exit_block_count = 1;
    startBlock = point(0.0,0.0);
}

void terrain_map::cleanupTerrainMap()
{
    std::vector < std::vector < terrain_struct > >().swap(terrain_vec);
}

void terrain_map::setTerrainStruct(point loc, point sze, terrain_type t_type, int tid, int cid)
{
    terrain_vec[loc.y()][loc.x()].t_type = t_type;
    terrain_vec[loc.y()][loc.x()].sze = sze;
    terrain_vec[loc.y()][loc.x()].tid = tid;
    terrain_vec[loc.y()][loc.x()].cid = cid;
}

/*
    Note: What follows is a process that in my mind creates clever looking maps.
    It by no means follows any logical algorithm. I created it by just experimenting with the
    arrangement (order) of method calls along with testing different parameters with it.
    The problems are:
    1) The maps are not always connected (can be fixed with flood fill algorithm)
    2) Not all areas are always reachable (i.e. by jumping) Not trivial to fix, but
       definitely needs to be on todo list. Making it to level 67 with an unstoppable weapon
       and then having to quit (or die) because the end is unreachable would be very disappointing.
*/

// m_sze: maze dimensions (odd x, odd y)
// b_sze: block size (fixed at 40.0,40.0)
// lev: level number
void terrain_map::createTerrainMap(point m_sze, point b_sze, int lev)
{
    initGeneratorFields(m_sze,b_sze);
    buildMaze(m_sze);
    addExit(lev);
    addLadderPoints();
    trimEdges(randInt(4,6));
    extendLadders();
    //addMiniPlatforms(2);
    trimUnneededLadders();
    trimEdges(randInt(14,17));
    addMiniPlatforms(1);
    addBigDoors();
    addMiniDoors();
    connectLadderChains();
    makeEdgesEmpty();
    addItems();
    addKeys(lev);
    //condenseWallCount();
    //squishLadderEndPlatforms();
}

void terrain_map::squishLadderEndPlatforms() {
    terrain_type upper_tile;
    terrain_type lower_tile;
    terrain_type current_tile;

    for (int y = 0; y < (int)dim.y(); ++y)
    for (int x = 0; x < (int)dim.x(); ++x)
    {
        if (y > 1 && x > 0 && x < (int)dim.x() - 1 && y < (int)dim.y() - 2) {
            current_tile = getTerrainStruct(point(x,y)).t_type;
            upper_tile = getTerrainStruct(point(x,y - 1)).t_type;
            lower_tile = getTerrainStruct(point(x,y + 1)).t_type;

            if (current_tile == TERRAINTYPE_WALL && lower_tile == TERRAINTYPE_LADDER &&
               (upper_tile < (int)TERRAINTYPE_DOOR && lower_tile < (int)TERRAINTYPE_DOOR))
                setTerrainStruct(point(x,y),point(40.0,15.0),TERRAINTYPE_WALL,26,1);
        }
    }
}

void terrain_map::buildMaze(point m_sze)
{
    maze mze_obj;
    mze_obj.initMaze(m_sze);
    mze_obj.build();

    for (int y = 0; y < (int)dim.y(); ++y)
    for (int x = 0; x < (int)dim.x(); ++x)
    {
        if (mze_obj.getUnit(point(x,y)) == MAZE_WALL)
            setTerrainStruct(point(x,y),point(SMALL_BLOCK_DIM, SMALL_BLOCK_DIM),TERRAINTYPE_WALL,15,0);
        else
            setTerrainStruct(point(x,y),point(0.0,0.0),TERRAINTYPE_EMPTY,0,0);
    }

    mze_obj.cleanupMaze();
}

void terrain_map::addItems()
{
    for (int y = 1; y < (int)dim.y() - 1; ++y)
    for (int x = 1; x < (int)dim.x() - 1; ++x)
    {
        if(getTerrainStruct(point(x,y+1)).t_type == TERRAINTYPE_WALL &&
           getTerrainStruct(point(x,y)).t_type == TERRAINTYPE_EMPTY &&
           getTerrainStruct(point(x+1,y+1)).t_type != TERRAINTYPE_WALL &&
           getTerrainStruct(point(x-1,y+1)).t_type != TERRAINTYPE_WALL &&
           getTerrainStruct(point(x,y-1)).t_type != TERRAINTYPE_BIGDOOR)
        {
            if (!roll(29))
                setTerrainStruct(point(x,y),point(0.0,0.0),TERRAINTYPE_ITEMPLACEHOLDER,0,0);
            else
                setTerrainStruct(point(x,y),point(0.0,0.0),TERRAINTYPE_ITEMPLACEHOLDER2,0,0);
        }
    }

    for (int y = 2; y < (int)dim.y() - 2; ++y)
    for (int x = 2; x < (int)dim.x() - 2; ++x)
    {
        if(getTerrainStruct(point(x,y+1)).t_type == TERRAINTYPE_WALL &&
           getTerrainStruct(point(x,y)).t_type == TERRAINTYPE_EMPTY &&
           getTerrainStruct(point(x+1,y+1)).t_type == TERRAINTYPE_WALL &&
           getTerrainStruct(point(x-1,y+1)).t_type == TERRAINTYPE_WALL &&
           getTerrainStruct(point(x+2,y+1)).t_type == TERRAINTYPE_EMPTY &&
           getTerrainStruct(point(x-2,y+1)).t_type == TERRAINTYPE_EMPTY &&
           getTerrainStruct(point(x+1,y)).t_type == TERRAINTYPE_EMPTY &&
           getTerrainStruct(point(x-1,y)).t_type == TERRAINTYPE_EMPTY &&
           getTerrainStruct(point(x,y-1)).t_type != TERRAINTYPE_BIGDOOR)
        {
            if (!roll(4))
                setTerrainStruct(point(x,y),point(0.0,0.0),TERRAINTYPE_ITEMPLACEHOLDER,0,0);
            else
                setTerrainStruct(point(x,y),point(0.0,0.0),TERRAINTYPE_ITEMPLACEHOLDER2,0,0);
        }
    }
}

void terrain_map::eraseTopWalls()
{
    for (int x = 1; x < (int)dim.x() - 1; ++x)
    {
        if (getTerrainStruct(point(x,1)).t_type == TERRAINTYPE_WALL &&
            getTerrainStruct(point(x,2)).t_type != TERRAINTYPE_BIGDOOR &&
            getTerrainStruct(point(x,3)).t_type != TERRAINTYPE_BIGDOOR &&
            getTerrainStruct(point(x,2)).t_type != TERRAINTYPE_DOOR)
        {
            setTerrainStruct(point(x,1),point(0.0,0.0),TERRAINTYPE_EMPTY,0,0);
        }
    }
}

void terrain_map::makeEdgesEmpty()
{
    for (int y = 0; y < (int)dim.y(); ++y)
    for (int x = 0; x < (int)dim.x(); ++x)
    {
        if (y == 0 || x == 0 || x == (int)dim.x() - 1 || y == (int)dim.y() - 1)
        {
            setTerrainStruct(point(x,y),point(0.0,0.0),TERRAINTYPE_EMPTY,0,0);
        }
    }
}

void terrain_map::connectLadderChains()
{
    for (int x = 0; x < (int)dim.x(); ++x)
    {
        for (int y = 1; y < (int)dim.y() - 1; ++y)
        {
            if (getTerrainStruct(point(x,y)).t_type == TERRAINTYPE_WALL &&
                getTerrainStruct(point(x,y+1)).t_type == TERRAINTYPE_LADDER &&
                getTerrainStruct(point(x,y-1)).t_type == TERRAINTYPE_LADDER)
                {
                    setTerrainStruct(point(x,y),point(12.0,40.0),TERRAINTYPE_LADDER,16,0);
                }
        }
    }
}

void terrain_map::condenseWallCount()
{
    for (int x = 1; x < (int)dim.x() - 1; ++x)
    for (int y = 1; y < (int)dim.y() - 1; ++y)
    {
        if(getTerrainStruct(point(x,y)).t_type == TERRAINTYPE_WALL &&
           getTerrainStruct(point(x+1,y)).t_type == TERRAINTYPE_WALL &&
           isAt(getTerrainStruct(point(x,y)).sze,point(40.0,40.0)) &&
           isAt(getTerrainStruct(point(x+1,y)).sze,point(40.0,40.0)))
        {
            setTerrainStruct(point(x,y),point(80.0,40.0),TERRAINTYPE_WALL,2,0);
            setTerrainStruct(point(x+1,y),point(0.0,0.0),TERRAINTYPE_EMPTY,0,0);
        }
    }

    for (int y = 1; y < (int)dim.y() - 1; ++y)
    for (int x = 1; x < (int)dim.x() - 1; ++x)
    {
        if(getTerrainStruct(point(x,y)).t_type == TERRAINTYPE_WALL &&
           getTerrainStruct(point(x,y+1)).t_type == TERRAINTYPE_WALL &&
           isAt(getTerrainStruct(point(x,y)).sze,point(40.0,40.0)) &&
           isAt(getTerrainStruct(point(x,y+1)).sze,point(40.0,40.0)))
        {
            setTerrainStruct(point(x,y),point(40.0,80.0),TERRAINTYPE_WALL,26,0);
            setTerrainStruct(point(x,y+1),point(0.0,0.0),TERRAINTYPE_EMPTY,0,0);
        }
    }
}

void terrain_map::addKeys(int lev)
{
    point key_loc;

    int num_keys = 1;

    if (lev <= BOSS_LEVEL3)
        num_keys = num_keys_per_level[lev - 1];
    else
        num_keys = exit_block_count;

    for (int i = 0; i < num_keys; ++i)
    {
        do
        {
          key_loc.set(randInt(1,dim.x()-2),randInt(1,dim.y()-2));
        } while (!(getTerrainStruct(key_loc).t_type == TERRAINTYPE_EMPTY &&
                   getTerrainStruct(point(key_loc.x(),key_loc.y()+1)).t_type == TERRAINTYPE_WALL &&
                   getTerrainStruct(point(key_loc.x(),key_loc.y()-1)).t_type != TERRAINTYPE_BIGDOOR));
        setTerrainStruct(key_loc,point(0.0,0.0),TERRAINTYPE_KEY,0,0);
    }
}

void terrain_map::addLadderPoints()
{
    for (int y = (int)dim.y() - 2; y >= 1; --y)
    for (int x = 1; x <= (int)dim.x() - 2; ++x)
    {
        if (getTerrainStruct(point(x,y)).t_type == TERRAINTYPE_EMPTY &&
            getTerrainStruct(point(x,y+1)).t_type == TERRAINTYPE_EMPTY &&
            terrainTilesAdj(point(x,y),1,1,TERRAINTYPE_EMPTY,false) == 2)
        {
            setTerrainStruct(point(x,y),point(12.0,40.0),TERRAINTYPE_LADDER,16,0);
            ladder_points.push_back(point(x,y));
        }
    }
}

void terrain_map::addExit(int lev)
{
    if (lev < BOSS_LEVEL1)
        exit_block_count = num_keys_per_level[lev-1];
    else if (lev >= BOSS_LEVEL1 && lev <= BOSS_LEVEL3)
        exit_block_count = 3;
    else if (lev > BOSS_LEVEL3 && lev <= MAX_KEY_LEVEL)
        exit_block_count = randInt(3,num_keys_per_level[lev-1]);
    else
        exit_block_count = randInt(3,6);

    if (roll(2))
    {
        for (int i = 0; i < exit_block_count; ++i)
        {
            setTerrainStruct(point(2.0+i,3.0),point(40.0,40.0),TERRAINTYPE_WALL,15,0);
            setTerrainStruct(point(2.0+i,2.0),point(23.0,block_size.y()),TERRAINTYPE_LOCKEDDOOR,29,0);
            setTerrainStruct(point(2.0+i,1.0),point(40.0,40.0),TERRAINTYPE_WALL,15,0);
        }
        setTerrainStruct(point(3.0+(double)exit_block_count-1.0,2.0),point(0.0,0.0),TERRAINTYPE_EMPTY,0,0);
        setTerrainStruct(point(1.0,2.0),point(40.0,40.0),TERRAINTYPE_EXIT,30,0);
        setTerrainStruct(point(1.0,3.0),point(40.0,40.0),TERRAINTYPE_WALL,15,0);
        setTerrainStruct(point(1.0,1.0),point(40.0,40.0),TERRAINTYPE_WALL,15,0);
    }
    else
    {
        for (int i = 0; i < exit_block_count; ++i)
        {
            setTerrainStruct(point(dim.x()-3.0-i,3.0),point(40.0,40.0),TERRAINTYPE_WALL,15,0);
            setTerrainStruct(point(dim.x()-3.0-i,2.0),point(23.0,block_size.y()),TERRAINTYPE_LOCKEDDOOR,29,0);
            setTerrainStruct(point(dim.x()-3.0-i,1.0),point(40.0,40.0),TERRAINTYPE_WALL,15,0);
        }
        setTerrainStruct(point(dim.x()-4.0-(double)exit_block_count+1.0,2.0),point(0.0,0.0),TERRAINTYPE_EMPTY,0,0);
        setTerrainStruct(point(dim.x()-2.0,2.0),point(40.0,40.0),TERRAINTYPE_EXIT,30,0);
        setTerrainStruct(point(dim.x()-2.0,3.0),point(40.0,40.0),TERRAINTYPE_WALL,15,0);
        setTerrainStruct(point(dim.x()-2.0,1.0),point(40.0,40.0),TERRAINTYPE_WALL,15,0);
    }
}

void terrain_map::trimEdges(int num_iters)
{
    for (int n = 0; n < num_iters; ++n)
    {
        for (int y = 0; y < (int)dim.y(); ++y)
        for (int x = 0; x < (int)dim.x(); ++x)
        {
            if (getTerrainStruct(point(x,y)).t_type == TERRAINTYPE_WALL &&
                terrainTilesAdj(point(x,y),1,1,TERRAINTYPE_EMPTY,true) == 3)
            {
                terrain_vec[y][x].t_type = TERRAINTYPE_TRIMPLACEHOLDER;
            }
        }
        for (int y = 0; y < (int)dim.y(); ++y)
        for (int x = 0; x < (int)dim.x(); ++x)
        {
            if (getTerrainStruct(point(x,y)).t_type == TERRAINTYPE_TRIMPLACEHOLDER)
            {
                setTerrainStruct(point(x,y),point(0.0,0.0),TERRAINTYPE_EMPTY,0,0);
            }
        }
    }
}

void terrain_map::extendLadders()
{
    double iter = 1.0;
    for (int i = 0; i < (int)ladder_points.size(); ++i)
    {
        bool top_end = false;
        bool bottom_end = false;
        iter = 1.0;
        do
        {
           if (!inRange(addPoints(ladder_points[i],point(0.0,iter)),point(0.0,0.0),point(dim.x()-1,dim.y()-1)))
               bottom_end = true;
           else if (getTerrainStruct(addPoints(ladder_points[i],point(0.0,iter))).t_type == TERRAINTYPE_EMPTY)
           {
               setTerrainStruct(addPoints(ladder_points[i],point(0.0,iter)),point(12.0,40.0),TERRAINTYPE_LADDER,16,0);
               iter += 1.0;
           }
           else
               bottom_end = true;
        } while(!bottom_end);
        iter = -1.0;
        do
        {
           if (!inRange(addPoints(ladder_points[i],point(0.0,iter)),point(0.0,0.0),point(dim.x()-1,dim.y()-1)))
               top_end = true;
           else if (getTerrainStruct(addPoints(ladder_points[i],point(0.0,iter))).t_type == TERRAINTYPE_EMPTY)
           {
               setTerrainStruct(addPoints(ladder_points[i],point(0.0,iter)),point(12.0,40.0),TERRAINTYPE_LADDER,16,0);
               iter -= 1.0;
           }
           else
               top_end = true;
        } while(!top_end);
    }

}

void terrain_map::trimUnneededLadders()
{
    for (int y = 2; y < (int)dim.y() - 2; ++y)
    for (int x = 1; x < (int)dim.x() - 1; ++x)
    {
        if (getTerrainStruct(point(x,y)).t_type == TERRAINTYPE_LADDER &&
            getTerrainStruct(point(x,y-2)).t_type == TERRAINTYPE_WALL &&
            getTerrainStruct(point(x,y+2)).t_type == TERRAINTYPE_WALL)
        {
            setTerrainStruct(point(x,y),point(0.0,0.0),TERRAINTYPE_EMPTY,0,0);
            setTerrainStruct(point(x,y-1),point(0.0,0.0),TERRAINTYPE_EMPTY,0,0);
            setTerrainStruct(point(x,y+1),point(0.0,0.0),TERRAINTYPE_EMPTY,0,0);
        }
    }

    ladder_points.clear();
}

void terrain_map::addMiniPlatforms(int variation)
{
    for (int y = 1; y < (int)dim.y() - 1; ++y)
    for (int x = 1; x < (int)dim.x() - 1; ++x)
    {
        if (getTerrainStruct(point(x,y)).t_type == TERRAINTYPE_EMPTY &&
            terrainTilesAdj(point(x,y),1,1+variation,TERRAINTYPE_EMPTY,false) == 8 + (6*variation))
        {
            setTerrainStruct(point(x,y),point(40.0,40.0),TERRAINTYPE_WALL,15,0);
        }
    }
    for (int y = 1; y < (int)dim.y() - 1; ++y)
    for (int x = 1; x < (int)dim.x() - 1; ++x)
    {
        if (getTerrainStruct(point(x,y)).t_type == TERRAINTYPE_EMPTY &&
            terrainTilesAdj(point(x,y),1,1,TERRAINTYPE_WALL,true) == 2 &&
            terrainTilesAdj(point(x,y),1,1,TERRAINTYPE_WALL,false) == 2 &&
            getTerrainStruct(point(x-1,y)).t_type == TERRAINTYPE_WALL &&
            getTerrainStruct(point(x+1,y)).t_type == TERRAINTYPE_WALL)
        {
            setTerrainStruct(point(x,y),point(40.0,40.0),TERRAINTYPE_WALL,15,0);
        }
    }
}

void terrain_map::addMiniDoors()
{
    for (int y = 2; y < (int)dim.y() - 2; ++y)
    for (int x = 2; x < (int)dim.x() - 2; ++x)
    {
        if (getTerrainStruct(point(x,y)).t_type == TERRAINTYPE_WALL &&
            terrainTilesAdj(point(x,y),1,1,TERRAINTYPE_WALL,true) == 2 &&
            getTerrainStruct(point(x,y-1)).t_type == TERRAINTYPE_WALL &&
            getTerrainStruct(point(x,y+1)).t_type == TERRAINTYPE_WALL &&
            getTerrainStruct(point(x,y+2)).t_type != TERRAINTYPE_BIGDOOR &&
            terrainTilesAdj(point(x,y),2,7,TERRAINTYPE_DOOR,false) == 0)
        {
            setTerrainStruct(point(x,y),point(23.0,block_size.y()),TERRAINTYPE_DOOR,24,0);
        }
    }
}

void terrain_map::addBigDoors()
{
    for (int y = 2; y < (int)dim.y() - 2; ++y)
    for (int x = 2; x < (int)dim.x() - 2; ++x)
    {
        if (getTerrainStruct(point(x,y)).t_type == TERRAINTYPE_WALL &&
            terrainTilesAdj(point(x,y),1,1,TERRAINTYPE_WALL,true) == 2 &&
            terrainTilesAdj(point(x,y+1),1,1,TERRAINTYPE_WALL,true) == 2 &&
            getTerrainStruct(point(x,y-1)).t_type == TERRAINTYPE_WALL &&
            getTerrainStruct(point(x,y-2)).t_type == TERRAINTYPE_WALL &&
            getTerrainStruct(point(x,y+1)).t_type == TERRAINTYPE_WALL &&
            getTerrainStruct(point(x,y+2)).t_type == TERRAINTYPE_WALL &&
            terrainTilesAdj(point(x,y),2,5,TERRAINTYPE_BIGDOOR,false) == 0)
        {
            setTerrainStruct(point(x,y+1),point(0.0,0.0),TERRAINTYPE_EMPTY,0,0);
            setTerrainStruct(point(x,y),point(23.0,block_size.y()*2.0),TERRAINTYPE_BIGDOOR,22,0);
        }
    }
}

point terrain_map::getBlockSize()
{
    return block_size;
}

terrain_struct terrain_map::getTerrainStruct(point p)
{
    return terrain_vec[p.y()][p.x()];
}

point terrain_map::getDim()
{
    return dim;
}

int terrain_map::terrainTilesAdj(point p, int radx, int rady, terrain_type tt, bool vn) {
    point aloc;
    int counter = 0;
    for (int dy = -rady; dy <= rady; ++dy)
    for (int dx = -radx; dx <= radx; ++dx) {
        aloc.set(dx+p.x(),dy+p.y());
        if (inRange(aloc,point(0.0,0.0),point(dim.x()-1.0,dim.y()-1.0)))
        if (!isAt(aloc,p))
        if (dx == 0 || dy == 0 || !vn)
        if (terrain_vec[aloc.y()][aloc.x()].t_type == tt) {
            counter++;
        }
    }
    return counter;
}

void terrain_map::loadLayoutFromFile(std::string fileName) {
    point p;

    initGeneratorFields(point(32.0,14.0),point(40.0,40.0));

    std::vector<char> layoutVector;
    std::fstream layoutFile(fileName);

    for(std::istreambuf_iterator<char> it(layoutFile), e; it != e; ++it) {
        char cell = *it;
        if (cell != '\0' && cell != '\n' && cell != '\t') {
            layoutVector.push_back(cell);
        }
    }

    for (int x = 1; x < (int)dim.x() - 1; ++x)
    for (int y = 1; y < (int)dim.y() - 1; ++y) {
        p.set((double)x,(double)y);

        switch(layoutVector[((int)dim.x() * y) + x]) {
            case('#'):
                     setTerrainStruct(p,point(40.0,40.0),TERRAINTYPE_WALL,15,0);
                     break;
            case('l'):
                     setTerrainStruct(p,point(12.0,40.0),TERRAINTYPE_LADDER,16,0);
                     break;
            case('p'):
                     setTerrainStruct(p,point(12.0,40.0),TERRAINTYPE_LADDER_2,60,0);
                     break;
            case('.'):
                     setTerrainStruct(p,point(40.0,40.0),TERRAINTYPE_BRICKBACKDROP_1,61,0);
                     break;
            case('D'):
                     setTerrainStruct(p,point(23.0,40.0),TERRAINTYPE_DOOR,24,0);
                     break;
            case('S'):
                     startBlock = p;
            default:
                     setTerrainStruct(p,point(0.0,0.0),TERRAINTYPE_EMPTY,0,0);
                     break;
        }
    }

    int tid;

    terrain_type l, r, u, d;

    for (int x = 1; x < (int)dim.x() - 1; ++x) {
        for (int y = 1; y < (int)dim.y() - 1; ++y) {
            if (getTerrainStruct(point(x,y)).t_type == TERRAINTYPE_BRICKBACKDROP_1) {
                l = getTerrainStruct(point(x-1,y)).t_type;
                r = getTerrainStruct(point(x+1,y)).t_type;
                u = getTerrainStruct(point(x,y-1)).t_type;
                d = getTerrainStruct(point(x,y+1)).t_type;

                tid = 61;

                if (l == TERRAINTYPE_WALL && u == TERRAINTYPE_WALL)
                    tid = 67;
                else if (l == TERRAINTYPE_WALL && d == TERRAINTYPE_WALL)
                    tid = 68;
                else if (r == TERRAINTYPE_WALL && u == TERRAINTYPE_WALL)
                    tid = 69;
                else if (r == TERRAINTYPE_WALL && d == TERRAINTYPE_WALL)
                    tid = 70;
                else if (l == TERRAINTYPE_WALL && r == TERRAINTYPE_BRICKBACKDROP_1)
                    tid = 65;
                else if (r == TERRAINTYPE_WALL && l == TERRAINTYPE_BRICKBACKDROP_1)
                    tid = 66;
                else if (u == TERRAINTYPE_WALL && d == TERRAINTYPE_BRICKBACKDROP_1)
                    tid = 63;
                else if (d == TERRAINTYPE_WALL && u == TERRAINTYPE_BRICKBACKDROP_1) {
                    if (getTerrainStruct(point(x+1,y-1)).t_type != TERRAINTYPE_WALL)
                        tid = 64;
                    else
                        tid = 72;
                }
                else if (u == TERRAINTYPE_WALL && d == TERRAINTYPE_WALL) {
                    if (r != TERRAINTYPE_DOOR)
                        tid = 62;
                    else
                        tid = 71;
                }

                terrain_vec[y][x].tid = tid;
            }
        }
    }
}

void terrain_map::initGeneratorFields(point gridSize, point blockSize) {
    terrain_struct terrainStruct = {point(0.0,0.0),TERRAINTYPE_WALL,15,0};
    terrain_vec.resize( gridSize.y(), std::vector<terrain_struct>( gridSize.x(), terrainStruct ) );
    dim = gridSize;
    block_size = blockSize;
}

point terrain_map::getStartBlock() {
    return startBlock;
}

