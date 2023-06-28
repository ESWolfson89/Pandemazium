// Copyright Eric Wolfson 2016-2017
// See LICENSE.txt (GPLv3)

#ifndef GENERATE_H_
#define GENERATE_H_

#include <stack>
#include "globals.h"
#include "point.h"
#include "rng.h"

static const int num_keys_per_level[39] =
{
    1,1,1,2,2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,6
};

// MAZE_EMPTY -> empty unblocked tile
// MAZE_WALL -> blocked wall tile
enum maze_unit_type
{
    MAZE_EMPTY,
    MAZE_WALL
};

enum maze_dig_direction
{
    DIG_UP,
    DIG_DOWN,
    DIG_LEFT,
    DIG_RIGHT
};

enum terrain_type
{
    TERRAINTYPE_ITEMPLACEHOLDER,
    TERRAINTYPE_ITEMPLACEHOLDER2,
    TERRAINTYPE_TRIMPLACEHOLDER,
    TERRAINTYPE_EMPTY,
    TERRAINTYPE_WALL,
    TERRAINTYPE_LADDER,
    TERRAINTYPE_DOOR,
    TERRAINTYPE_LOCKEDDOOR,
    TERRAINTYPE_BIGDOOR,
    TERRAINTYPE_EXIT,
    TERRAINTYPE_KEY,
    TERRAINTYPE_LADDER_2,
    TERRAINTYPE_BRICKBACKDROP_1
};

struct maze_unit_struct
{
    maze_unit_type u_type;
};

struct terrain_struct
{
    point sze;
    terrain_type t_type;
    int tid;
    int cid;
};

class maze
{
    public:
        // default const. to initialize all values of mazeArray to 0
        maze();
        // init maze to be trapped "MAZE_EMPTY" empty tiles
        void initMaze(point);
        // create maze
        void build();
        // set unit at location
        void setUnit(point, maze_unit_type);
        // dig...
        void digIntoMazeWall(maze_dig_direction&, point&, point&, point, point, point, point);
        // set points adjacent to "current_dig" in build()
        void setAdjacentPoints(point, point&, point&, point&, point&);
        void cleanupMaze();
        // check if any points adjacent to "current_dig" are trapped
        bool anyAdjacentTrapped(point, point, point, point, point);
        // get num MAZE_EMPTY units at initialization
        int getGuaranteedNumMazeFloors();
        // check if unit at point is trapped by 1s
        bool isUnitTrapped(point);
        // randomly pick dig direction
        maze_dig_direction pickDigDirection();
        // get unit at ith location
        maze_unit_type getUnit(point);
        point getMazeSize();
    private:
        // maze array value designations:
        std::vector < std::vector < maze_unit_type > > maze_array;
        // start point from maze dig algorithm
        point start_dig;
        // end point from maze dig algorithm
        point finish_dig;
        point maze_size;
        // maze RNG
};

class terrain_map
{
public:
    terrain_map();
    void cleanupTerrainMap();
    void loadLayoutFromFile(std::string fileName);
    void createTerrainMap(point,point,int);
    void initGeneratorFields(point,point);
    void buildMaze(point);
    void setTerrainStruct(point,point,terrain_type,int,int);
    void addLadderPoints();
    void trimEdges(int);
    void addMiniPlatforms(int);
    void extendLadders();
    void trimUnneededLadders();
    void addMiniDoors();
    void addBigDoors();
    void condenseWallCount();
    void connectLadderChains();
    void squishLadderEndPlatforms();
    void makeEdgesEmpty();
    void eraseTopWalls();
    void addItems();
    void addExit(int);
    void addKeys(int);
    terrain_struct getTerrainStruct(point);
    int terrainTilesAdj(point,int,int,terrain_type,bool);
    point getDim();
    point getBlockSize();
    point getStartBlock();
private:
    std::vector < std::vector < terrain_struct > > terrain_vec;
    std::vector < point > ladder_points;
    point dim;
    point block_size;
    point startBlock;
    int exit_block_count;
};

#endif
