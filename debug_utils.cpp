#ifndef DEBUG_UTILS_CPP
#define DEBUG_UTILS_CPP
#include "debug_utils.h"


using namespace std;

void copyDebugMenuNode(DebugMenuNode *src, DebugMenuNode *dest)
{
    *dest = *src;
    dest->children.clear();

    for(uint32 i = 0; i < src->children.size(); i++)
    {
        dest->children.push_back(new DebugMenuNode());
        copyDebugMenuNode(src->children[i], dest->children[i]);
    }
    dest->resetParent(dest->parent);
}

void clearDebugRoamingMenuNodeList(DebugStateInformation &debug, DebugMenuNode *node)
{
    for(uint32 i = 0; i < debug.ui.freeRoamingNodeList.size(); i++)
    {
        delete debug.ui.freeRoamingNodeList[i].node;
    }
    debug.ui.freeRoamingNodeList.clear();
}

void reloadDebugItemList(DebugStateInformation &debug, DebugMenuNode *node)
{
    debug.trigger_reloadItemList=true;
}

void reloadDebugResourceManager(DebugStateInformation &debug, DebugMenuNode *node)
{
    ResourceManager::instance().reloadAll();
}

void initDebugState(DebugStateInformation &debug)
{
    debug.font.loadFromFile("Resources/Fonts/Debug.woff");

    ImGuiIO& IO = ImGui::GetIO();
    IO.Fonts->Clear();
    IO.Fonts->AddFontFromFileTTF("Resources/Fonts/Cousine-Regular.ttf", 16.f);
    ImGui::SFML::UpdateFontTexture();
    //ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);

    // Load Inital Settings
    {
        // Debug Style Colours
        Json::Value root = readJsonFile(kDataFile_debug_colours);
        if(!loadLinearJsonIntoMemory(root, (byte*)(&debug.colour), kDataFormat_debug_colours, true))
        {
            cout << "Error when loading debug style settings" << endl;
        }
    }




    debug.debugEventSnapshotArray = new frame_event_block[debug.kDebugRecordSnapshotSize*kTotalRecordCount]();


    debug.frameGraph_list.resize(kTotalRecordCount+1);
    for(uint32 i = 0; i < kTotalRecordCount+1; i++)
    {
        debug.frameGraph_list[i].graph = new sf::RenderTexture;
    }

    // Initalize UI State
    debug.ui.addNode(new DebugMenuNode("Map"));
        debug.ui.addNode(new DebugMenuNode("Render"));
            debug.ui.addNode(new DebugMenuNode("ReloadTextures", &reloadDebugResourceManager));
            debug.ui.addNode(new DebugMenuNode("Display_RoomBoundaries", &debug.display_RoomBoundaries));
            debug.ui.addNode(new DebugMenuNode("Display_RoomGraph", &debug.display_RoomGraph));
            debug.ui.addNode(new DebugMenuNode("Display_TileGrid", &debug.display_TileGrid));
            debug.ui.addNode(new DebugMenuNode("Display_RoomID", &debug.display_RoomID));
            debug.ui.addNode(new DebugMenuNode("Display_RoomConnections", &debug.display_RoomConnections), 1);
        debug.ui.addNode(new DebugMenuNode("Room"));
            debug.ui.addNode(new DebugMenuNode("Display_TileID", &debug.display_TileID));
            debug.ui.addNode(new DebugMenuNode("Display_TileAO", &debug.display_TileAO), 2);

    debug.ui.addNode(new DebugMenuNode("Entity"));
        debug.ui.addNode(new DebugMenuNode("OpenEntityCache", &debug.memoryAnalyzer.isEnabled));
        debug.ui.addNode(new DebugMenuNode("Display_SelectedEntity", &debug.display_memorySelectedEntity));
        debug.ui.addNode(new DebugMenuNode("Follow_SelectedEntity", &debug.follow_memorySelectedEntity));
        debug.ui.addNode(new DebugMenuNode("Entity Factory"));
            debug.ui.addNode(new DebugMenuNode("Type", {}, &debug.user_clickToPlaceEntity_id));
            for(uint32 i = 0; i < kEntityTypeCount; i++)
            {
                debug.ui.lastNodeAdded->option_list.option.push_back(kEntityTypeString[i]);
            }
            debug.ui.addNode(new DebugMenuNode("Click To Add", &debug.user_clickToPlaceEntity), 2);

    debug.ui.addNode(new DebugMenuNode("System"));
        debug.ui.addNode(new DebugMenuNode("Display_FPS", &debug.display_FPS));
        debug.ui.addNode(new DebugMenuNode("Profiler"));
            debug.ui.addNode(new DebugMenuNode("Paused", &debug.simulation_paused));
            debug.ui.addNode(new DebugMenuNode("Filter", {"None"}, nullptr));
            debug.ui.lastNodeAdded->option_list.isUnique=true;
            debug.ui.lastNodeAdded->option_list.option_colour.push_back(sf::Color::White);
            debug.ui.addNode(new DebugMenuNode(DEBUG_UI_NODE_PROFILER), 2);

    debug.ui.addNode(new DebugMenuNode("UI"));
        debug.ui.addNode(new DebugMenuNode("ClearDebugNodes", &clearDebugRoamingMenuNodeList), 1);

    static uint32 item_index=0;
    debug.ui.addNode(new DebugMenuNode("Items"));
        debug.ui.addNode(new DebugMenuNode("ReloadItemFile", &reloadDebugItemList));
        debug.ui.addNode(new DebugMenuNode("Item", {}, &item_index), 1);
        debug.node_itemList = debug.ui.lastNodeAdded;
}

