#ifndef GAME_ITEM_H
#define GAME_ITEM_H
#include "game_consts.h"
#include "resource_manager.cpp"
#include "tool_functions.h"
#include "tool_functions.cpp"
#include "math_utils.h"
#include "math_utils.cpp"


struct game_item
{
    string          name;
    string          description;
    resource_handle texture_ui;
    resource_handle texture_game;
    uint32          unique_id;
};

struct game_item_reference
{
    uint32 id;
};

struct ItemManager
{
    uint32 item_count=0;
    uint32 item_current_index=0;
    map<uint32, game_item> item_list;

    game_item getItem(game_item_reference &ref)
    {
        assert(ref.id < item_count);
        return item_list[ref.id];
    }
};


#endif
