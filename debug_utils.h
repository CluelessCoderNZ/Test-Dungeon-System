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
#define NAMED_BLOCK(y, x) timed_block TOKENPASTE(TimedBlock_, __LINE__)(__COUNTER__, __FILE__, __LINE__, y, x);


static inline uint64 rdtsc()
{
    uint64 lo,hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64)hi << 32) | lo;
}

enum debug_event_type
{
    DEBUG_EVENT_TIMED_BLOCK_START,
    DEBUG_EVENT_TIMED_BLOCK_END
};

struct debug_event
{
    uint64 clock;
    uint32 hitCount;
    uint16 type;
    uint16 record_id;
};

struct debug_profile_record
{
    string fileName;
    string functionName;
    uint32 lineNumber;
};

extern debug_profile_record DebugProfileRecordArray[];
extern const uint32 kTotalRecordCount;

const uint32 kMaxEventCount = (16 * 65536);
debug_event DebugEventList[kMaxEventCount];
uint32 DebugEvent_Index = 0;

struct timed_block
{
    uint32 record_id=0;
    timed_block(uint32 counter, string filename, uint32 linenumber, string functionname, uint32 hitcount = 1)
    {
        debug_profile_record* Record;
        Record = DebugProfileRecordArray + counter;
        Record->fileName = filename;
        Record->lineNumber = linenumber;
        Record->functionName = functionname;

        assert(DebugEvent_Index < kMaxEventCount);
        debug_event *event = &DebugEventList[DebugEvent_Index];
        event->clock = rdtsc();
        event->record_id = counter;
        event->hitCount = hitcount;
        event->type = (uint16)DEBUG_EVENT_TIMED_BLOCK_START;
        DebugEvent_Index++;

        record_id=counter;
    }

    ~timed_block()
    {
        assert(DebugEvent_Index < kMaxEventCount);
        debug_event *event = &DebugEventList[DebugEvent_Index];
        event->clock = rdtsc();
        event->record_id = record_id;
        event->type = (uint16)DEBUG_EVENT_TIMED_BLOCK_END;
        DebugEvent_Index++;
    }
};

struct frame_event_block
{
    uint16 record_id = 0;
    uint64 startClock = 0;
    uint64 clock_time = 0;
    uint32 hitCount = 0;

    vector<frame_event_block*> children;
    frame_event_block* parent = nullptr;

    ~frame_event_block()
    {
        for(uint32 i = 0; i < children.size(); i++)
        {
            delete children[i];
        }
    }
};

struct frame_event_summary
{
    frame_event_block main_event;
    uint32 max_depth=1;
};


struct GameState;
struct Entity_State_Controller;
struct Entity_Reference;
struct DebugStateInformation;

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
    DEBUG_UI_NODE_FUNCTION,
    DEBUG_UI_NODE_PROFILER
};

struct DebugProfilerUiData
{
    sf::Vector2f min_viewport_size = sf::Vector2f(500, 100);
    sf::Vector2f viewport_size = sf::Vector2f(800,100);
    sf::Vector2f viewportMargin = sf::Vector2f(0,10);
    uint32       framebar_height = 30;
    uint32       framebar_slotOutlineWidth=1;
    sf::Color    frameCurrentColour = sf::Color(48,197,255);
    sf::Color    frameViewingColour = sf::Color(0,0,0,0);
    sf::Color    frameViewingOutlineColour = sf::Color::White;
    sf::Color    frameOutlineColour = sf::Color::Black;
    sf::Color    frameBlankColour   = sf::Color(100,100,100);
    sf::Color    frameFineColour    = sf::Color(81,203,32);
    sf::Color    frameWarningColour = sf::Color(234,196,53);
    sf::Color    frameDangerColour  = sf::Color(215,38,56);

    uint32       frameContextMargin = 10;
    uint32       frameContextHeight = 40;
    uint32       frameContextOutlineWidth  = 2;
    sf::Color    frameContextOutlineColour = sf::Color::Black;
    uint32       frameLayerHeight = 20;
    uint32       frameContextPadding = 10;