void updateDebugMemoryAnalyzer(DebugStateInformation &debug, GameMap &map, InputState &input)
{
     TIMED_BLOCK(1, 0);
    // Update Window to match
    if(debug.memoryAnalyzer.isEnabled)
    {
        if(!debug.memoryAnalyzer.window.isOpen())
        {
            debug.memoryAnalyzer.window.create(sf::VideoMode(1024,768), "Memory Analyzer");
        }
    }else{
        if(debug.memoryAnalyzer.window.isOpen())
        {
            debug.memoryAnalyzer.window.close();
        }
    }

    if(debug.memoryAnalyzer.window.isOpen())
    {
        sf::Event event;
        while(debug.memoryAnalyzer.window.pollEvent(event))
        {
            switch(event.type)
            {
                case sf::Event::Closed:
                {
                    debug.memoryAnalyzer.isEnabled=false;
                }break;
                case sf::Event::Resized:
                {
                    sf::View newView;
                    newView.reset(sf::FloatRect(0, 0, debug.memoryAnalyzer.window.getSize().x, debug.memoryAnalyzer.window.getSize().y));

                    debug.memoryAnalyzer.window.setView(newView);
                }break;
                case sf::Event::MouseWheelScrolled :
                {
                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                    {
                        debug.memoryAnalyzer.scale += event.mouseWheelScroll.delta*0.1;

                        debug.memoryAnalyzer.scale = min(max(debug.memoryAnalyzer.scale, debug.memoryAnalyzer.scaleMin), debug.memoryAnalyzer.scaleMax);
                    }else{
                        debug.memoryAnalyzer.viewOffset += event.mouseWheelScroll.delta*0.005;
                    }
                    debug.memoryAnalyzer.viewOffset = min(max((real32)debug.memoryAnalyzer.viewOffset, (real32)0), real32(1.0-1.0/debug.memoryAnalyzer.scale));
                }break;
            }
        }

        debug.memoryAnalyzer.window.clear();

        sf::RectangleShape chunk_index_display;
        chunk_index_display.setOutlineColor(sf::Color(235,235,235));
        chunk_index_display.setFillColor(sf::Color(200,200,200));
        chunk_index_display.setOutlineThickness(5);
        chunk_index_display.setSize(sf::Vector2f(debug.memoryAnalyzer.chunk_id_sectionSize-chunk_index_display.getOutlineThickness()*2, debug.memoryAnalyzer.chunk_height-chunk_index_display.getOutlineThickness()*2));

        sf::RectangleShape chunk_background;
        chunk_background.setOutlineColor(sf::Color(100,100,100));
        chunk_background.setOutlineThickness(chunk_index_display.getOutlineThickness());
        chunk_background.setSize(sf::Vector2f(debug.memoryAnalyzer.window.getSize().x-chunk_background.getOutlineThickness()*2 - debug.memoryAnalyzer.chunk_id_sectionSize-debug.memoryAnalyzer.chunk_id_margin, debug.memoryAnalyzer.chunk_height-chunk_background.getOutlineThickness()*2));

        sf::Text           chunk_index_text;
        chunk_index_text.setFillColor(sf::Color(120, 180, 175));
        chunk_index_text.setCharacterSize(chunk_index_display.getSize().y);
        chunk_index_text.setFont(debug.font);

        uint32 lineSize = 10;
        uint32 lineDistance = 50;
        uint32 lineLength   = 75;
        sf::Color lineColour = sf::Color(35,35,35);
        sf::VertexArray chunk_emptySpaceLine(sf::Quads, 4);

        for(uint32 chunk_index = 0; chunk_index < debug.memoryAnalyzer.controller->entity_storage.chunk.size(); chunk_index++)
        {
            chunk_background.setFillColor(sf::Color(50,50,50));
            chunk_background.setPosition(debug.memoryAnalyzer.chunk_id_margin + debug.memoryAnalyzer.chunk_id_sectionSize + chunk_background.getOutlineThickness(), chunk_background.getOutlineThickness()+chunk_index*(debug.memoryAnalyzer.chunk_height-chunk_background.getOutlineThickness()));
            debug.memoryAnalyzer.window.draw(chunk_background);

            for(uint32 x = lineDistance; x < chunk_background.getSize().x + lineLength; x+=lineDistance)
            {
                chunk_emptySpaceLine[0] = sf::Vertex(sf::Vector2f(chunk_background.getPosition().x + chunk_background.getSize().x - x,chunk_background.getPosition().y + chunk_background.getSize().y), lineColour);
                chunk_emptySpaceLine[1] = sf::Vertex(sf::Vector2f(chunk_background.getPosition().x + chunk_background.getSize().x - x + lineLength,chunk_background.getPosition().y), lineColour);
                chunk_emptySpaceLine[2] = sf::Vertex(sf::Vector2f(chunk_background.getPosition().x + chunk_background.getSize().x - x + lineLength + lineSize,chunk_background.getPosition().y), lineColour);
                chunk_emptySpaceLine[3] = sf::Vertex(sf::Vector2f(chunk_background.getPosition().x + chunk_background.getSize().x - x + lineSize,chunk_background.getPosition().y + chunk_background.getSize().y), lineColour);
                debug.memoryAnalyzer.window.draw(chunk_emptySpaceLine);
            }

            chunk_background.setFillColor(sf::Color(0,0,0, 0));
            debug.memoryAnalyzer.window.draw(chunk_background);
        }

        // Fill in memory chunks
        sf::RectangleShape memoryItem;
        memoryItem.setOutlineThickness(2);

        sf::Text entityID_text;
        entityID_text.setFont(debug.font);
        entityID_text.setOutlineThickness(2);
        entityID_text.setOutlineColor(sf::Color::Black);
        entityID_text.setCharacterSize(12);

        for(uint32 room_index=0; room_index < map.room.size(); room_index++)
        {
            for(uint32 i=0; i < map.room[room_index].entity_list.size(); i++)
            {
                Entity_Reference ref = map.room[room_index].entity_list[i];

                // Entity Header
                memoryItem.setPosition(-debug.memoryAnalyzer.viewOffset*(debug.memoryAnalyzer.scale*chunk_background.getSize().x)+debug.memoryAnalyzer.scale*chunk_background.getSize().x*((real32)ref.chunkOffset / debug.memoryAnalyzer.controller->entity_storage.chunkSize) + debug.memoryAnalyzer.chunk_id_margin + debug.memoryAnalyzer.chunk_id_sectionSize + chunk_background.getOutlineThickness(), chunk_background.getOutlineThickness()+ref.chunkID*(debug.memoryAnalyzer.chunk_height-chunk_background.getOutlineThickness()));
                memoryItem.setSize(sf::Vector2f(debug.memoryAnalyzer.scale*chunk_background.getSize().x*((real32)sizeof(Entity) / debug.memoryAnalyzer.controller->entity_storage.chunkSize) - memoryItem.getOutlineThickness()*2,
                                   chunk_background.getSize().y));
                memoryItem.setFillColor(sf::Color(255,200,0));
                memoryItem.setOutlineColor(sf::Color(memoryItem.getFillColor().r*0.75,memoryItem.getFillColor().g*0.75,memoryItem.getFillColor().b*0.75));

                if(memoryItem.getPosition().x+memoryItem.getSize().x > 0 && memoryItem.getPosition().x < debug.memoryAnalyzer.window.getSize().x)
                {
                    if(debug.memoryAnalyzer.enity_isSelected && debug.memoryAnalyzer.selected_entity.global_id == ref.global_id)
                    {
                        memoryItem.setOutlineColor(sf::Color::White);
                    }

                    debug.memoryAnalyzer.window.draw(memoryItem);

                    entityID_text.setPosition(memoryItem.getPosition());
                    entityID_text.setString(numToStr((int32)ref.global_id));
                    if(entityID_text.getLocalBounds().width < memoryItem.getSize().x)
                    {
                        debug.memoryAnalyzer.window.draw(entityID_text);
                    }

                }else
                {
                    continue;
                }

                // Check if clicking should become new entity focus
                if(input.action(MOUSE_LEFTCLICK).state == BUTTON_PRESSED && memoryItem.getGlobalBounds().contains(sf::Mouse::getPosition(debug.memoryAnalyzer.window).x, sf::Mouse::getPosition(debug.memoryAnalyzer.window).y))
                {
                    debug.memoryAnalyzer.enity_isSelected=true;
                    debug.memoryAnalyzer.selected_entity = ref;
                    debug.memoryAnalyzer.chunk_id_margin = debug.memoryAnalyzer.entity_expandedWidth;
                }

                Entity entity           = getEntity(*debug.memoryAnalyzer.controller, ref);
                uint32 chunkMaxSize     = entity.chunkSize;
                uint32 unknownChunkLeft = chunkMaxSize;

                // Specfic typed chunks
                uint32 index = 0;
                for(uint32 j = 1; j < (1 << ENTITY_COMPONENT_MAX_FLAGS) && unknownChunkLeft > 0; j+=j)
                {
                    if(j & (uint32)entity.component)
                    {
                        memoryItem.setPosition(-debug.memoryAnalyzer.viewOffset*(debug.memoryAnalyzer.scale*chunk_background.getSize().x)+debug.memoryAnalyzer.scale*chunk_background.getSize().x*((real32)(ref.chunkOffset+sizeof(Entity)+chunkMaxSize-unknownChunkLeft) / debug.memoryAnalyzer.controller->entity_storage.chunkSize) + debug.memoryAnalyzer.chunk_id_margin + debug.memoryAnalyzer.chunk_id_sectionSize + chunk_background.getOutlineThickness(), chunk_background.getOutlineThickness()+ref.chunkID*(debug.memoryAnalyzer.chunk_height-chunk_background.getOutlineThickness()));
                        memoryItem.setSize(sf::Vector2f(debug.memoryAnalyzer.scale*chunk_background.getSize().x*((real32)kEntity_Component_ByteSize[index] / debug.memoryAnalyzer.controller->entity_storage.chunkSize) - memoryItem.getOutlineThickness()*2,
                                           chunk_background.getSize().y));
                        memoryItem.setFillColor(kDebug_ColourPalatte[index]);
                        memoryItem.setOutlineColor(sf::Color(memoryItem.getFillColor().r*0.75,memoryItem.getFillColor().g*0.75,memoryItem.getFillColor().b*0.75));
                        unknownChunkLeft-=kEntity_Component_ByteSize[index];

                        if(memoryItem.getPosition().x+memoryItem.getSize().x > 0 && memoryItem.getPosition().x < debug.memoryAnalyzer.window.getSize().x)
                        {
                            debug.memoryAnalyzer.window.draw(memoryItem);
                        }else
                        {
                            break;
                        }
                    }
                    index++;
                }

                // Remaining chunk
                if(unknownChunkLeft > 0)
                {
                    memoryItem.setPosition(-debug.memoryAnalyzer.viewOffset*(debug.memoryAnalyzer.scale*chunk_background.getSize().x)+debug.memoryAnalyzer.scale*chunk_background.getSize().x*((real32)(ref.chunkOffset+sizeof(Entity)+chunkMaxSize-unknownChunkLeft) / debug.memoryAnalyzer.controller->entity_storage.chunkSize) + debug.memoryAnalyzer.chunk_id_margin + debug.memoryAnalyzer.chunk_id_sectionSize + chunk_background.getOutlineThickness(), chunk_background.getOutlineThickness()+ref.chunkID*(debug.memoryAnalyzer.chunk_height-chunk_background.getOutlineThickness()));
                    memoryItem.setSize(sf::Vector2f(debug.memoryAnalyzer.scale*chunk_background.getSize().x*((real32)unknownChunkLeft / debug.memoryAnalyzer.controller->entity_storage.chunkSize) - memoryItem.getOutlineThickness()*2,
                                       chunk_background.getSize().y));
                    memoryItem.setFillColor(sf::Color(80,80,80));
                    memoryItem.setOutlineColor(sf::Color(memoryItem.getFillColor().r*0.75,memoryItem.getFillColor().g*0.75,memoryItem.getFillColor().b*0.75));
                    if(memoryItem.getPosition().x+memoryItem.getSize().x > 0 && memoryItem.getPosition().x < debug.memoryAnalyzer.window.getSize().x)
                    {
                        debug.memoryAnalyzer.window.draw(memoryItem);
                    }else
                    {
                        continue;
                    }
                }
            }
        }

        for(uint32 chunk_index = 0; chunk_index < debug.memoryAnalyzer.controller->entity_storage.chunk.size(); chunk_index++)
        {
            chunk_index_display.setPosition(debug.memoryAnalyzer.chunk_id_margin + chunk_index_display.getOutlineThickness(), chunk_background.getOutlineThickness()+chunk_index*(debug.memoryAnalyzer.chunk_height-chunk_background.getOutlineThickness()));
            debug.memoryAnalyzer.window.draw(chunk_index_display);

            chunk_index_text.setString(numToStr((int32)chunk_index));
            chunk_index_text.setPosition(chunk_index_display.getPosition().x + chunk_index_display.getSize().x/2 - chunk_index_text.getLocalBounds().width/2, chunk_index_display.getPosition().y + chunk_index_display.getSize().y/2 - chunk_index_text.getLocalBounds().height);
            debug.memoryAnalyzer.window.draw(chunk_index_text);
        }

        // Selected entity
        if(debug.memoryAnalyzer.enity_isSelected)
        {
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Delete))
            {
                removeEntity(*debug.memoryAnalyzer.controller, debug.memoryAnalyzer.selected_entity);
                debug.memoryAnalyzer.controller->entity_storage.compressChunk(debug.memoryAnalyzer.selected_entity.chunkID, debug.memoryAnalyzer.controller->entity_list);
                debug.memoryAnalyzer.enity_isSelected = false;
                debug.memoryAnalyzer.chunk_id_margin = 0;
            }

            Entity entity           = getEntity(*debug.memoryAnalyzer.controller, debug.memoryAnalyzer.selected_entity);
            uint32 chunkMaxSize     = entity.chunkSize;
            uint32 chunkCursor      = 0;

            memoryItem.setOutlineThickness(5);
            memoryItem.setSize(sf::Vector2f(debug.memoryAnalyzer.entity_expandedWidth-memoryItem.getOutlineThickness()*2, debug.memoryAnalyzer.entity_expandedHeight-memoryItem.getOutlineThickness()*2));
            memoryItem.setPosition(memoryItem.getOutlineThickness(), memoryItem.getOutlineThickness());
            memoryItem.setFillColor(sf::Color(255,200,0));
            memoryItem.setOutlineColor(sf::Color(memoryItem.getFillColor().r*0.75,memoryItem.getFillColor().g*0.75,memoryItem.getFillColor().b*0.75));
            debug.memoryAnalyzer.window.draw(memoryItem);

            sf::Text componentDataText;
            string   data;
            componentDataText.setFont(debug.font);
            componentDataText.setCharacterSize(16);
            componentDataText.setOutlineThickness(2);

            data = "";
            data = data + "flagForClear: " + variableToStr(entity.flagForClear) + "\n";
            data = data + "id:           " + variableToStr(entity.id)           + "\n";
            data = data + "component:    " + variableToStr(entity.component)    + "\n";
            data = data + "system:       " + variableToStr(entity.system)       + "\n";
            data = data + "chunkSize:    " + variableToStr(entity.chunkSize)    + "\n";
            componentDataText.setString(data);
            componentDataText.setPosition(memoryItem.getPosition()+sf::Vector2f(memoryItem.getSize().x/2, 5)-sf::Vector2f(componentDataText.getLocalBounds().width/2, 0));
            debug.memoryAnalyzer.window.draw(componentDataText);

            uint32 renderSpaceStart = memoryItem.getSize().y+memoryItem.getOutlineThickness()*2;
            uint32 renderSpaceHeight = debug.memoryAnalyzer.window.getSize().y - renderSpaceStart;

            uint32 index = 0;
            for(uint32 j = 1; j < (1 << ENTITY_COMPONENT_MAX_FLAGS) && chunkCursor < chunkMaxSize; j+=j)
            {
                if(j & (uint32)entity.component)
                {
                    memoryItem.setPosition(memoryItem.getPosition().x, renderSpaceStart+memoryItem.getOutlineThickness()+renderSpaceHeight*((real32)chunkCursor/chunkMaxSize));
                    memoryItem.setSize(sf::Vector2f(memoryItem.getSize().x, renderSpaceHeight*((real32)kEntity_Component_ByteSize[index]/chunkMaxSize)-memoryItem.getOutlineThickness()*2));
                    memoryItem.setFillColor(kDebug_ColourPalatte[index]);
                    memoryItem.setOutlineColor(sf::Color(memoryItem.getFillColor().r*0.75,memoryItem.getFillColor().g*0.75,memoryItem.getFillColor().b*0.75));

                    debug.memoryAnalyzer.window.draw(memoryItem);
                    chunkCursor+=kEntity_Component_ByteSize[index];

                    data = "";
                    switch((Entity_Component_Flags)j)
                    {
                        case EC_POSITION:
                        {
                            data = data + "position: " + variableToStr(((Entity_Component_Position*)getEntityComponent(*debug.memoryAnalyzer.controller, debug.memoryAnalyzer.selected_entity, EC_POSITION))->position) + "\n";
                            data = data + "room:     " + variableToStr(((Entity_Component_Position*)getEntityComponent(*debug.memoryAnalyzer.controller, debug.memoryAnalyzer.selected_entity, EC_POSITION))->room)     + "\n";
                        }break;
                        case EC_VELOCITY:
                        {
                            data = data + "velocity: " + variableToStr(((Entity_Component_Velocity*)getEntityComponent(*debug.memoryAnalyzer.controller, debug.memoryAnalyzer.selected_entity, EC_VELOCITY))->velocity) + "\n";
                        }break;
                        case EC_SPEED:
                        {
                            data = data + "speed: " + variableToStr(((Entity_Component_Speed*)getEntityComponent(*debug.memoryAnalyzer.controller, debug.memoryAnalyzer.selected_entity, EC_SPEED))->speed) + "\n";
                        }break;
                        case EC_SHAPERENDER:
                        {
                            data = data + "fillColour: " + variableToStr(((Entity_Component_ShapeRender*)getEntityComponent(*debug.memoryAnalyzer.controller, debug.memoryAnalyzer.selected_entity, EC_SHAPERENDER))->fillColour) + "\n";
                            data = data + "outlineColour: " + variableToStr(((Entity_Component_ShapeRender*)getEntityComponent(*debug.memoryAnalyzer.controller, debug.memoryAnalyzer.selected_entity, EC_SHAPERENDER))->outlineColour) + "\n";
                            data = data + "outlineThickness: " + variableToStr(((Entity_Component_ShapeRender*)getEntityComponent(*debug.memoryAnalyzer.controller, debug.memoryAnalyzer.selected_entity, EC_SHAPERENDER))->outlineThickness) + "\n";
                            data = data + "size:             " + variableToStr(((Entity_Component_ShapeRender*)getEntityComponent(*debug.memoryAnalyzer.controller, debug.memoryAnalyzer.selected_entity, EC_SHAPERENDER))->size) + "\n";
                        }break;
                        case EC_SIZE_BOUNDS:
                        {
                            data = data + "bounds: " + variableToStr(((Entity_Component_Size*)getEntityComponent(*debug.memoryAnalyzer.controller, debug.memoryAnalyzer.selected_entity, EC_SIZE_BOUNDS))->size) + "\n";
                        }break;
                    }
                    componentDataText.setString(data);
                    componentDataText.setPosition(memoryItem.getPosition()+sf::Vector2f(memoryItem.getSize().x/2, 5)-sf::Vector2f(componentDataText.getLocalBounds().width/2, 0));
                    debug.memoryAnalyzer.window.draw(componentDataText);
                }
                index++;
            }
        }

        debug.memoryAnalyzer.window.display();
    }
}


