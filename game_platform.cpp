#ifndef GAME_PLATFORM_CPP
#define GAME_PLATFORM_CPP
#include "game_platform.h"

// TODO(Connor): Add a time step for updates
void UpdateEntity(Entity_State_Controller &controller, GameState &state, InputState &input, Entity_Reference &ref, real32 t)
{
    Entity entity = getEntity(controller, ref);

    if(entity.system & (uint32)ES_KEYBOARD_CONTROL)
        Entity_System_KeyboardControl(controller, t, input, ref);

    if(entity.system & (uint32)ES_BASIC_TEST_AI)
        Entity_System_BasicTestAI(controller, t, ref);

    if(entity.system & (uint32)ES_COLLISIONTILEMAP)
        Entity_System_CollisionTilemap(controller, t, state.current_map, state.tileset, ref);


    if(entity.system & (uint32)ES_PHYSICS_VELOCITY)
        Entity_System_PhysicsVelocity(controller, t, state.current_map, ref);

    // HACK(Connor): Maybe sorting should use an enum for better control
    // Update entity sort key by default
    if(entity.component & (uint32)EC_POSITION)
        ref.sort_key = ((Entity_Component_Position*)getEntityComponent(controller, ref, EC_POSITION))->position.y;
}

void RenderEntity(Entity_State_Controller &controller, GameState &state, Entity_Reference &ref)
{
    Entity entity = getEntity(controller, ref);

    if(entity.system & (uint32)ES_CIRCLE_RENDER)
        Entity_System_CircleRender(controller, &state.window, state.current_map, ref);
}

void RenderWholeMap(Entity_State_Controller &controller, GameState &state)
{
    TIMED_BLOCK(1);

    // Todo apply X restictions
    sf::IntRect pixelViewRegion = sf::IntRect(state.gameview.getCenter().x - state.gameview.getSize().x/2, state.gameview.getCenter().y - state.gameview.getSize().y/2, state.gameview.getSize().x, state.gameview.getSize().y);
    sf::FloatRect viewRegion = scaleRect(pixelViewRegion, sf::Vector2f((real32)1.0/state.current_map.tileSize.x, (real32)1.0/state.current_map.tileSize.y));
    sf::IntRect renderRegion(viewRegion.left-2, viewRegion.top-2, viewRegion.width+4, viewRegion.height+4);

    for(uint32 sortRoom_index = 0; sortRoom_index < state.current_map.room.size(); sortRoom_index++)
    {
        uint32 room_index = state.current_map.sort_list[sortRoom_index].room_id;

        if(state.current_map.room[room_index].bounds.intersects(renderRegion))
        {
            // Sort entitiy render order
            insertion_sortUpdateOrder(state.current_map.room[room_index].entity_list);
            uint32 entity_index=0;

            // Cut intersection of room region by render region
            sf::IntRect roomRenderRegion = state.current_map.room[room_index].bounds;
            roomRenderRegion.left = 0;
            roomRenderRegion.top  = 0;

            // Render seen tiles and entities
            for(uint32 y = 0; y < roomRenderRegion.height; y++)
            {
                // In back render tiles
                for(uint32 x = 0; x < roomRenderRegion.width; x++)
                {
                    TIMED_BLOCK(1);
                    MapTile tile = state.current_map.room[room_index].getTile(x, y);
                    if(state.tileset.tile[tile.tileID].isVisible && state.tileset.tile[tile.tileID].isFloor)
                    {
                        state.tileset.tile[tile.tileID].sprite.setPosition(state.current_map.tileSize.x * (x+state.current_map.room[room_index].bounds.left), state.current_map.tileSize.y * (y+state.current_map.room[room_index].bounds.top));
                        state.window.draw(state.tileset.tile[tile.tileID].sprite);

                        // Ambient Occlusion
                        for(uint32 i = 0; i < 8; i++)
                        {
                            if(tile.AO & (1 << i))
                            {
                                state.tileset.AO_RenderState.transform = sf::Transform();
                                state.tileset.AO_RenderState.transform.translate(state.current_map.tileSize.x * (x+state.current_map.room[room_index].bounds.left), state.current_map.tileSize.y * (y+state.current_map.room[room_index].bounds.top));
                                state.window.draw(state.tileset.AO_Sprites[i], state.tileset.AO_RenderState);
                            }
                        }
                    }
                }

                // Entity Render
                // HACK(Connor): Replace with a seperate sorting order system
                while(entity_index < state.current_map.room[room_index].entity_list.size() &&
                      state.current_map.room[room_index].entity_list[entity_index].sort_key <= y)
                {
                    RenderEntity(controller, state, state.current_map.room[room_index].entity_list[entity_index++]);
                }


                // In front render tiles
                for(uint32 x = 0; x < roomRenderRegion.width; x++)
                {
                    MapTile tile = state.current_map.room[room_index].getTile(x, y);
                    if(state.tileset.tile[tile.tileID].isVisible && !state.tileset.tile[tile.tileID].isFloor)
                    {
                        state.tileset.tile[tile.tileID].sprite.setPosition(state.current_map.tileSize.x * (x+state.current_map.room[room_index].bounds.left), state.current_map.tileSize.y * (y+state.current_map.room[room_index].bounds.top));
                        state.window.draw(state.tileset.tile[tile.tileID].sprite);
                    }
                }
            }
        }
    }
}

