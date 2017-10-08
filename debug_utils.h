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
    frame_event_block* find(uint16 find_id)
    {
        if(record_id==find_id)
        {
            return this;
        }else{
            for(uint32 i = 0; i < children.size(); i++)
            {
                frame_event_block* result = children[i]->find(find_id);
                if(result!=nullptr) return result;
            }
        }

        return nullptr;
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

#define DEBUG_MAX_COLOURPALATTE 50
const sf::Color kDebug_ColourPalatte[] =
{
    sf::Color(239, 204, 148),
	sf::Color(169, 173, 232),
	sf::Color(54, 247, 231),
	sf::Color(59, 56, 183),
	sf::Color(173, 34, 53),
	sf::Color(234, 242, 87),
	sf::Color(242, 98, 252),
	sf::Color(87, 164, 219),
	sf::Color(226, 110, 74),
	sf::Color(46, 67, 221),
	sf::Color(255, 174, 137),
	sf::Color(135, 232, 111),
	sf::Color(204, 232, 255),
	sf::Color(124, 224, 96),
	sf::Color(57, 90, 155),
	sf::Color(207, 81, 219),
	sf::Color(20, 135, 206),
	sf::Color(133, 37, 249),
	sf::Color(54, 193, 105),
	sf::Color(252, 108, 158),
	sf::Color(230, 188, 255),
	sf::Color(125, 177, 206),
	sf::Color(54, 91, 150),
	sf::Color(209, 237, 111),
	sf::Color(221, 135, 205),
	sf::Color(237, 166, 182),
	sf::Color(113, 232, 67),
	sf::Color(21, 114, 119),
	sf::Color(124, 234, 155),
	sf::Color(146, 239, 158),
	sf::Color(209, 154, 237),
	sf::Color(216, 181, 75),
	sf::Color(204, 225, 255),
	sf::Color(194, 161, 252),
	sf::Color(226, 113, 13),
	sf::Color(231, 252, 174),
	sf::Color(255, 134, 132),
	sf::Color(198, 176, 242),
	sf::Color(81, 204, 0),
	sf::Color(232, 120, 211),
	sf::Color(249, 159, 243),
	sf::Color(239, 194, 249),
	sf::Color(242, 171, 184),
	sf::Color(229, 22, 222),
	sf::Color(174, 214, 104),
	sf::Color(155, 148, 221),
	sf::Color(89, 214, 110),
	sf::Color(232, 134, 239),
	sf::Color(209, 242, 157),
	sf::Color(146, 13, 198)
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
    DEBUG_UI_NODE_PROFILER,
    DEBUG_UI_NODE_ITEM_LIST
};

struct DebugItemListData
{
    bool listIsOpen=false;
    vector<string>      option;
    vector<sf::Color>   option_colour;
    bool       isUnique=false;
    uint32*    selected_id=nullptr;
    uint32     local_selected_id=0;


    sf::Color selectedColour = sf::Color(36,180,200);
};

struct DebugProfilerUiData
{
    sf::Vector2f min_viewport_size = sf::Vector2f(500, 170);
    sf::Vector2f viewport_size = sf::Vector2f(800,170);
    sf::Vector2f viewportMargin = sf::Vector2f(0,10);
    uint32       framebar_height = 30;
    uint32       framebar_slotOutlineWidth=1;
    sf::Color    frameCurrentColour = sf::Color(48,197,255);
    sf::Color    frameViewingColour = sf::Color(0,0,0,0);
    sf::Color    frameViewingOutlineColour = sf::Color::White;
    sf::Color    frameOutlineColour = sf::Color::Black;
    sf::Color    frameBlankColour   = sf::Color(20,20,20);
    sf::Color    frameFineColour    = sf::Color(81,203,32);
    sf::Color    frameWarningColour = sf::Color(234,196,53);
    sf::Color    frameDangerColour  = sf::Color(215,38,56);

    real32       frameGraphHeight = 0.5;

    uint32       frameContextMargin = 10;
    uint32       frameContextHeight = 40;
    uint32       frameContextOutlineWidth  = 2;
    sf::Color    frameContextOutlineColour = sf::Color::Black;
    uint32       frameLayerHeight = 20;
    uint32       frameContextPadding = 10;

    uint64            frameIndexOfLastGraphUpdate=0;
    bool              frameTextureRewrite=false;

    uint16       framerateThresholdFine     = 60;
    uint16       framerateThresholdWarning  = 55;
    uint16       framerateThresholdDanger   = 45;

    string       tooltip_text="";

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
    uint32              margin = 2;

    union
    {
        bool  isSubMenuOpen;                              // SUBMENU
        bool* bool_pointer;                               // ITEM_BOOL
        void (*function_pointer)(DebugStateInformation&, DebugMenuNode*); // FUNCTION
    };
    DebugProfilerUiData ui_profiler;                      // PROFILER
    DebugItemListData option_list;                        // ITEM_LIST

    vector<DebugMenuNode*> children;
    DebugMenuNode*         parent=nullptr;

    void draw(sf::RenderWindow &window, InputState &input, DebugStateInformation &debug, sf::Text &text, sf::Vector2f &position, uint32 indent);
    DebugMenuNode* getChildByName(string name)
    {
        for(uint32 i = 0; i < children.size(); i++)
        {
            if(children[i]->display_name==name)
            {
                return children[i];
            }
        }
        return nullptr;
    }
    void resetParent(DebugMenuNode* new_parent)
    {
        parent = new_parent;
        for(uint32 i = 0; i < children.size(); i++)
        {
            children[i]->resetParent(this);
        }
    }

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

    DebugMenuNode(string name, vector<string> list, uint32* update_ptr=nullptr)
    {
        display_name = name;
        type         = DEBUG_UI_NODE_ITEM_LIST;
        option_list.option = list;
        option_list.selected_id = update_ptr;
    }

    DebugMenuNode(string name, void (*func_ptr)(DebugStateInformation&, DebugMenuNode*))
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

    DebugMenuNode(string name, DebugMenuNodeType type_)
    {
        display_name = name;
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

struct debug_ui_tooltip
{
    sf::Vector2i position;
    string       text;

    debug_ui_tooltip(string _text, sf::Vector2i _position)
    {
        text = _text;
        position = _position;
    }
};

struct DebugMenuUIState
{
    stack<DebugMenuNode*> node_stack;
    DebugMenuNode*        lastNodeAdded;

    DebugMenuNode rootNode = DebugMenuNode("Debug Menu");
    vector<FreeRoamingDebugMenuNode> freeRoamingNodeList;
    vector<debug_ui_tooltip>         tooltipList;
    sf::Vector2f  position = sf::Vector2f(0,0);
    uint32  indent=25;
    uint32  textSize = 20;


    DebugMenuUIState()
    {
        node_stack.push(&rootNode);
        lastNodeAdded = &rootNode;
    }

    sf::Vector2f draw(sf::RenderWindow &window, InputState &input, DebugStateInformation &debug, sf::Font &font);

    void addNode(DebugMenuNode* node, uint32 pop_back=0)
    {
        node->parent = node_stack.top();
        node_stack.top()->children.push_back(node);
        lastNodeAdded = node;

        if(pop_back > 0)
        {
            for(uint32 i = 0; i < pop_back && node_stack.size() > 1; i++)
            {
                node_stack.pop();
            }
        }else if(node->type==DEBUG_UI_NODE_SUBMENU)
        {
            node_stack.push(node);
    }
    }
};

struct debug_frame_record
{
    sf::Time duration = sf::milliseconds(0);
};

struct debug_frame_graph
{
    sf::RenderTexture *graph;
    uint64            frameLastUpdated=0;
};

struct DebugStateInformation
{
    bool     isEnabled = false;
    DebugMemoryAnalyzerState memoryAnalyzer;
    DebugMenuUIState         ui;

    sf::Font font;
    string   additionalInfo;

    GameState *gamestate=nullptr;

    // Trigger Flag
    bool           trigger_reloadItemList=false;
    DebugMenuNode* node_itemList;

    // Camera Data
    real32   free_camera_normalspeed = 12;
    real32   free_camera_fastspeed = 25;
    real32   free_camera_zoom_normalspeed = 0.05;
    real32   free_camera_zoom_fastspeed   = 0.1;
    real32   free_camera_max_zoom = 20;
    real32   free_camera_min_zoom = 0.1;

    // Entity Flags
    bool     user_clickToPlaceEntity=false;
    uint32   user_clickToPlaceEntity_id=0;
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
    uint64 overallFrameIndex        = 0;
    uint32 kDebugRecordSnapshotSize = 180;
    uint32 debugSnapshotIndex=0;
    frame_event_summary debugEventSnapshotArray[180];
    debug_frame_record debugFrameSnapshotArray[180];
    real32   lastRecordedFrameRate=60;
    int32    mouse_hovered_room_id = -1;


    vector<debug_frame_graph> frameGraph_list;

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

void initDebugState(DebugStateInformation &debug);
void updateDebugMemoryAnalyzer(DebugStateInformation &debug, GameMap &map, InputState &input);
void collateDebugEventFrameData(DebugStateInformation &debug);

#endif /* end of include guard: DEBUG_UTILS_H */
