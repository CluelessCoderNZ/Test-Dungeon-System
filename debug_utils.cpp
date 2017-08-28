#ifndef DEBUG_UTILS_CPP
#define DEBUG_UTILS_CPP
#include "debug_utils.h"
#include <iostream>


using namespace std;

string numToStr(real32 value, int32 sf)
{
    stringstream iss;
    if(sf > 0)
    {
        iss << fixed << setprecision(sf) << value;
    }else{
        iss << value;
    }
    return iss.str();
}

string numToStr(int32 value)
{
    stringstream iss;
    iss << value;
    return iss.str();
}

string numToStr(int64 value)
{
    stringstream iss;
    iss << value;
    return iss.str();
}

string numToStr(uint32 value)
{
    stringstream iss;
    iss << value;
    return iss.str();
}

string numToStr(uint64 value)
{
    stringstream iss;
    iss << value;
    return iss.str();
}

string binaryToStr(byte value)
{
    bitset<8> b(value);
    return b.to_string();
}

string variableToStr(sf::Vector2u value)
{
    return "("+numToStr((int32)value.x)+", "+numToStr((int32)value.y)+")";
}

string variableToStr(sf::Vector2f value)
{
    return "("+numToStr((real32)value.x, 2)+", "+numToStr((real32)value.y, 2)+")";
}

string variableToStr(sf::Vector2i value)
{
    return "("+numToStr((int32)value.x)+", "+numToStr((int32)value.y)+")";
}

string variableToStr(sf::IntRect value)
{
    return "("+numToStr((int32)value.top)+", "+numToStr((int32)value.left)+", "+numToStr((int32)value.width)+", "+numToStr((int32)value.height)+")";
}

string variableToStr(sf::FloatRect value)
{
    return "("+numToStr((real32)value.top)+", "+numToStr((real32)value.left)+", "+numToStr((real32)value.width)+", "+numToStr((real32)value.height)+")";
}

string variableToStr(sf::Color value)
{
    return "rgb("+numToStr((int32)value.r)+", "+numToStr((int32)value.g)+", "+numToStr((int32)value.b)+")";
}

string variableToStr(uint32 value)
{
    return numToStr((int32)value);
}

string variableToStr(real32 value)
{
    return numToStr(value);
}

string variableToStr(int32 value)
{
    return numToStr(value);
}

string variableToStr(int64 value)
{
    return numToStr(value);
}

string variableToStr(uint64 value)
{
    return numToStr(value);
}

string variableToStr(bool value)
{
    return value ? "True" : "False";
}

void copyDebugMenuNode(DebugMenuNode *src, DebugMenuNode *dest)
{
    *dest = *src;
    dest->children.clear();

    for(uint32 i = 0; i < src->children.size(); i++)
    {
        dest->children.push_back(new DebugMenuNode());
        copyDebugMenuNode(src->children[i], dest->children[i]);
    }
}

void clearDebugRoamingMenuNodeList(DebugStateInformation &debug)
{
    for(uint32 i = 0; i < debug.ui.freeRoamingNodeList.size(); i++)
    {
        delete debug.ui.freeRoamingNodeList[i].node;
    }
    debug.ui.freeRoamingNodeList.clear();
}