void GAME_UPDATE_AND_RENDER(GameState &state, InputState input, real32 t)
{
    TIMED_BLOCK(1);
    real32 entityUpdateT = t;
    if(state.pausedGameplay)
    {
        entityUpdateT=0;
    }


    state.window.setView(state.gameview);
    input.mouse_globalPos = state.window.mapPixelToCoords(input.mouse_screenPos);

    // Camera follow room group update
    if(getEntity(state.entity_controller, state.camera_follow).component & EC_POSITION)
    {
        state.cameraFollowInRoomGroup = state.current_map.room[((Entity_Component_Position*)getEntityComponent(state.entity_controller, state.camera_follow, EC_POSITION))->room].room_group;
    }

    // Sim Region Check
    if(getEntity(state.entity_controller, state.camera_follow).component & EC_POSITION)
    {
        state.activeSimRoom = ((Entity_Component_Position*)getEntityComponent(state.entity_controller, state.camera_follow, EC_POSITION))->room;
    }

    // Simulate sim room
    for(uint32 i = 0; i < state.current_map.room[state.activeSimRoom].entity_list.size(); i++)
    {
        NAMED_BLOCK("Entity_Update", 1);
        UpdateEntity(state.entity_controller, state, input, state.current_map.room[state.activeSimRoom].entity_list[i], entityUpdateT);
    }

    // Simulate connected rooms to sim room
    for(uint32 connect_index = 0; connect_index < state.current_map.room[state.activeSimRoom].connection.size(); connect_index++)
    {

        uint32 connectID = state.current_map.room[state.activeSimRoom].connection[connect_index].secondaryRoom.id;
        for(uint32 i = 0; i < state.current_map.room[connectID].entity_list.size(); i++)
        {
            NAMED_BLOCK("Entity_Update", 1);
            UpdateEntity(state.entity_controller, state, input, state.current_map.room[connectID].entity_list[i], entityUpdateT);
        }
    }

    // If camera entity room group changes then transition screen
    if(state.cameraEntityLinked)
    {
        if(getEntity(state.entity_controller, state.camera_follow).component & EC_POSITION && state.current_map.room[((Entity_Component_Position*)getEntityComponent(state.entity_controller, state.camera_follow, EC_POSITION))->room].room_group != state.cameraFollowInRoomGroup)
        {
            state.roomTransitionStart = getLockedCameraCenterPosition(state.camera_fixedView, localRoomPositionToScreen(state.current_map, *((Entity_Component_Position*)getEntityComponent(state.entity_controller, state.camera_follow, EC_POSITION))), state.gameview.getSize());

            state.camera_fixedView  = scaleRect(getRoomGroupBounds(state.current_map, state.current_map.room[((Entity_Component_Position*)getEntityComponent(state.entity_controller, state.camera_follow, EC_POSITION))->room].room_group), sf::Vector2f(state.current_map.tileSize.x, state.current_map.tileSize.y));
            state.camera_fixedView.height += state.tileset.tileSize.y-state.current_map.tileSize.y;
            state.camera_fixedView.top    -= state.tileset.tileSize.y-state.current_map.tileSize.y;

            // If hallway extend view to include connected doors of rooms
            if(state.current_map.room[((Entity_Component_Position*)getEntityComponent(state.entity_controller, state.camera_follow, EC_POSITION))->room].room_type==0)
            {
                state.camera_fixedView.left   -= state.current_map.tileSize.x;
                state.camera_fixedView.top    -= state.current_map.tileSize.y;
                state.camera_fixedView.width  += state.current_map.tileSize.x*2;
                state.camera_fixedView.height += state.current_map.tileSize.y*2;
            }

            state.timer_roomTransition.restart(350);
            state.timer_roomTransition.setTween(TWEEN_CUBIC_INOUT);
            state.pausedGameplay=true;
            state.cameraUnlocked=true;

            state.roomTransitionEnd  = getLockedCameraCenterPosition(state.camera_fixedView, localRoomPositionToScreen(state.current_map, *((Entity_Component_Position*)getEntityComponent(state.entity_controller, state.camera_follow, EC_POSITION))), state.gameview.getSize());

        }
        if(!state.timer_roomTransition.hasFinished())
        {
            real32 t_value = state.timer_roomTransition.getValue(t);
            state.camera_unlocked_position.x = interpolate(state.roomTransitionStart.x, state.roomTransitionEnd.x, t_value);
            state.camera_unlocked_position.y = interpolate(state.roomTransitionStart.y, state.roomTransitionEnd.y, t_value);

            if(state.timer_roomTransition.hasFinished())
            {
                state.pausedGameplay=false;
                state.cameraUnlocked=false;
            }
        }
    }

    // Viewport

    state.gameview.reset(sf::FloatRect(0, 0, state.window.getSize().x, state.window.getSize().y));
    state.screenView = state.gameview;
    state.gameview.zoom(state.viewZoom);

    if(state.cameraUnlocked)
    {
        state.gameview.setCenter(state.camera_unlocked_position);
    }
    else
    {
        state.gameview.setCenter(getLockedCameraCenterPosition(state.camera_fixedView, localRoomPositionToScreen(state.current_map, *((Entity_Component_Position*)getEntityComponent(state.entity_controller, state.camera_follow, EC_POSITION))), state.gameview.getSize()));
    }
    sf::View temp = state.gameview;
    temp.zoom(10);
    state.window.setView(state.gameview);



    RenderWholeMap(state.entity_controller, state);

    if(state.debug.isEnabled)
        updateDebugState(state, input);
}

