#ifndef GAME_ITEM_CPP
#define GAME_ITEM_CPP
#include "game_item.h"

map<uint32, game_item>::iterator doesItemExistByName(ItemManager &manager, string name)
{
    for (map<uint32, game_item>::iterator it=manager.item_list.begin(); it!=manager.item_list.end(); it++)
    {
        if(it->second.name==name)
        {
            return it;
        }
    }
    return manager.item_list.end();
}

void loadItemListFromConfigFile(ItemManager &manager, string filename, bool reloadItemList=false)
{
    Json::Value root = readJsonFile(filename);

    if(!reloadItemList)
    {
        manager.item_list.clear();
    }

    if(root.isMember("ItemList") && root["ItemList"].isArray())
    {
        for(uint32 i = 0; i < root["ItemList"].size(); i++)
        {
            game_item item;
            if(loadLinearJsonIntoMemory(root["ItemList"][(int)i], (byte*)&item, "s%item_name;s%description", false))
            {
                bool addNewItem=true;

                if(reloadItemList)
                {
                    map<uint32, game_item>::iterator it = doesItemExistByName(manager, item.name);
                    if(it != manager.item_list.end())
                    {
                        item.unique_id = it->second.unique_id;
                        it->second=item;
                        addNewItem=false;
                    }
                }

                if(addNewItem)
                {
                    manager.item_list[manager.item_current_index] = item;
                    manager.item_count++;
                    manager.item_current_index++;
                }
            }
        }
    }else{
        cout << "Error: Loading item list file format incorrect" << endl;
    }
}

#endif
