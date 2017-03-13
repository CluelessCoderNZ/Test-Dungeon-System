#ifndef GAME_MAP_H
#define GAME_MAP_H
#include <vector>
#include <iostream>
#include <assert.h>
#include <sstream>
#include <limits>
#include <SFML/Graphics.hpp>
#include "game_consts.h"
#include "game_tileset.h"
#include "tool_functions.h"
#include "tool_functions.cpp"
#include "math_utils.h"
#include "math_utils.cpp"
#include "entity.h"
#include "map_flow_graph.h"
#include "map_flow_graph.cpp"

#include "delaunay/delaunay.h"
#include "delaunay/delaunay.cpp"

struct Entity_Reference;

using namespace std;

struct MapRoom_Refrence
{
    uint32 id;
};

enum MapDirection
{
    DIR_NONE,
    DIR_NORTH,
    DIR_EAST,
    DIR_SOUTH,
    DIR_WEST
};
MapDirection strToMapDirection(string a);

const string kMapDirectionString[5] =
{
    "NONE",
    "NORTH",
    "EAST",
    "SOUTH",
    "WEST"
};

const sf::Vector2i kMapDirectionNormals[5] =
{
    sf::Vector2i(0 , 0),
    sf::Vector2i(0 ,-1),
    sf::Vector2i(1 , 0),
    sf::Vector2i(0 , 1),
    sf::Vector2i(-1, 0)
};

struct MapRoomConnection
{
    MapRoom_Refrence primaryRoom;
    MapRoom_Refrence secondaryRoom;
    MapDirection     direction;
};

struct room_metadata_item
{
    string          type;
    Json::Value     data;
    sf::Vector2u    position;
};

struct RoomIndexConfigFile
{
    string      filename;
    string      folderDirectory;
    Json::Value root;
    uint32      chanceWeightTotal = 0;
};

struct MapRoom
{
    uint32                          room_type = 0;
    uint32                          room_group = 0;
    uint32                          steps_fromHomeRoom=0;
    vector<MapRoomConnection>       connection;
    sf::IntRect                     bounds;
    byte*                           tilemap;
    vector<room_metadata_item>      metadata;

    std::vector<Entity_Reference>   entity_list;

    inline byte getTile(uint32 x, uint32 y)
    {
        if(x >= bounds.width || y >= bounds.height)
            return 0;
        else
            return tilemap[bounds.width*y + x];
    }
};

struct MapRoom_SortKey
{
    int32  key      = 0;
    uint32 room_id  = 0;
};

struct MapRoom_GraphNode
{
  sf::Vector2i      point;
  MapRoom_Refrence  room;
  uint32            doorID;
  MapDirection      doorDirection;
};

struct MapRoom_GraphEdge
{
  MapRoom_GraphNode p1;
  MapRoom_GraphNode p2;
  real32            length;
  // This is not set if the map edge is apart of a loop
  bool              is_main_branch = true;

  vector<sf::Vector2i>      graphPath;
};

struct GameMap
{
    uint32                      home_Room = 0;
    vector<MapRoom>             room;
    vector<MapRoom_SortKey>     sort_list;
    sf::Vector2u                tileSize = sf::Vector2u(32,20);

    vector<MapRoom_GraphEdge> graphMap;
    MapFlowGraph::Node        graphFlow;
};


RoomIndexConfigFile loadRoomIndexConfigFile(string filename);

MapRoom loadTiledRoom_CSV(string filename, sf::Vector2u size);
MapRoom loadTiledRoom_JSON(string filename);
MapRoom loadTiledRoom(string filename, sf::Vector2u size=sf::Vector2u(0,0));
MapRoom loadRoomFromChanceSplit(RoomIndexConfigFile &file, real32 splitPoint);

vector<sf::Vector2i> getRoomToRoomPath(GameMap &map, sf::Vector2u corridor, MapRoom_GraphEdge edge);

GameMap generateRandomGenericDungeon(uint32 seed, string roomdata_filename);

void    generateRoomClusterNode(GameMap &map, mt19937 &random_engine, RoomIndexConfigFile &indexFile, MapRoom_Refrence parentRoom, uint32 clusterCount, real32 maxRoomDistance = 5, real32 minRoomDistance = 0);
GameMap generateRandomGenericDungeonUsingMapFlow(mt19937 &random_engine, string roomdata_filename);

MapRoom createRoom(sf::Vector2u size, byte tile_type = 0);

sf::Vector2f relativeRoomPosition(GameMap &map, sf::Vector2f position, MapRoom_Refrence old_room, MapRoom_Refrence new_room);
void    setRectTileArea(MapRoom &room, byte tile_id, sf::IntRect area);
bool    doesRectContainSolidTiles(GameMap &map, Tileset &tileset, uint32 room_id, sf::FloatRect rect,sf::Vector2f offset = sf::Vector2f(0,0));
void    attachRoom(GameMap &map, uint32 primaryRoom, uint32 secondaryRoom, sf::Vector2u primaryDoor, sf::Vector2u secondaryDoor, MapDirection direction);
int32   setTileFromGlobalPos(GameMap &map, sf::Vector2i position, byte tile_id);
bool    setTileAreaFromGlobalPos(GameMap &map, sf::IntRect area, byte tile_id);
int32   roomAtGlobalPos(GameMap &map, sf::Vector2i position);
bool    doesRectContainRoom(GameMap &map, sf::IntRect rect);
void    generateSortKeysForMap(GameMap &map);
void    generateHomeDistanceForRoom(GameMap &map, uint32 room_id, uint32 steps=0, int32 sourceEdge=-1);
void    insertion_sortRoomOrder(GameMap &map);
void    debugRenderRoom(sf::RenderTarget *target, GameMap &map, uint32 roomID);
sf::IntRect getRoomGroupBounds(GameMap &map, uint32 room_group);

#endif /* end of include guard: GAME_MAP_H */
