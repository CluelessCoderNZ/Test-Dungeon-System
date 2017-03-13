 #include <iostream>
#include <SFML/Graphics.hpp>

#include "game_platform.h"
#include "game_platform.cpp"
#include "debug_utils.h"
#include "debug_utils.cpp"

using namespace std;


// Entry Point
int main(int argc, char* argv[])
{
    generateByteSizeDataOfComponents();

    GameState gamestate;
    gamestate.window.create(sf::VideoMode(1024,768), "Entity Test System");
    gamestate.window.setFramerateLimit(60);
    gamestate.window.setVerticalSyncEnabled(true);

    gamestate.entity_controller.entity_storage.addChunk();

    InputState input;
    Keybind    keybind;
    keybind.setKey(INPUT_UP,    sf::Keyboard::W);
    keybind.setKey(INPUT_LEFT,  sf::Keyboard::A);
    keybind.setKey(INPUT_DOWN,  sf::Keyboard::S);
    keybind.setKey(INPUT_RIGHT, sf::Keyboard::D);
    keybind.setKey(INPUT_DEBUG_TOGGLE, sf::Keyboard::Tilde);
    keybind.setKey(INPUT_DEBUG_ACTION_1, sf::Keyboard::Num1);
    keybind.setKey(INPUT_DEBUG_ACTION_2, sf::Keyboard::Num2);

    gamestate.tileset = loadTilesetFromFile("Resources/Graphics/World/Tileset.png", sf::Vector2u(32,32));

    uint32 startingSeed = time(NULL);
    if(argc > 1)
        startingSeed = str2uint(argv[1]);

    mt19937 random_engine(startingSeed);

    gamestate.current_map       = generateRandomGenericDungeonUsingMapFlow(random_engine, "Resources/World_Data/Room_Types/room_data.json");
    gamestate.player            = createPlayerEntity(gamestate.current_map, gamestate.entity_controller, Entity_Component_Position(sf::Vector2f(5,5), 0));


    gamestate.camera_fixedView  = scaleRect(getRoomGroupBounds(gamestate.current_map, gamestate.current_map.room[((Entity_Component_Position*)getEntityComponent(gamestate.entity_controller, gamestate.player, EC_POSITION))->room].room_group), sf::Vector2f(gamestate.current_map.tileSize.x, gamestate.current_map.tileSize.y));
    gamestate.camera_follow = gamestate.player;

    for(uint32 i = 0; i < 500; i++)
    {
        createDumbAIEntity(gamestate.current_map, gamestate.entity_controller, Entity_Component_Position(sf::Vector2f(5,5), 0));
    }

    gamestate.tileset.tile[2].isSolid               = true;
    gamestate.tileset.tile[2].isFloor               = false;

    initDebugState(gamestate.debug);
    gamestate.debug.memoryAnalyzer.controller = &gamestate.entity_controller;

    sf::Clock framerateTimer;
    while(gamestate.window.isOpen())
    {
        input = pollForKeyboardInput(input, keybind);
        input.mouse_screenPos = sf::Mouse::getPosition(gamestate.window);

        sf::Event event;
        while(gamestate.window.pollEvent(event))
        {
            switch(event.type)
            {
                case sf::Event::Closed:
                {
                    gamestate.window.close();
                }break;

                case sf::Event::MouseWheelScrolled :
                {
                    input.mouseWheel.delta = event.mouseWheelScroll.delta;
                }break;

                default:
                    break;
            }
        }


        gamestate.window.clear();
        GAME_UPDATE_AND_RENDER(gamestate, input, 1);
        gamestate.window.display();

        #ifdef DEBUG_TOGGLE
            if(input.action(INPUT_DEBUG_TOGGLE).state == BUTTON_PRESSED)
            {
                gamestate.debug.isEnabled = !gamestate.debug.isEnabled;

                // Close debug windows
                if(!gamestate.debug.isEnabled)
                {
                    gamestate.debug.memoryAnalyzer.window.close();
                }
            }
        #endif


        gamestate.debug.lastRecordedFrameRate = 1000000.0 / framerateTimer.restart().asMicroseconds();

    }

    CleanUpGameState(gamestate);
    return 0;
}