void draw_DebugMenuNodeSubMenu(sf::RenderWindow &window, InputState &input, DebugStateInformation &debug, DebugMenuNode *node, sf::Text &text, sf::Vector2f &position)
{
    string textStr = node->display_name;
    if (node->isSubMenuOpen)
    {
        textStr = "V "+textStr;
    }else{
        textStr = "> "+textStr;
    }

    text.setPosition(position);
    text.setString(textStr);
    text.setOutlineColor(node->textOutlineColour);

    // Mouse Input
    if(text.getGlobalBounds().contains(input.mouse_screenPos.x, input.mouse_screenPos.y))
    {
        if(input.action(MOUSE_LEFTCLICK).state == BUTTON_PRESSED)
        {
            if(input.action(INPUT_CONTROL).isDown)
            {
                debug.ui.freeRoamingNodeList.push_back(FreeRoamingDebugMenuNode());
                copyDebugMenuNode(node, debug.ui.freeRoamingNodeList[debug.ui.freeRoamingNodeList.size()-1].node);
            }else{
                node->isSubMenuOpen = !node->isSubMenuOpen;
            }
        }else{
            text.setFillColor(node->textHighlightColour);
        }
    }else{
        text.setFillColor(node->textColour);
    }

    window.draw(text);

    position.y+=node->margin + text.getGlobalBounds().height;
}

