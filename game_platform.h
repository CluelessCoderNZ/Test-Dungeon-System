#ifndef GAME_PLATFORM_H
#define GAME_PLATFORM_H

#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include "game_consts.h"
#include "game_input.h"
#include "game_input.cpp"
#include "entity.h"
#include "entity.cpp"
#include "game_map.h"
#include "game_map.cpp"
#include "game_tileset.h"
#include "game_tileset.cpp"
#include "math_utils.h"
#include "math_utils.cpp"
#include "debug_utils.h"
#include "debug_utils.cpp"
#include "game_timer.h"
#include "game_timer.cpp"
#include "game_item.cpp"

using namespace std;

struct GameState
{
    bool                    pausedGameplay       = false;
    bool                    cameraUnlocked       = false;
    bool                    cameraEntityLinked   = true;

    sf::RenderWindow        window;
    sf::View                gameview;
    sf::View                screenView;
    real32                  default_viewZoom = 0.5;
    real32                  viewZoom = 0.5;
    Entity_Reference        player;
    Entity_State_Controller entity_controller;
    GameMap                 current_map;
    Tileset                 tileset;
    ItemManager             item_manager;

    uint32                  activeSimRoom=0;

    uint32                  cameraFollowInRoomGroup=0;
    sf::FloatRect           camera_fixedView;
    Entity_Reference        camera_follow;
    sf::Vector2f            camera_unlocked_position;

    GameTimer               timer_roomTransition;
    sf::Vector2f            roomTransitionStart;
    sf::Vector2f            roomTransitionEnd;

    DebugStateInformation   debug;
};

sf::Vector2f getLockedCameraCenterPosition(sf::FloatRect cameraLockBounds, sf::Vector2f targetCenter, sf::Vector2f cameraSize);

void UpdateEntity(Entity_State_Controller &controller, GameState &state, InputState &input, Entity_Reference &ref, real32 t);
void RenderEntity(Entity_State_Controller &controller, GameState &state, sf::FloatRect &viewport, Entity_Reference &ref);
void RenderWholeMap(Entity_State_Controller &controller, GameState &state);
void RenderByTargetRoom(Entity_State_Controller &controller, GameState &state);
void GAME_UPDATE_AND_RENDER(GameState &state, InputState input, real32 t);
void CleanUpGameState(GameState &state);
void updateDebugState(GameState &state, InputState input);


#endif /* end of include guard: GAME_PLATFORM_H */