sf::Vector2f getLockedCameraCenterPosition(sf::FloatRect cameraLockBounds, sf::Vector2f targetCenter, sf::Vector2f cameraSize)
{
    targetCenter = sf::Vector2f(min(max(targetCenter.x, cameraLockBounds.left+cameraSize.x/2), cameraLockBounds.left+cameraLockBounds.width -cameraSize.x/2),
                                min(max(targetCenter.y, cameraLockBounds.top +cameraSize.y/2), cameraLockBounds.top +cameraLockBounds.height-cameraSize.y/2));

    if(cameraSize.x > cameraLockBounds.width)
    {
        targetCenter.x = cameraLockBounds.left+cameraLockBounds.width/2;
    }

    if(cameraSize.y > cameraLockBounds.height)
    {
        targetCenter.y = cameraLockBounds.top+cameraLockBounds.height/2;
    }

    return targetCenter;
}

void CleanUpGameState(GameState &state)
{
    for(uint32 i = 0; i < state.current_map.room.size(); i++)
    {
        delete state.current_map.room[i].tilemap;
    }

    while(state.entity_controller.entity_storage.chunk.size() > 0)
    {
        state.entity_controller.entity_storage.removeChunk(0);
    }
}

void updateDebugState(GameState &state, InputState input)
{
    TIMED_BLOCK(1);

    if(input.action(INPUT_SHIFT).isDown && input.action(INPUT_SPACE).state == BUTTON_PRESSED)
    {
        state.debug.simulation_paused=!state.debug.simulation_paused;
        state.cameraEntityLinked=!state.debug.simulation_paused;
        state.cameraUnlocked=state.debug.simulation_paused;

        if(getEntity(state.entity_controller, state.camera_follow).component & EC_POSITION)
        {
            state.camera_unlocked_position = localRoomPositionToScreen(state.current_map, *(Entity_Component_Position*)getEntityComponent(state.entity_controller, state.camera_follow, EC_POSITION));
        }

        state.viewZoom = state.default_viewZoom;
    }

    if(!state.cameraEntityLinked && state.cameraUnlocked)
    {
        real32 camera_speed = input.action(INPUT_SHIFT).isDown ? state.debug.free_camera_fastspeed : state.debug.free_camera_normalspeed;
        camera_speed *= state.viewZoom;
        state.camera_unlocked_position.x += (real32)input.action(INPUT_RIGHT).isDown*camera_speed + (real32)input.action(INPUT_LEFT).isDown*-camera_speed;
        state.camera_unlocked_position.y += (real32)input.action(INPUT_DOWN).isDown*camera_speed + (real32)input.action(INPUT_UP).isDown*-camera_speed;

        real32 zoom_speed = input.action(INPUT_SHIFT).isDown ? state.debug.free_camera_zoom_fastspeed : state.debug.free_camera_zoom_normalspeed;
        state.viewZoom *= 1 - input.mouseWheel.delta*zoom_speed;
        state.viewZoom = min(max(state.viewZoom ,(real32)state.debug.free_camera_min_zoom), (real32)state.debug.free_camera_max_zoom);
    }

    state.pausedGameplay = state.debug.simulation_paused;
    // ------
    // UPDATE
    // ------
    state.debug.mouse_hovered_room_id = roomAtGlobalPos(state.current_map, sf::Vector2i(input.mouse_globalPos.x/state.current_map.tileSize.x, input.mouse_globalPos.y/state.current_map.tileSize.y));
    updateDebugMemoryAnalyzer(state.debug, state.current_map, input);

    if(state.debug.follow_memorySelectedEntity && state.debug.memoryAnalyzer.enity_isSelected)
    {
        Entity entity = getEntity(state.entity_controller, state.debug.memoryAnalyzer.selected_entity);
        if(entity.component & EC_POSITION)
        {
            state.camera_follow = state.debug.memoryAnalyzer.selected_entity;
        }
    }

    static uint32 debug_mapReloadIteration = 0;
    if(input.action(INPUT_DEBUG_ACTION_1).state == BUTTON_RELEASED)
    {
        mt19937 random_engine(time(NULL)+debug_mapReloadIteration);
        state.current_map = generateRandomGenericDungeonUsingMapFlow(random_engine, "Resources/World_Data/Room_Types/room_data.json");
        debug_mapReloadIteration++;
    }

    if(input.action(INPUT_DEBUG_ACTION_2).state == BUTTON_RELEASED)
    {
        state.debug.memoryAnalyzer.isEnabled=true;
    }

    // ------
    // RENDER
    // ------
    if(state.debug.display_TileGrid)
    {
        sf::FloatRect game_viewport(0,0, state.gameview.getSize().x, state.gameview.getSize().y);
        game_viewport.left = state.gameview.getCenter().x - game_viewport.width / 2;
        game_viewport.top  = state.gameview.getCenter().y - game_viewport.height / 2;

        for(int32 x = game_viewport.left; x < game_viewport.left + game_viewport.width; x+=state.current_map.tileSize.x)
        {
            uint32 _x = x - (x%state.current_map.tileSize.x);
            sf::Color lineColour = state.debug.colour_tilegrid;
            sf::Vertex line[] =
            {
                sf::Vertex(sf::Vector2f(_x, game_viewport.top),    lineColour),
                sf::Vertex(sf::Vector2f(_x, game_viewport.top +
                                            game_viewport.height), lineColour)
            };

            state.window.draw(line, 2, sf::Lines);
        }

        for(int32 y = game_viewport.top; y < game_viewport.top + game_viewport.height; y+=state.current_map.tileSize.y)
        {
            uint32 _y = y - (abs(y)%state.current_map.tileSize.y);
            sf::Color lineColour = state.debug.colour_tilegrid;
            sf::Vertex line[] =
            {
                sf::Vertex(sf::Vector2f(game_viewport.left, _y), lineColour),
                sf::Vertex(sf::Vector2f(game_viewport.left +
                                        game_viewport.width, _y), lineColour)
            };

            state.window.draw(line, 2, sf::Lines);
        }
    }

    if(state.debug.display_RoomBoundaries)
    {
        for(uint32 i = 0; i < state.current_map.room.size(); i++)
        {
            debugRenderRoom(&state.window, state.current_map, i);
        }
    }

    if(state.debug.display_RoomGraph)
    {
        for(uint32 i = 0; i < state.current_map.graphMap.size(); i++)
        {
            sf::Color lineColour = state.debug.colour_roomGraph;
            sf::Vertex line[] =
            {
                sf::Vertex(sf::Vector2f(state.current_map.graphMap[i].p1.point.x * state.current_map.tileSize.x, state.current_map.graphMap[i].p1.point.y * state.current_map.tileSize.y), lineColour),
                sf::Vertex(sf::Vector2f(state.current_map.graphMap[i].p2.point.x * state.current_map.tileSize.x, state.current_map.graphMap[i].p2.point.y * state.current_map.tileSize.y), lineColour)
            };

            state.window.draw(line, 2, sf::Lines);
            for(uint32 j = 1; j < state.current_map.graphMap[i].graphPath.size(); j++)
            {
                lineColour = sf::Color::White;
                drawLine(state.window,
                         sf::Vector2f(state.current_map.graphMap[i].graphPath[j].x * state.current_map.tileSize.x, state.current_map.graphMap[i].graphPath[j].y * state.current_map.tileSize.y),
                         sf::Vector2f(state.current_map.graphMap[i].graphPath[j-1].x * state.current_map.tileSize.x, state.current_map.graphMap[i].graphPath[j-1].y * state.current_map.tileSize.y),
                         4,
                         lineColour);
            }
        }
    }

    if(state.debug.display_RoomConnections)
    {
        if(state.debug.mouse_hovered_room_id >= 0)
        {
            for(uint32 i = 0; i < state.current_map.room[state.debug.mouse_hovered_room_id].connection.size(); i++)
            {
                uint32 connect_room_id = state.current_map.room[state.debug.mouse_hovered_room_id].connection[i].secondaryRoom.id;
                sf::Color lineColour = state.debug.colour_roomConnections;
                sf::Color secondColour = state.debug.colour_roomConnectionHighlight;
                sf::Vertex line[] =
                {
                    sf::Vertex(sf::Vector2f((state.current_map.room[state.debug.mouse_hovered_room_id].bounds.left + state.current_map.room[state.debug.mouse_hovered_room_id].bounds.width/2 ) * state.current_map.tileSize.x,
                                             (state.current_map.room[state.debug.mouse_hovered_room_id].bounds.top + state.current_map.room[state.debug.mouse_hovered_room_id].bounds.height/2 )* state.current_map.tileSize.y), secondColour),
                    sf::Vertex(sf::Vector2f((state.current_map.room[connect_room_id].bounds.left + state.current_map.room[connect_room_id].bounds.width/2 ) * state.current_map.tileSize.x,
                                             (state.current_map.room[connect_room_id].bounds.top + state.current_map.room[connect_room_id].bounds.height/2 )* state.current_map.tileSize.y), lineColour)
                };

                state.window.draw(line, 2, sf::Lines);
            }
        }
    }

    if(state.debug.display_memorySelectedEntity && state.debug.memoryAnalyzer.enity_isSelected)
    {
        Entity entity = getEntity(state.entity_controller, state.debug.memoryAnalyzer.selected_entity);
        if(entity.component & EC_POSITION)
        {
            sf::RectangleShape identifier;
            identifier.setSize(sf::Vector2f(10,10));
            identifier.setOrigin(identifier.getSize().x/2, identifier.getSize().y/2);
            identifier.rotate(45);
            identifier.setPosition(localRoomPositionToScreen(state.current_map, *((Entity_Component_Position*)getEntityComponent(state.entity_controller, state.debug.memoryAnalyzer.selected_entity, EC_POSITION))));
            identifier.setFillColor(sf::Color::Cyan);
            identifier.setOutlineColor(sf::Color::White);
            identifier.setOutlineThickness(2);
            state.window.draw(identifier);
        }
    }

    state.window.setView(state.screenView);
    if(state.debug.display_RoomID)
    {
        if(state.debug.mouse_hovered_room_id >= 0)
        {
            sf::Text text_room_id;
            text_room_id.setFont(state.debug.font);
            text_room_id.setFillColor(state.debug.colour_roomid);
            text_room_id.setOutlineThickness(1);
            text_room_id.setOutlineColor(sf::Color::White);
            text_room_id.setPosition(input.mouse_screenPos.x + 5, input.mouse_screenPos.y + 5);
            text_room_id.setCharacterSize(24);
            text_room_id.setString(numToStr(state.debug.mouse_hovered_room_id));
            state.window.draw(text_room_id);

            if(state.debug.display_roomDifficulty)
            {
                text_room_id.setFillColor(state.debug.colour_roomDifficulty);
                text_room_id.move(text_room_id.getLocalBounds().width + 10,0);
                text_room_id.setString(numToStr((int32)state.current_map.room[state.debug.mouse_hovered_room_id].steps_fromHomeRoom));
                state.window.draw(text_room_id);
            }
        }
    }

    state.debug.additionalInfo = "";

    if(state.debug.display_FPS)
    {
        state.debug.additionalInfo = state.debug.additionalInfo+"FPS: "+numToStr(state.debug.lastRecordedFrameRate, 1)+"\n";
    }
    if(state.debug.display_TileID)
    {
        byte tileID = 0;
        if(state.debug.mouse_hovered_room_id != -1)
        {
            sf::Vector2i localTilePos = sf::Vector2i(input.mouse_globalPos.x/state.current_map.tileSize.x, input.mouse_globalPos.y/state.current_map.tileSize.y)-sf::Vector2i(state.current_map.room[(state.debug.mouse_hovered_room_id)].bounds.left, state.current_map.room[(state.debug.mouse_hovered_room_id)].bounds.top);
            tileID = state.current_map.room[state.debug.mouse_hovered_room_id].getTile(localTilePos.x, localTilePos.y).tileID;
        }
        state.debug.additionalInfo = state.debug.additionalInfo+"TileID: "+numToStr((int32)tileID)+"\n";
    }
    if(state.debug.display_TileAO)
    {
        byte tileAO = 0;
        if(state.debug.mouse_hovered_room_id != -1)
        {
            sf::Vector2i localTilePos = sf::Vector2i(input.mouse_globalPos.x/state.current_map.tileSize.x, input.mouse_globalPos.y/state.current_map.tileSize.y)-sf::Vector2i(state.current_map.room[(state.debug.mouse_hovered_room_id)].bounds.left, state.current_map.room[(state.debug.mouse_hovered_room_id)].bounds.top);
            tileAO = state.current_map.room[state.debug.mouse_hovered_room_id].getTile(localTilePos.x, localTilePos.y).AO;
        }
        state.debug.additionalInfo = state.debug.additionalInfo+"TileAO: "+binaryToStr((int32)tileAO)+"\n";
    }

    sf::Text additionalText;
    additionalText.setPosition(state.debug.ui.draw(state.window, input, state.debug, state.debug.font));
    additionalText.setFont(state.debug.font);
    additionalText.setFillColor(state.debug.colour_AdditionalInfo);
    additionalText.setCharacterSize(16);
    additionalText.setString(state.debug.additionalInfo);
    state.window.draw(additionalText);
}

#endif /* end of include guard: GAME_PLATFORM_CPP */
