#ifndef DEBUG_UTILS_H
#define DEBUG_UTILS_H
#include <SFML/Graphics.hpp>
#include <iostream>
#include <typeinfo>
#include <sstream>
#include "entity.h"
#include "game_platform.h"
#include "tool_functions.h"

using namespace std;

struct GameState;
struct Entity_State_Controller;
struct Entity_Reference;

#define DEBUG_MAX_COLOURPALATTE 7
const sf::Color kDebug_ColourPalatte[] =
{
    sf::Color(242, 178, 51),
    sf::Color(204, 76, 4),
    sf::Color(51, 102, 204),
    sf::Color(87, 166, 78),
    sf::Color(178, 102, 229),
    sf::Color(127, 204, 25),
    sf::Color(222, 222, 108)
};

struct DebugMemoryAnalyzerState
{
    bool             isEnabled = false;
    sf::RenderWindow window;
    real32           viewOffset=0;
    real32           scale=1;
    real32           scaleMin = 1;
    real32           scaleMax = 10;
    uint32           chunk_id_margin = 0;
    uint32           chunk_id_sectionSize = 100;
    uint32           chunk_height = 75;
    uint32           entity_expandedHeight = 120;
    uint32           entity_expandedWidth  = 325;
    bool             enity_isSelected=false;
    Entity_Reference selected_entity;

    Entity_State_Controller *controller;
};

struct DebugStateInformation
{
    bool     isEnabled = false;
    DebugMemoryAnalyzerState memoryAnalyzer;

    sf::Font font;

    bool     follow_memorySelectedEntity=true;
    bool     display_memorySelectedEntity=true;

    bool     display_FPS=true;
    bool     display_RoomBoundaries=true;
    bool     display_RoomGraph=true;
    bool     display_TileGrid=false;
    bool     display_RoomID=true;
    bool     display_roomDifficulty=false;
    bool     display_RoomConnections=false;

    sf::Color colour_fps                        = sf::Color::Yellow;
    sf::Color colour_roomBoundaries             = sf::Color::Green;
    sf::Color colour_roomGraph                  = sf::Color::Cyan;
    sf::Color colour_tilegrid                   = sf::Color(0,50,0);
    sf::Color colour_roomid                     = sf::Color(180,0,255);
    sf::Color colour_roomConnections            = sf::Color::Red;
    sf::Color colour_roomConnectionHighlight    = sf::Color(0,0,255);
    sf::Color colour_roomDifficulty             = sf::Color::Red;

    real32   lastRecordedFrameRate=60;
    int32    mouse_hovered_room_id = -1;
};

string numToStr(real32 value, int32 sf = -1);
string numToStr(int32 value);

string variableToStr(sf::Vector2u  value);
string variableToStr(sf::Vector2f  value);
string variableToStr(sf::Vector2i  value);
string variableToStr(sf::IntRect   value);
string variableToStr(sf::FloatRect value);
string variableToStr(sf::Color     value);
string variableToStr(uint32        value);
string variableToStr(real32        value);
string variableToStr(int32         value);
string variableToStr(bool          value);

void initDebugState(DebugStateInformation &debug);
void updateDebugMemoryAnalyzer(DebugStateInformation &debug, GameMap &map, InputState &input);


#endif /* end of include guard: DEBUG_UTILS_H */