void draw_DebugMenuNodeItemBool(sf::RenderWindow &window, InputState &input, DebugStateInformation &debug, DebugMenuNode *node, sf::Text &text, sf::Vector2f &position)
{
    string textStr = node->display_name;
    textStr = textStr+": "+variableToStr(*node->bool_pointer);

    text.setPosition(position);
    text.setString(textStr);
    text.setOutlineColor(node->textOutlineColour);
    // Mouse Input
    if(text.getGlobalBounds().contains(input.mouse_screenPos.x, input.mouse_screenPos.y))
    {
        if(input.action(MOUSE_LEFTCLICK).state == BUTTON_PRESSED)
        {
            if(input.action(INPUT_CONTROL).isDown)
            {
                debug.ui.freeRoamingNodeList.push_back(FreeRoamingDebugMenuNode());
                copyDebugMenuNode(node, debug.ui.freeRoamingNodeList[debug.ui.freeRoamingNodeList.size()-1].node);
            }else{
                *node->bool_pointer = !(*node->bool_pointer);
            }
        }else{
            text.setFillColor(node->textHighlightColour);
        }
    }else{
        text.setFillColor(node->textColour);
    }

    window.draw(text);

    position.y+=node->margin + text.getGlobalBounds().height;
}

void draw_DebugMenuNodeItemFunction(sf::RenderWindow &window, InputState &input, DebugStateInformation &debug, DebugMenuNode *node, sf::Text &text, sf::Vector2f &position)
{
    string textStr = node->display_name;
    textStr = textStr+"()";

    text.setPosition(position);
    text.setString(textStr);
    text.setOutlineColor(node->textOutlineColour);
    // Mouse Input
    if(text.getGlobalBounds().contains(input.mouse_screenPos.x, input.mouse_screenPos.y))
    {
        if(input.action(MOUSE_LEFTCLICK).state == BUTTON_PRESSED)
        {
            if(input.action(INPUT_CONTROL).isDown)
            {
                debug.ui.freeRoamingNodeList.push_back(FreeRoamingDebugMenuNode());
                copyDebugMenuNode(node, debug.ui.freeRoamingNodeList[debug.ui.freeRoamingNodeList.size()-1].node);
            }else{
                node->function_pointer(debug, node);
            }
        }else{
            text.setFillColor(node->textHighlightColour);
        }
    }else{
        text.setFillColor(node->textColour);
    }

    window.draw(text);

    position.y+=node->margin + text.getGlobalBounds().height;
}

void draw_DebugMenuNodeItemList(sf::RenderWindow &window, InputState &input, DebugStateInformation &debug, DebugMenuNode *node, sf::Text &text, sf::Vector2f &position)
{
    if(node->option_list.isUnique)
    {
        node->option_list.selected_id = &node->option_list.local_selected_id;
    }else if(node->option_list.selected_id==nullptr)
    {
        node->option_list.selected_id = new uint32;
        *node->option_list.selected_id = 0;
    }

    real32 height=0;
    string textStr = node->display_name+": ";
    if(*node->option_list.selected_id < node->option_list.option.size())
        textStr+=node->option_list.option[*node->option_list.selected_id];

    text.setPosition(position);
    text.setString(textStr);
    text.setFillColor(node->textColour);
    text.setOutlineColor(node->textOutlineColour);

    bool mouseOverTitleText=false;
    mouseOverTitleText = text.getGlobalBounds().contains(input.mouse_screenPos.x, input.mouse_screenPos.y);
    if(mouseOverTitleText)
    {
        text.setFillColor(node->textHighlightColour);
    }

    textStr = node->display_name+": ";
    text.setString(textStr);
    window.draw(text);

    if(*node->option_list.selected_id < node->option_list.option.size())
    {
        text.move(text.getGlobalBounds().width, 0);
        if(*node->option_list.selected_id < node->option_list.option_colour.size())
        {
            text.setFillColor(node->option_list.option_colour[*node->option_list.selected_id]);
        }else{
            text.setFillColor(node->option_list.selectedColour);
        }
        text.setString(node->option_list.option[*node->option_list.selected_id]);
        window.draw(text);
    }
    height+=node->margin + text.getGlobalBounds().height;

    // Mouse Input
    if(!node->option_list.listIsOpen)
    {
        if(mouseOverTitleText)
        {
            if(input.action(MOUSE_LEFTCLICK).state == BUTTON_PRESSED)
            {
                if(input.action(INPUT_CONTROL).isDown)
                {
                    debug.ui.freeRoamingNodeList.push_back(FreeRoamingDebugMenuNode());
                    copyDebugMenuNode(node, debug.ui.freeRoamingNodeList[debug.ui.freeRoamingNodeList.size()-1].node);
                }else{
                    node->option_list.listIsOpen=true;
                }
            }
        }
    }else{
        for(uint32 i = 0; i < node->option_list.option.size(); i++)
        {
            if(i == *node->option_list.selected_id)
            {
                text.setFillColor(node->option_list.selectedColour);
                text.setOutlineColor(sf::Color::White);
            }else{
                if(i < node->option_list.option_colour.size())
                {
                    text.setFillColor(node->option_list.option_colour[i]);
                }else{
                    text.setFillColor(node->textColour);
                }
            }
            text.setPosition(position+sf::Vector2f(0,height));
            text.setString(whitespace(2+node->display_name.length())+node->option_list.option[i]);

            if(text.getGlobalBounds().contains(input.mouse_screenPos.x, input.mouse_screenPos.y))
            {
                if(input.action(MOUSE_LEFTCLICK).state == BUTTON_PRESSED)
                {
                    *node->option_list.selected_id=i;
                }
                text.setFillColor(node->textHighlightColour);
            }

            window.draw(text);
            text.setOutlineColor(sf::Color::Black);
            height+=node->margin + text.getGlobalBounds().height;
        }

        if(input.action(MOUSE_LEFTCLICK).state == BUTTON_PRESSED)
        {
            node->option_list.listIsOpen=false;
        }
    }

    text.setFillColor(node->textColour);

    position.y+=height;
}

