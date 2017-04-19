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

string variableToStr(bool value)
{
    return value ? "True" : "False";
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
        debug.ui.rootNode.children[1]->children.push_back(new DebugMenuNode("Display_SelectedEntity", &debug.display_memorySelectedEntity));
        debug.ui.rootNode.children[1]->children.push_back(new DebugMenuNode("Follow_SelectedEntity", &debug.follow_memorySelectedEntity));
        debug.ui.rootNode.children[1]->children.push_back(new DebugMenuNode("OpenEntityCache", &debug.memoryAnalyzer.isEnabled));

    debug.ui.rootNode.children.push_back(new DebugMenuNode("System"));
        debug.ui.rootNode.children[2]->children.push_back(new DebugMenuNode("Display_FPS", &debug.display_FPS));

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

void DebugMenuNode::draw(sf::RenderWindow &window, InputState &input, sf::Text &text, uint32 margin, uint32 indent)
{
    // String Setup
    string textStr = display_name;
    switch(type)
    {
        case DEBUG_UI_NODE_ITEM_BOOL:
        {
            textStr = textStr+": "+variableToStr(*bool_pointer);
        }break;

        case DEBUG_UI_NODE_SUBMENU:
        {
            if (isSubMenuOpen)
            {
                textStr = "V "+textStr;
            }else{
                textStr = "> "+textStr;
            }
        }break;
    }
    text.setString(textStr);
    text.setOutlineColor(textOutlineColour);

    // Mouse Input
    if(text.getGlobalBounds().contains(input.mouse_screenPos.x, input.mouse_screenPos.y))
    {
        if(input.action(MOUSE_LEFTCLICK).state == BUTTON_PRESSED)
        {
            switch(type)
            {
                case DEBUG_UI_NODE_SUBMENU:
                {
                    isSubMenuOpen = !isSubMenuOpen;
                }break;
                case DEBUG_UI_NODE_ITEM_BOOL:
                {
                    *bool_pointer = !(*bool_pointer);
                }break;
            }
        }else{
            text.setFillColor(textHighlightColour);
        }
    }else{
        text.setFillColor(textColour);
    }
    window.draw(text);

    // Render Children
    text.setPosition(text.getPosition().x + indent, text.getPosition().y + margin+text.getGlobalBounds().height);
    if(type == DEBUG_UI_NODE_SUBMENU and isSubMenuOpen == true)
    {
        for(uint32 i = 0; i < children.size(); i++)
        {
            children[i]->draw(window, input, text, margin, indent);
            string test = text.getString();
        }
    }
    text.setPosition(text.getPosition().x - indent, text.getPosition().y);
}

sf::Vector2f DebugMenuUIState::draw(sf::RenderWindow &window, InputState &input, sf::Font &font)
{
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(textSize);
    text.setPosition(position.x, position.y);
    text.setOutlineThickness(2);

    rootNode.draw(window, input, text, margin, indent);

    return text.getPosition();
}


#endif /* end of include guard: DEBUG_UTILS_CPP */
