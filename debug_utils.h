#ifndef DEBUG_UTILS_H
#define DEBUG_UTILS_H
#include <SFML/Graphics.hpp>
#include <iostream>
#include <typeinfo>
#include <bitset>
#include <sstream>
#include "entity.h"
#include "game_platform.h"
#include "tool_functions.h"
#include <time.h>

using namespace std;

#define TOKENPASTE_(x, y) x ## y
#define TOKENPASTE(x, y) TOKENPASTE_(x, y)

#define TIMED_BLOCK(x) timed_block TOKENPASTE(TimedBlock_, __LINE__)(__COUNTER__, __FILE__, __LINE__, __FUNCTION__, x);

static inline uint64 rdtsc()
{
    uint64 lo,hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64)hi << 32) | lo;
}

struct debug_profile_record
{
    uint64 startClock=0;
    uint64 clockCount = 0;
    uint32 hitCount = 0;

    string fileName;
    string functionName;
    uint32 lineNumber;
};

extern debug_profile_record DebugProfileRecordArray[];
extern uint32 kTotalRecordCount;

struct timed_block
{
    debug_profile_record *Record;

    timed_block(uint32 counter, string filename, uint32 linenumber, string functionname, uint32 hitcount = 1)
    {
        Record = DebugProfileRecordArray + counter;
        Record->fileName = filename;
        Record->lineNumber = linenumber;
        Record->functionName = functionname;
        Record->startClock = rdtsc();
        Record->hitCount += hitcount;
    }

    ~timed_block()
    {
        Record->clockCount += rdtsc()-Record->startClock;
    }
};


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

enum DebugMenuNodeType
{
    DEBUG_UI_NODE_END,
    DEBUG_UI_NODE_SUBMENU,
    DEBUG_UI_NODE_ITEM_BOOL,
    DEBUG_UI_NODE_PROFILER
};

struct DebugMenuNode
{
    string              display_name;
    DebugMenuNodeType   type;

    sf::Color           textColour            = sf::Color::White;
    sf::Color           textHighlightColour   = sf::Color(255, 255, 125);
    sf::Color           textOutlineColour     = sf::Color::Black;

    union
    {
        bool  isSubMenuOpen;                // SUBMENU
        bool* bool_pointer;                 // ITEM_BOOL
    };

    vector<DebugMenuNode*> children;

    void draw(sf::RenderWindow &window, InputState &input, sf::Text &text, uint32 margin, uint32 indent);
    DebugMenuNode(){}
    ~DebugMenuNode()
    {
        for(uint32 i = 0; i < children.size(); i++)
        {
            delete children[i];
        }
    }
    DebugMenuNode(string name)
    {
        display_name = name;
        type         = DEBUG_UI_NODE_SUBMENU;
        isSubMenuOpen= false;
    }

    DebugMenuNode(string name, bool* data)
    {
        display_name = name;
        type         = DEBUG_UI_NODE_ITEM_BOOL;
        bool_pointer = data;
    }

    DebugMenuNode(DebugMenuNodeType type_)
    {
        type = type_;
    }
};

struct DebugMenuUIState
{
    DebugMenuNode rootNode = DebugMenuNode("Debug Menu");
    sf::Vector2f  position = sf::Vector2f(0,0);
    uint32  margin=0;
    uint32  indent=25;
    uint32  textSize = 20;

    sf::Vector2f draw(sf::RenderWindow &window, InputState &input, sf::Font &font);
};

struct DebugStateInformation
{
    bool     isEnabled = false;
    DebugMemoryAnalyzerState memoryAnalyzer;
    DebugMenuUIState         ui;

    sf::Font font;
    string   additionalInfo;

    // Entity Flags
    bool     follow_memorySelectedEntity=false;
    bool     display_memorySelectedEntity=false;

    // System Flags
    bool     display_FPS=true;

    // Map Flags
    bool     display_RoomBoundaries=false;
    bool     display_RoomGraph=false;
    bool     display_TileGrid=false;
    bool     display_RoomID=false;
    bool     display_roomDifficulty=false;
    bool     display_RoomConnections=false;

    // Room Flags
    bool    display_TileID=false;
    bool    display_TileAO=false;


    sf::Color colour_AdditionalInfo             = sf::Color::Yellow;
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
string binaryToStr(byte value);

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