void draw_DebugFrameEventBlock(uint32 &event_id, uint32 snapshot_id, sf::RenderWindow &window, InputState &input, DebugStateInformation &debug, DebugMenuNode *node, sf::Text &text, sf::Vector2f &position, uint32 width, uint32 layerHeight)
{
    sf::RectangleShape rect;
    rect.setPosition(position);
    rect.setSize(sf::Vector2f(width, layerHeight*node->ui_profiler.frameLayerHeight+node->ui_profiler.frameContextPadding));
    rect.setFillColor(kDebug_ColourPalatte[debug.getEventSnapshot(event_id, snapshot_id)->record_id % DEBUG_MAX_COLOURPALATTE]);
    rect.setOutlineColor(node->ui_profiler.frameContextOutlineColour);
    rect.setOutlineThickness(node->ui_profiler.frameContextOutlineWidth);
    window.draw(rect);

    if(rect.getGlobalBounds().contains(input.mouse_screenPos.x, input.mouse_screenPos.y) && input.mouse_stillFrameCount > 10)
    {
        node->ui_profiler.tooltip_text = "Name: "+DebugProfileRecordArray[debug.getEventSnapshot(event_id, snapshot_id)->record_id].functionName+
                                         "\nFile: "+DebugProfileRecordArray[debug.getEventSnapshot(event_id, snapshot_id)->record_id].fileName+":"+variableToStr(DebugProfileRecordArray[debug.getEventSnapshot(event_id, snapshot_id)->record_id].lineNumber)+
                                         "\n\nTotalClock: "+variableToStr((uint32)debug.getEventSnapshot(event_id, snapshot_id)->clock_time)+
                                         "cy\nHits: "+variableToStr((uint32)debug.getEventSnapshot(event_id, snapshot_id)->hitCount)+
                                         "hit\nClocks/Hit: "+variableToStr((uint32)debug.getEventSnapshot(event_id, snapshot_id)->clock_time/debug.getEventSnapshot(event_id, snapshot_id)->hitCount)+"cy/hit";
    }

    text.setCharacterSize(node->ui_profiler.frameLayerHeight*0.75);
    text.setString(DebugProfileRecordArray[debug.getEventSnapshot(event_id, snapshot_id)->record_id].functionName);
    if(text.getGlobalBounds().width+5 < rect.getSize().x)
    {
        text.setFillColor(sf::Color::White);
        text.setPosition(position+sf::Vector2f(5,0));
        window.draw(text);
    }


    sf::Vector2f child_position=position+sf::Vector2f(0,node->ui_profiler.frameLayerHeight);

    uint32 parent_id = event_id;
    for(uint32 child_id = event_id+1; child_id < kTotalRecordCount; child_id++)
    {
        // Check if not child
        if(debug.getEventSnapshot(child_id, snapshot_id)->parent!=parent_id) break;

        real32 child_width=width*((real32)debug.getEventSnapshot(child_id, snapshot_id)->clock_time/debug.getEventSnapshot(parent_id, snapshot_id)->clock_time);
        draw_DebugFrameEventBlock(child_id, parent_id, window, input, debug, node, text, child_position, child_width, layerHeight-1);
        event_id+=1;
    }

    position.x+=rect.getSize().x;
}

void draw_DebugFrameEventSlot(uint32 &event_id, uint32 snapshot_id, sf::RenderTarget &window, DebugStateInformation &debug, sf::Vector2f &position, uint32 width, uint32 height)
{
    sf::RectangleShape rect;
    rect.setPosition(position-sf::Vector2f(0,height));
    rect.setSize(sf::Vector2f(width, height));
    string name = DebugProfileRecordArray[debug.getEventSnapshot(event_id, snapshot_id)->record_id].functionName;

    if(name=="Program" || name=="Idle")
    {
        rect.setFillColor(sf::Color(20,20,20));
    }else{
        rect.setFillColor(kDebug_ColourPalatte[debug.getEventSnapshot(event_id, snapshot_id)->record_id % DEBUG_MAX_COLOURPALATTE]);
    }
    window.draw(rect);

    sf::Vector2f child_position=position;
    uint32 parent_id = event_id;
    for(uint32 child_id = event_id+1; child_id < kTotalRecordCount; child_id++)
    {
        // Check if not child
        if(debug.getEventSnapshot(child_id, snapshot_id)->parent!=parent_id) break;

        real32 child_height=height*((real32)debug.getEventSnapshot(child_id, snapshot_id)->clock_time/debug.getEventSnapshot(parent_id, snapshot_id)->clock_time);
        draw_DebugFrameEventSlot(child_id, snapshot_id, window, debug, child_position, width, child_height);
        event_id+=1;
    }
    position.y-=rect.getSize().y;
}

const sf::Texture& getDebugGraphTexture(DebugStateInformation &debug, int32 event_filter=-1)
{
    debug_frame_graph *graph;

    if(event_filter==-1)
    {
        graph = &debug.frameGraph_list[0];
    }else if(event_filter >= 0)
    {
        graph = &debug.frameGraph_list[event_filter+1];
    }



    if(graph->frameLastUpdated == 0)
    {
        graph->graph->create(debug.kDebugRecordSnapshotSize, 100);
    }

    uint64 frameDiff = min(debug.overallFrameIndex-graph->frameLastUpdated, (uint64)debug.kDebugRecordSnapshotSize);
    if(frameDiff > 0)
    {
        for(uint32 i = 0; i < frameDiff; i++)
        {
            uint32 block_id=0;
            int32 index = debug.debugSnapshotIndex-i;
            index = (index >= 0) ? index : debug.kDebugRecordSnapshotSize+index;
            if(event_filter >= 0)
            {
                for(uint32 i = 0; i < kTotalRecordCount;i++)
                {
                    if(debug.getEventSnapshot(i, index)->record_id==event_filter)
                    {
                        block_id=i;
                        break;
                    }
                }
            }

            sf::Vector2f graphPos = sf::Vector2f(index, graph->graph->getSize().y);
            draw_DebugFrameEventSlot(block_id, index, *graph->graph, debug, graphPos, 1, graph->graph->getSize().y);
        }
        graph->graph->display();
    }

    graph->frameLastUpdated=debug.overallFrameIndex;
    return graph->graph->getTexture();
}