void initDebugState(DebugStateInformation &debug)
{
    debug.font.loadFromFile("Resources/Fonts/Debug.woff");

    // Initalize UI State
    debug.ui.rootNode.children.push_back(new DebugMenuNode("Map"));
        debug.ui.rootNode.children[0]->children.push_back(new DebugMenuNode("Room"));
            debug.ui.rootNode.children[0]->children[0]->children.push_back(new DebugMenuNode("Display_TileID", &debug.display_TileID));
            debug.ui.rootNode.children[0]->children[0]->children.push_back(new DebugMenuNode("Display_TileAO", &debug.display_TileAO));
        debug.ui.rootNode.children[0]->children.push_back(new DebugMenuNode("Display_RoomBoundaries", &debug.display_RoomBoundaries));
        debug.ui.rootNode.children[0]->children.push_back(new DebugMenuNode("Display_RoomGraph", &debug.display_RoomGraph));
        debug.ui.rootNode.children[0]->children.push_back(new DebugMenuNode("Display_TileGrid", &debug.display_TileGrid));
        debug.ui.rootNode.children[0]->children.push_back(new DebugMenuNode("Display_RoomID", &debug.display_RoomID));
        debug.ui.rootNode.children[0]->children.push_back(new DebugMenuNode("Display_RoomConnections", &debug.display_RoomConnections));

    debug.ui.rootNode.children.push_back(new DebugMenuNode("Entity"));
        debug.ui.rootNode.children[1]->children.push_back(new DebugMenuNode("Entity Factory"));
            debug.ui.rootNode.children[1]->children[0]->children.push_back(new DebugMenuNode("Type", {}, &debug.user_clickToPlaceEntity_id));
            for(uint32 i = 0; i < kEntityTypeCount; i++)
            {
                debug.ui.rootNode.children[1]->children[0]->children[0]->option_list.option.push_back(kEntityTypeString[i]);
            }
            debug.ui.rootNode.children[1]->children[0]->children.push_back(new DebugMenuNode("Click To Add", &debug.user_clickToPlaceEntity));
        debug.ui.rootNode.children[1]->children.push_back(new DebugMenuNode("Display_SelectedEntity", &debug.display_memorySelectedEntity));
        debug.ui.rootNode.children[1]->children.push_back(new DebugMenuNode("Follow_SelectedEntity", &debug.follow_memorySelectedEntity));
        debug.ui.rootNode.children[1]->children.push_back(new DebugMenuNode("OpenEntityCache", &debug.memoryAnalyzer.isEnabled));

    debug.ui.rootNode.children.push_back(new DebugMenuNode("System"));
        debug.ui.rootNode.children[2]->children.push_back(new DebugMenuNode("Profiler"));
            debug.ui.rootNode.children[2]->children[0]->children.push_back(new DebugMenuNode("Paused", &debug.simulation_paused));
            debug.ui.rootNode.children[2]->children[0]->children.push_back(new DebugMenuNode(DEBUG_UI_NODE_PROFILER));
        debug.ui.rootNode.children[2]->children.push_back(new DebugMenuNode("Display_FPS", &debug.display_FPS));
    debug.ui.rootNode.children.push_back(new DebugMenuNode("UI"));
        debug.ui.rootNode.children[3]->children.push_back(new DebugMenuNode("ClearDebugNodes", &clearDebugRoamingMenuNodeList));
}