    uint16       framerateThresholdFine     = 60;
    uint16       framerateThresholdWarning  = 55;
    uint16       framerateThresholdDanger   = 45;

    uint16       frameSelected_id  = 0;
    bool         frameSelected      = false;

    bool         profilerSelected = false;
    uint32       selectionSize = 5;
};

struct DebugMenuNode
{
    string              display_name;
    DebugMenuNodeType   type;

    sf::Color           textColour            = sf::Color(255,255,255);
    sf::Color           textHighlightColour   = sf::Color(255, 255, 125);
    sf::Color           textOutlineColour     = sf::Color(0,0,0);
    uint32              margin = 0;

    union
    {
        bool  isSubMenuOpen;                              // SUBMENU
        bool* bool_pointer;                               // ITEM_BOOL
        void (*function_pointer)(DebugStateInformation&); // FUNCTION
        DebugProfilerUiData ui_profiler;                  // PROFILER
    };
    vector<DebugMenuNode*> children;

    void draw(sf::RenderWindow &window, InputState &input, DebugStateInformation &debug, sf::Text &text, sf::Vector2f &position, uint32 indent);
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

    DebugMenuNode(string name, void (*func_ptr)(DebugStateInformation&))
    {
        display_name     = name;
        type             = DEBUG_UI_NODE_FUNCTION;
        function_pointer = func_ptr;
    }

    DebugMenuNode(DebugMenuNodeType type_)
    {
        type = type_;
        switch(type)
        {
            case DEBUG_UI_NODE_PROFILER:
            {
                ui_profiler = DebugProfilerUiData();
            }break;
        };
    }
};

struct FreeRoamingDebugMenuNode
{
    bool          selectedByCursor=true;
    sf::Vector2f  position;
    DebugMenuNode *node = new DebugMenuNode();
};

struct DebugMenuUIState
{
    DebugMenuNode rootNode = DebugMenuNode("Debug Menu");
    vector<FreeRoamingDebugMenuNode> freeRoamingNodeList;
    sf::Vector2f  position = sf::Vector2f(0,0);
    uint32  indent=25;
    uint32  textSize = 20;

    sf::Vector2f draw(sf::RenderWindow &window, InputState &input, DebugStateInformation &debug, sf::Font &font);
};

struct debug_frame_record
{
    sf::Time duration = sf::milliseconds(0);
};

struct DebugStateInformation
{
    bool     isEnabled = false;
    DebugMemoryAnalyzerState memoryAnalyzer;
    DebugMenuUIState         ui;

    sf::Font font;
    string   additionalInfo;

    // Camera Data
    real32   free_camera_normalspeed = 12;
    real32   free_camera_fastspeed = 25;
    real32   free_camera_zoom_normalspeed = 0.05;
    real32   free_camera_zoom_fastspeed   = 0.12;
    real32   free_camera_max_zoom = 20;
    real32   free_camera_min_zoom = 0.1;

    // Entity Flags
    bool     follow_memorySelectedEntity=false;
    bool     display_memorySelectedEntity=false;

    // System Flags
    bool     simulation_paused=false;
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

    // Debug Data
    uint32 kDebugRecordSnapshotSize = 180;
    uint32 debugSnapshotIndex=0;
    frame_event_summary debugEventSnapshotArray[180];
    debug_frame_record debugFrameSnapshotArray[180];
    real32   lastRecordedFrameRate=60;
    int32    mouse_hovered_room_id = -1;


    // UI Settings
    sf::Color colour_AdditionalInfo             = sf::Color::Yellow;
    sf::Color colour_roomBoundaries             = sf::Color::Green;
    sf::Color colour_roomGraph                  = sf::Color::Cyan;
    sf::Color colour_tilegrid                   = sf::Color(0,50,0);
    sf::Color colour_roomid                     = sf::Color(180,0,255);
    sf::Color colour_roomConnections            = sf::Color::Red;
    sf::Color colour_roomConnectionHighlight    = sf::Color(0,0,255);
    sf::Color colour_roomDifficulty             = sf::Color::Red;
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
void collateDebugEventFrameData(DebugStateInformation &debug);

#endif /* end of include guard: DEBUG_UTILS_H */