void draw_DebugMenuNodeProfiler(sf::RenderWindow &window, InputState &input, DebugStateInformation &debug, DebugMenuNode *node, sf::Text &text, sf::Vector2f &position)
{
     TIMED_BLOCK(1, 1);
    sf::Vector2f localPosition;
    node->ui_profiler.frameSelected = debug.simulation_paused;
    node->ui_profiler.tooltip_text="";

    sf::RectangleShape frameSlot;
    frameSlot.setSize(sf::Vector2f((real32)node->ui_profiler.viewport_size.x/debug.kDebugRecordSnapshotSize, node->ui_profiler.framebar_height));
    frameSlot.setOutlineThickness(node->ui_profiler.framebar_slotOutlineWidth);
    frameSlot.setOutlineColor(node->ui_profiler.frameOutlineColour);


    for(uint32 i=0; i < debug.kDebugRecordSnapshotSize; i++)
    {
        frameSlot.setPosition(position+sf::Vector2f(i*frameSlot.getSize().x,0)+node->ui_profiler.viewportMargin);
        real32 framerate = 0;

        if(debug.debugFrameSnapshotArray[i].duration.asMicroseconds() == 0)
        {
            frameSlot.setFillColor(node->ui_profiler.frameBlankColour);
        }else if(i==debug.debugSnapshotIndex)
        {
            frameSlot.setFillColor(node->ui_profiler.frameCurrentColour);
        }else{
            framerate = 1000000/debug.debugFrameSnapshotArray[i].duration.asMicroseconds();
            if(framerate > node->ui_profiler.framerateThresholdFine)
            {
                frameSlot.setFillColor(node->ui_profiler.frameFineColour);
            }else if(framerate > node->ui_profiler.framerateThresholdWarning)
            {
                real32 t = precentDiff(node->ui_profiler.framerateThresholdFine, node->ui_profiler.framerateThresholdWarning, framerate);
                t = max(min(1.0f, t),0.0f);

                frameSlot.setFillColor(interpolate(node->ui_profiler.frameFineColour, node->ui_profiler.frameWarningColour, t));
            }else{
                real32 t = precentDiff(node->ui_profiler.framerateThresholdWarning, node->ui_profiler.framerateThresholdDanger, framerate);
                t = max(min(1.0f, t),0.0f);

                frameSlot.setFillColor(interpolate(node->ui_profiler.frameWarningColour, node->ui_profiler.frameDangerColour, t));
            }
        }

        if(frameSlot.getGlobalBounds().contains(input.mouse_screenPos.x, input.mouse_screenPos.y))
        {
            if(input.action(MOUSE_LEFTCLICK).isDown)
            {
                if(input.action(MOUSE_LEFTCLICK).state == BUTTON_PRESSED)
                {
                    debug.simulation_paused=true;
                }

                if(debug.simulation_paused)
                {
                    node->ui_profiler.frameSelected = true;
                    node->ui_profiler.frameSelected_id = i;
                }
            }
            if(input.mouse_stillFrameCount > 10)
            {
                node->ui_profiler.tooltip_text = "Frame Duration: "+variableToStr(debug.debugFrameSnapshotArray[i].duration.asSeconds()*1000)+"ms\nFPS: "+variableToStr(framerate);
            }
        }

        window.draw(frameSlot);
    }
    // Viewing slot
    uint32 selectedSlot = 0;
    if(node->ui_profiler.frameSelected)
    {
        selectedSlot = node->ui_profiler.frameSelected_id;
    }else if(debug.debugSnapshotIndex != 0)
    {
        selectedSlot = debug.debugSnapshotIndex - 1;
        node->ui_profiler.frameSelected_id=selectedSlot;
    }else{
        selectedSlot = debug.kDebugRecordSnapshotSize-1;
        node->ui_profiler.frameSelected_id=selectedSlot;
    }

    localPosition=localPosition+node->ui_profiler.viewportMargin+sf::Vector2f(0,node->ui_profiler.framebar_height);

    real32 graphHeight = (node->ui_profiler.viewport_size.y-localPosition.y)*node->ui_profiler.frameGraphHeight;

    int32 graph_filter = -1;
    DebugMenuNode* filterListNode = node->parent->getChildByName("Filter");
    if(filterListNode!=nullptr)
    {
        graph_filter = filterListNode->option_list.local_selected_id-1;
    }

    sf::Sprite graph;
    graph.setTexture(getDebugGraphTexture(debug, graph_filter));
    graph.setScale(sf::Vector2f(node->ui_profiler.viewport_size.x/graph.getTexture()->getSize().x,graphHeight/graph.getTexture()->getSize().y));
    graph.setPosition(position+localPosition);
    window.draw(graph);

    frameSlot.setPosition(position+sf::Vector2f(selectedSlot*frameSlot.getSize().x,0)+node->ui_profiler.viewportMargin);
    frameSlot.setSize(frameSlot.getSize()+sf::Vector2f(-1,graphHeight*node->ui_profiler.frameSelected));
    frameSlot.setOutlineThickness(node->ui_profiler.framebar_slotOutlineWidth);
    frameSlot.setFillColor(node->ui_profiler.frameViewingColour);
    frameSlot.setOutlineColor(node->ui_profiler.frameViewingOutlineColour);
    window.draw(frameSlot);

    localPosition=localPosition+sf::Vector2f(0,node->ui_profiler.frameContextMargin+graphHeight);

    sf::Vector2f eventBlockPosition = position+localPosition;
    sf::Text eventBlockText = text;
    node->ui_profiler.frameLayerHeight = (node->ui_profiler.viewport_size.y-eventBlockPosition.y+position.y)/(debug.debugSnapshotMaxDepth+0.5);
    node->ui_profiler.frameContextPadding = node->ui_profiler.frameLayerHeight/2;
    uint32 event_id=1;

    draw_DebugFrameEventBlock(event_id, selectedSlot, window, input, debug, node, eventBlockText, eventBlockPosition, node->ui_profiler.viewport_size.x, debug.debugSnapshotMaxDepth);

    sf::RectangleShape selector;
    selector.setSize(sf::Vector2f(node->ui_profiler.selectionSize, node->ui_profiler.selectionSize));
    selector.setFillColor(sf::Color(200,200,200));
    selector.setOrigin(node->ui_profiler.selectionSize/2.0, node->ui_profiler.selectionSize/2.0);
    selector.setPosition(position+node->ui_profiler.viewport_size);
    window.draw(selector);

    if(input.action(MOUSE_LEFTCLICK).state == BUTTON_PRESSED && selector.getGlobalBounds().contains(input.mouse_screenPos.x, input.mouse_screenPos.y))
    {
        node->ui_profiler.profilerSelected=true;
    }
    if(node->ui_profiler.profilerSelected)
    {
        node->ui_profiler.viewport_size.x = min(max(input.mouse_screenPos.x - position.x, node->ui_profiler.min_viewport_size.x), window.getSize().x - position.x);
        node->ui_profiler.viewport_size.y = min(max(input.mouse_screenPos.y - position.y, node->ui_profiler.min_viewport_size.y), window.getSize().y - position.y);

        node->ui_profiler.profilerSelected=input.action(MOUSE_LEFTCLICK).isDown;
    }

    if(node->ui_profiler.tooltip_text != "")
    {
        debug.ui.tooltipList.push_back(debug_ui_tooltip(node->ui_profiler.tooltip_text,input.mouse_screenPos));
    }

    position.y+=node->margin+node->ui_profiler.viewport_size.y;
}

void DebugMenuNode::draw(sf::RenderWindow &window, InputState &input, DebugStateInformation &debug, sf::Text &text, sf::Vector2f &position, uint32 indent)
{
    // Render Node
    switch(type)
    {
        case DEBUG_UI_NODE_SUBMENU:
        {
            draw_DebugMenuNodeSubMenu(window, input, debug, this, text, position);
        }break;
        case DEBUG_UI_NODE_ITEM_BOOL:
        {
            draw_DebugMenuNodeItemBool(window, input, debug, this, text, position);
        }break;
        case DEBUG_UI_NODE_ITEM_LIST:
        {
            draw_DebugMenuNodeItemList(window, input, debug, this, text, position);
        }break;
        case DEBUG_UI_NODE_FUNCTION:
        {
            draw_DebugMenuNodeItemFunction(window, input, debug, this, text, position);
        }break;
        case DEBUG_UI_NODE_PROFILER:
        {
            draw_DebugMenuNodeProfiler(window, input, debug, this, text, position);
        }break;
    }

    // Render Children
    position.x+=indent;
    if(type == DEBUG_UI_NODE_SUBMENU and isSubMenuOpen == true)
    {
        for(uint32 i = 0; i < children.size(); i++)
        {
            children[i]->draw(window, input, debug, text, position, indent);
            string test = text.getString();
        }
    }
    position.x-=indent;
}