void updateDebugMemoryAnalyzer(DebugStateInformation &debug, GameMap &map, InputState &input)
{
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

void collateDebugEventFrameData(DebugStateInformation &debug)
{
    if(!debug.simulation_paused)
    {
        if(DebugEvent_Index > 0)
        {
            uint32 current_depth=1;

            frame_event_summary* frame = &debug.debugEventSnapshotArray[debug.debugSnapshotIndex];
            *frame = frame_event_summary();
            frame_event_block* target = &frame->main_event;
            target->record_id  = DebugEventList[0].record_id;
            target->startClock = DebugEventList[0].clock;
            target->hitCount  += DebugEventList[0].hitCount;

            for(uint32 i = 1; i < DebugEvent_Index; i++)
            {
                debug_event* event = &DebugEventList[i];
                switch(event->type)
                {
                    case DEBUG_EVENT_TIMED_BLOCK_START:
                    {
                        bool alreadyExists = false;
                        for(uint32 x=0; x < target->children.size(); x++)
                        {
                            if(target->children[x]->record_id == event->record_id)
                            {
                                target = target->children[x];
                                target->startClock = event->clock;
                                target->hitCount  += event->hitCount;
                                alreadyExists=true;

                                current_depth++;
                                break;
                            }
                        }

                        if(!alreadyExists)
                        {
                            frame_event_block* new_target = new frame_event_block();
                            target->children.push_back(new_target);
                            new_target->parent = target;
                            target = new_target;

                            target->record_id  = event->record_id;
                            target->startClock = event->clock;
                            target->hitCount  += event->hitCount;

                            current_depth++;
                        }
                    }break;

                    case DEBUG_EVENT_TIMED_BLOCK_END:
                    {
                        target->clock_time += event->clock-target->startClock;
                        if(target->parent != nullptr)
                        {
                            target = target->parent;
                            current_depth--;
                        }
                    }break;
                }
                if(current_depth > frame->max_depth)
                    frame->max_depth = current_depth;
            }
        }

        debug.debugSnapshotIndex++;
        if(debug.debugSnapshotIndex==debug.kDebugRecordSnapshotSize)
            debug.debugSnapshotIndex=0;
    }
    DebugEvent_Index=0;
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
                node->function_pointer(debug);
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
    assert(node->option_list.selected_id!=nullptr);
    real32 height=0;
    string textStr = node->display_name+": "+node->option_list.option[*node->option_list.selected_id];

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

    text.move(text.getGlobalBounds().width, 0);
    text.setFillColor(node->option_list.selectedColour);
    text.setString(node->option_list.option[*node->option_list.selected_id]);
    window.draw(text);
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
            }else{
                text.setFillColor(node->textColour);
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

void draw_DebugFrameEventBlock(frame_event_block *block, sf::RenderWindow &window, InputState &input, DebugStateInformation &debug, DebugMenuNode *node, sf::Text &text, sf::Vector2f &position, uint32 width, uint32 layerHeight)
{
    sf::RectangleShape rect;
    rect.setPosition(position);
    rect.setSize(sf::Vector2f(width, layerHeight*node->ui_profiler.frameLayerHeight+node->ui_profiler.frameContextPadding));
    rect.setFillColor(kDebug_ColourPalatte[block->record_id % DEBUG_MAX_COLOURPALATTE]);
    rect.setOutlineColor(node->ui_profiler.frameContextOutlineColour);
    rect.setOutlineThickness(node->ui_profiler.frameContextOutlineWidth);
    window.draw(rect);


    text.setCharacterSize(node->ui_profiler.frameLayerHeight*0.75);
    text.setString(DebugProfileRecordArray[block->record_id].functionName+":"+variableToStr(DebugProfileRecordArray[block->record_id].lineNumber));
    if(text.getGlobalBounds().width+5 < rect.getSize().x)
    {
        text.setFillColor(sf::Color::White);
        text.setPosition(position+sf::Vector2f(5,0));
        window.draw(text);
    }

    sf::Vector2f child_position=position+sf::Vector2f(0,node->ui_profiler.frameLayerHeight);
    for(uint32 i = 0; i < block->children.size(); i++)
    {
        real32 child_width=width*((real32)block->children[i]->clock_time/block->clock_time);
        draw_DebugFrameEventBlock(block->children[i], window, input, debug, node, text, child_position, child_width, layerHeight-1);
    }
    position.x+=rect.getSize().x;
}

void draw_DebugMenuNodeProfiler(sf::RenderWindow &window, InputState &input, DebugStateInformation &debug, DebugMenuNode *node, sf::Text &text, sf::Vector2f &position)
{
    node->ui_profiler.frameSelected = debug.simulation_paused;

    sf::RectangleShape frameSlot;
    frameSlot.setSize(sf::Vector2f((real32)node->ui_profiler.viewport_size.x/debug.kDebugRecordSnapshotSize, node->ui_profiler.framebar_height));
    frameSlot.setOutlineThickness(node->ui_profiler.framebar_slotOutlineWidth);
    frameSlot.setOutlineColor(node->ui_profiler.frameOutlineColour);


    for(uint32 i=0; i < debug.kDebugRecordSnapshotSize; i++)
    {
        frameSlot.setPosition(position+sf::Vector2f(i*frameSlot.getSize().x,0)+node->ui_profiler.viewportMargin);


        if(debug.debugFrameSnapshotArray[i].duration.asMicroseconds() == 0)
        {
            frameSlot.setFillColor(node->ui_profiler.frameBlankColour);
        }else if(i==debug.debugSnapshotIndex)
        {
            frameSlot.setFillColor(node->ui_profiler.frameCurrentColour);
        }else{
            real32 framerate = 1000000/debug.debugFrameSnapshotArray[i].duration.asMicroseconds();

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

        if(input.action(MOUSE_LEFTCLICK).isDown && frameSlot.getGlobalBounds().contains(input.mouse_screenPos.x, input.mouse_screenPos.y))
        {
            node->ui_profiler.frameSelected = true;
            debug.simulation_paused = true;
            node->ui_profiler.frameSelected_id = i;
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

    frameSlot.setPosition(position+sf::Vector2f(selectedSlot*frameSlot.getSize().x,0)+node->ui_profiler.viewportMargin);
    frameSlot.setSize(frameSlot.getSize()-sf::Vector2f(1,0));
    frameSlot.setOutlineThickness(node->ui_profiler.framebar_slotOutlineWidth);
    frameSlot.setFillColor(node->ui_profiler.frameViewingColour);
    frameSlot.setOutlineColor(node->ui_profiler.frameViewingOutlineColour);
    window.draw(frameSlot);


    sf::Vector2f eventBlockPosition = position+node->ui_profiler.viewportMargin+sf::Vector2f(0,node->ui_profiler.frameContextMargin+node->ui_profiler.framebar_height);
    sf::Text eventBlockText = text;
    node->ui_profiler.frameLayerHeight = (node->ui_profiler.viewport_size.y-eventBlockPosition.y+position.y)/(debug.debugEventSnapshotArray[selectedSlot].max_depth+0.5);
    node->ui_profiler.frameContextPadding = node->ui_profiler.frameLayerHeight/2;

    draw_DebugFrameEventBlock(&debug.debugEventSnapshotArray[selectedSlot].main_event, window, input, debug, node, eventBlockText, eventBlockPosition, node->ui_profiler.viewport_size.x, debug.debugEventSnapshotArray[selectedSlot].max_depth);


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

    return position;
}


#endif /* end of include guard: DEBUG_UTILS_CPP */