sf::Vector2f DebugMenuUIState::draw(sf::RenderWindow &window, InputState &input, DebugStateInformation &debug, sf::Font &font)
{
    NAMED_BLOCK("DebugMenuState", 1, 2);
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(textSize);
    text.setPosition(position.x, position.y);
    text.setOutlineThickness(2);
    sf::Vector2f position(0,0);

    rootNode.draw(window, input, debug, text, position, indent);

    for(uint32 i=0; i < freeRoamingNodeList.size(); i++)
    {
        sf::RectangleShape selector;
        selector.setSize(sf::Vector2f(5, 5));
        selector.setFillColor(sf::Color(200,200,200));
        selector.setOrigin(selector.getSize().x/2.0, selector.getSize().y/2.0);
        selector.setPosition(freeRoamingNodeList[i].position);
        window.draw(selector);

        if(input.action(MOUSE_LEFTCLICK).state == BUTTON_PRESSED && selector.getGlobalBounds().contains(input.mouse_screenPos.x, input.mouse_screenPos.y))
        {
            freeRoamingNodeList[i].selectedByCursor=true;
        }else if(input.action(MOUSE_RIGHTCLICK).state == BUTTON_PRESSED && selector.getGlobalBounds().contains(input.mouse_screenPos.x, input.mouse_screenPos.y))
        {
            delete freeRoamingNodeList[i].node;
            freeRoamingNodeList.erase(freeRoamingNodeList.begin()+i);
            i--;
            continue;
        }

        if(freeRoamingNodeList[i].selectedByCursor)
        {
            freeRoamingNodeList[i].position.x = input.mouse_screenPos.x;
            freeRoamingNodeList[i].position.y = input.mouse_screenPos.y;
            freeRoamingNodeList[i].selectedByCursor = input.action(MOUSE_LEFTCLICK).isDown;
        }
        sf::Vector2f position = freeRoamingNodeList[i].position;
        freeRoamingNodeList[i].node->draw(window, input, debug, text, position, indent);
    }

    text.setOutlineThickness(0);
    text.setCharacterSize(14);
    uint32 padding = 10;
    for(uint32 i=0; i < tooltipList.size(); i++)
    {
        text.setPosition(sf::Vector2f(tooltipList[i].position.x+padding,tooltipList[i].position.y+padding));
        text.setString(tooltipList[i].text);
        sf::RectangleShape toolip_rect;
        toolip_rect.setFillColor(sf::Color(40,40,40,200));
        toolip_rect.setOutlineColor(sf::Color(30,30,30));
        toolip_rect.setOutlineThickness(2);
        toolip_rect.setSize(sf::Vector2f(text.getGlobalBounds().width+padding*2,text.getGlobalBounds().height+padding*2));
        toolip_rect.setPosition(tooltipList[i].position.x, tooltipList[i].position.y);
        window.draw(toolip_rect);
        window.draw(text);
    }
    tooltipList.clear();

    return position;
}

void collateDebugEventFrameData(DebugStateInformation &debug)
{
    if(!debug.simulation_paused)
    {
        if(DebugEvent_Index > 0)
        {
            uint32          current_depth=0;

            uint32          current_event_index=0;
            uint32          frame_index = debug.debugSnapshotIndex;
            stack<uint32>   parent_stack;
            parent_stack.push(-1);


            for(uint32 i = 0 ; i < kTotalRecordCount; i++)
            {
                *debug.getEventSnapshot(i, frame_index) = frame_event_block();
            }

            for(uint32 i = 0; i < DebugEvent_Index; i++)
            {
                debug_event* event = &DebugEventList[i];
                switch(event->type)
                {
                    case DEBUG_EVENT_TIMED_BLOCK_START:
                    {
                        bool alreadyExists = false;
                        for(uint32 x=0; x < current_event_index; x++)
                        {
                            if(debug.getEventSnapshot(current_event_index, frame_index)->record_id == event->record_id)
                            {
                                debug.getEventSnapshot(current_event_index, frame_index)->startClock = event->clock;
                                debug.getEventSnapshot(current_event_index, frame_index)->hitCount  += event->hitCount;
                                alreadyExists=true;

                                current_depth++;
                                parent_stack.push(current_event_index);
                                break;
                            }
                        }

                        if(!alreadyExists)
                        {
                            debug.getEventSnapshot(current_event_index, frame_index)->record_id  = event->record_id;
                            debug.getEventSnapshot(current_event_index, frame_index)->parent     = parent_stack.top();
                            debug.getEventSnapshot(current_event_index, frame_index)->startClock = event->clock;
                            debug.getEventSnapshot(current_event_index, frame_index)->hitCount  += event->hitCount;
                            debug.getEventSnapshot(current_event_index, frame_index)->depth      = current_depth;

                            current_depth++;
                            parent_stack.push(current_event_index);
                        }
                        current_event_index++;

                        if(debug.debugSnapshotMaxDepth < current_depth) debug.debugSnapshotMaxDepth = current_depth;
                    }break;

                    case DEBUG_EVENT_TIMED_BLOCK_END:
                    {
                        debug.getEventSnapshot(parent_stack.top(), frame_index)->clock_time += event->clock-debug.getEventSnapshot(parent_stack.top(), frame_index)->startClock;
                        current_depth--;
                        parent_stack.pop();
                    }break;
                }
            }
        }

        debug.debugSnapshotIndex++;
        if(debug.debugSnapshotIndex==debug.kDebugRecordSnapshotSize)
            debug.debugSnapshotIndex=0;
    }
    DebugEvent_Index=0;
    debug.overallFrameIndex++;

    DebugMenuNode* node = debug.ui.rootNode.getChildByName("System")->getChildByName("Profiler")->getChildByName("Filter");
    if(node != nullptr)
    {
        node->option_list.option[0] = "None";
        node->option_list.option.resize(kTotalRecordCount+1);
        node->option_list.option_colour.resize(kTotalRecordCount+1);

        for(uint32 i = 0; i < kTotalRecordCount; i++)
        {
            node->option_list.option[i+1] = (DebugProfileRecordArray[i].functionName);
            node->option_list.option_colour[i+1] = (kDebug_ColourPalatte[i]);
        }
    }
}

void ShowColorEdit(const char* text, sf::Color &colour, ImGuiColorEditFlags flags=0)
{
    static ImVec4 colourChange;
    colourChange = ImVec4(colour);
    ImGui::ColorEdit4(text, (float*)&colourChange, flags);
    colour = (sf::Color)colourChange;
}

void ShowDebugWindow_Map(DebugStateInformation &debug, bool *p_open)
{
    static bool p_map_open = true;
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_ShowBorders;
    //ImGui::SetNextWindowSize(ImVec2(550,680), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Map Settings", p_open, window_flags))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }

    if(ImGui::CollapsingHeader("Debug Rendering"))
    {
        static string    string_InfoText = "";
        static ImVec4    colour_InfoText;

        if(ImGui::Button("Load Colours"))
        {
            Json::Value root = readJsonFile(kDataFile_debug_colours);
            if(!loadLinearJsonIntoMemory(root, (byte*)(&debug.colour), kDataFormat_debug_colours, true))
            {
                string_InfoText = "Failed to load file!";
                colour_InfoText = ImVec4(0.75, 0.24, 0.13, 1.0);
            }else{
                string_InfoText = "File loaded successfully";
                colour_InfoText = ImVec4(0.32, 0.75, 0.19, 1.0);
            }
        }

        ImGui::SameLine();
        if(ImGui::Button("Save Colours"))
        {
            if(!saveLinearJsonFileFromMemory(kDataFile_debug_colours, (byte*)(&debug.colour), kDataFormat_debug_colours))
            {
                string_InfoText = "Failed to save to file!";
                colour_InfoText = ImVec4(0.75, 0.24, 0.13, 1.0);
            }else{
                string_InfoText = "Style saved successfully";
                colour_InfoText = ImVec4(0.32, 0.75, 0.19, 1.0);
            }
        }
        ImGui::TextColored(colour_InfoText, "%s",string_InfoText.c_str());

        ImGui::Checkbox("##Show_Room_Boundaries", &debug.display_RoomBoundaries); ImGui::SameLine();
        ShowColorEdit("Room Boundary Colour", debug.colour.roomBoundaries, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
        ImGui::TextUnformatted("Show Room Boundaries");


        ImGui::Checkbox("##Show_Room_Connections", &debug.display_RoomConnections); ImGui::SameLine();
        ShowColorEdit("Room Connection In Colour", debug.colour.roomConnectionHighlight, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
        ShowColorEdit("Room Connection Out Colour", debug.colour.roomConnections, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
        ImGui::TextUnformatted("Show Room Connections");


        ImGui::Checkbox("##Show_Room_Graph", &debug.display_RoomGraph); ImGui::SameLine();
        ShowColorEdit("Room Graph Colour", debug.colour.roomGraph, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
        ImGui::TextUnformatted("Show Room Graph");

        ImGui::Checkbox("##Show_Room_ID", &debug.display_RoomID); ImGui::SameLine();
        ShowColorEdit("Room ID Text Colour", debug.colour.roomid, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
        ImGui::TextUnformatted("Show Room ID");


        ImGui::Checkbox("##Show_Tilegrid", &debug.display_TileGrid); ImGui::SameLine();
        ShowColorEdit("Tilegrid Colour", debug.colour.tilegrid, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
        ImGui::TextUnformatted("Show Tile Grid");


        ImGui::Separator();
        ImGui::Checkbox("Show Tile IDs", &debug.display_TileID);
        ImGui::Checkbox("Show Tile AOs", &debug.display_TileAO);

    }

    ImGui::End();
}

void ShowDebugWindow_Entities(DebugStateInformation &debug, bool *p_open)
{
    static bool p_entity_open = true;
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_ShowBorders;
    //ImGui::SetNextWindowSize(ImVec2(550,680), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Entity Settings", p_open, window_flags))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }

    ImGui::End();
}

void ShowDebugWindow_Items(DebugStateInformation &debug, bool *p_open)
{
    static bool p_item_open = true;
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_ShowBorders;
    //ImGui::SetNextWindowSize(ImVec2(550,680), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Item Settings", p_open, window_flags))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }

    static uint32 selected_item  = 0;
    static bool   isItemSelected = false;
    static bool   item_changed   = false;

    if(ImGui::Button("Reload Item Config"))
    {
        debug.game_itemManager->item_list.clear();
        debug.game_itemManager->item_count=0;
        debug.game_itemManager->item_current_index=0;
        loadItemListFromConfigFile(*debug.game_itemManager, "Resources/Config/item_list.json", true);

        isItemSelected=false;
    }
    ImGui::Columns(2, NULL, true);


    ImGui::BeginChild("ItemList", ImVec2(0,300), true);

        for (map<uint32, game_item>::iterator it=debug.game_itemManager->item_list.begin(); it!=debug.game_itemManager->item_list.end(); it++)
        {
            const bool selected = (isItemSelected && it->first==selected_item);
            ImGui::PushID(it->first);
            char* buffer = new char[128];
            ImFormatString(buffer, 128, "%3d %-s", it->first, it->second.name.c_str());
            if(ImGui::Selectable(buffer, selected))
            {
                selected_item = it->first;
                isItemSelected=true;
                item_changed=true;
            }
            ImGui::PopID();
        }

    ImGui::EndChild();
    ImGui::NextColumn();
    ImGui::BeginChild("ItemPropertyEditor", ImVec2(0,300), true);
    if(isItemSelected)
    {
        static char* name_buffer = new char[128];
        static char* desc_buffer = new char[512];

        if(item_changed)
        {
            str2char(debug.game_itemManager->item_list[selected_item].name, name_buffer, 128);
            str2char(debug.game_itemManager->item_list[selected_item].description, desc_buffer, 512);
        }


        ImGui::Text("Name:");
        if(ImGui::InputText("##Name", name_buffer, 128))
        {
            debug.game_itemManager->item_list[selected_item].name=name_buffer;
        }

        ImGui::Text("Description:");
        if(ImGui::InputTextMultiline("##Description", desc_buffer, 512, ImVec2(ImGui::GetContentRegionAvailWidth(),100)))
        {
            debug.game_itemManager->item_list[selected_item].description=desc_buffer;
        }


    }else{
        ImGui::Text("Select an item to edit it.");
    }

    item_changed=false;

    ImGui::EndChild();
    ImGui::End();
}

void ShowDebugWindow_Resources(DebugStateInformation &debug, bool *p_open)
{
    static bool p_resources_open = true;
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_ShowBorders;
    //ImGui::SetNextWindowSize(ImVec2(550,680), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Resource Settings", p_open, window_flags))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }

    ImGui::End();
}

void ShowDebugWindow_System(DebugStateInformation &debug, bool *p_open)
{
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_ShowBorders;
    //ImGui::SetNextWindowSize(ImVec2(550,680), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("System Settings", p_open, window_flags))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }

    //ImGui::PlotLines("Framerate", )

    ImGui::End();
}


void ShowDebugWindow_ConfigEditor(DebugStateInformation &debug, bool *p_open)
{
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_ShowBorders;
    if (!ImGui::Begin("Config Editor", p_open, window_flags))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }

    static char* filename_buffer = new char[128]();
    ImGui::InputText("##Filename", filename_buffer, 128); ImGui::SameLine();
    static ImGuiFs::Dialog file_dialog;
    file_dialog.chooseFileDialog(ImGui::Button("..."), "./Resources/Config/", ".json;", "Pick a config file");
    strcpy(filename_buffer, file_dialog.getChosenPath());
    ImGui::SameLine();

    static string filename_loaded;
    static char* file_buffer = new char[4096]();
    static bool  needs_saved = false;

    static GUI_WarningText warningText;


    if(ImGui::Button("Load"))
    {
        string file = fastReadFile(string(filename_buffer));
        if(file.length() < 4096)
        {
            str2char(file, file_buffer, 4096);
            filename_loaded=filename_buffer;
            warningText.alert("File loaded successfully", sf::Color(0.32*255, 0.75*255, 0.19*255));
            needs_saved=false;
        }else{
            warningText.alert("File is too big!(>4Kb)", sf::Color(0.75*255, 0.24*255, 0.13*255));
        }
    }

    warningText.show();

    if(!needs_saved) ImGui::GetStyle().Alpha=0.2;

    if(ImGui::ButtonEx("Save Changes", ImVec2(0,0), ImGuiButtonFlags_Disabled&needs_saved) && needs_saved)
    {
        ofstream file_id;
        file_id.open(filename_buffer);

        file_id << string(file_buffer);

        file_id.close();

        needs_saved=false;
    }


    ImGui::GetStyle().Alpha=1.0;

    ImGuiInputTextFlags input_flags = (filename_loaded.length() != 0) ? 0 : ImGuiInputTextFlags_ReadOnly;

    ImGui::BeginChild("File Editor", ImVec2(ImGui::GetContentRegionAvailWidth(),0), true);

    if(ImGui::InputTextMultiline("##File Edit Region", file_buffer, 1024, ImVec2(ImGui::GetContentRegionAvailWidth(),ImGui::GetContentRegionAvail().y), input_flags | ImGuiInputTextFlags_AllowTabInput))
    {
        needs_saved=true;
    }

    ImGui::EndChild();




    ImGui::End();
}


void ShowDebugWindows(DebugStateInformation &debug)
{
    static bool map_window_open = false;
    static bool entity_window_open = false;
    static bool item_window_open = false;
    static bool resource_window_open = false;
    static bool system_window_open = false;

    static bool configeditor_window_open = false;

    static bool metrics_window_open = false;
    static bool style_window_open = false;
    static bool demo_window_open = false;

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Debug"))
        {
            if (ImGui::MenuItem("Map"))
            {
                map_window_open=true;
            }

            if (ImGui::MenuItem("Entities"))
            {
                 entity_window_open=true;
            }

            if (ImGui::MenuItem("Items"))
            {
                 item_window_open=true;
            }

            if (ImGui::MenuItem("Resources"))
            {
                resource_window_open=true;
            }

            if (ImGui::MenuItem("System"))
            {
                system_window_open=true;
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Tools"))
        {
            if (ImGui::MenuItem("Config Editor"))
            {
                configeditor_window_open=true;
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help"))
        {
            ImGui::MenuItem("Metrics", NULL, &metrics_window_open);
            ImGui::MenuItem("Style Editor", NULL, &style_window_open);
            ImGui::MenuItem("Demo", NULL, &demo_window_open);

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    static bool test=true;


    if(map_window_open)             ShowDebugWindow_Map(debug,       &map_window_open);
    if(entity_window_open)          ShowDebugWindow_Entities(debug,  &entity_window_open);
    if(item_window_open)            ShowDebugWindow_Items(debug,     &item_window_open);
    if(resource_window_open)        ShowDebugWindow_Resources(debug, &resource_window_open);
    if(system_window_open)          ShowDebugWindow_System(debug,    &system_window_open);

    if(configeditor_window_open)    ShowDebugWindow_ConfigEditor(debug,    &configeditor_window_open);

    if(metrics_window_open)         ImGui::ShowMetricsWindow(&metrics_window_open);
    if(style_window_open)           ImGui::ShowStyleEditor(&ImGui::GetStyle());
    if(demo_window_open)            ImGui::ShowTestWindow(&demo_window_open);
}


#endif /* end of include guard: DEBUG_UTILS_CPP */
