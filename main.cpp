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
    //gamestate.window.setFramerateLimit(60);
    //gamestate.window.setVerticalSyncEnabled(true);

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
    keybind.setKey(INPUT_CONTROL, sf::Keyboard::LControl);
    keybind.setKey(INPUT_SPACE, sf::Keyboard::Space);
    keybind.setKey(INPUT_SHIFT, sf::Keyboard::LShift);

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

    generateMapAO(gamestate.current_map, gamestate.tileset);
    generateTilesetAOSprites(gamestate.tileset, gamestate.current_map.tileSize, 7, sf::Color(0,0,0,75), sf::Color(0,0,0,0));
    initDebugState(gamestate.debug);
    gamestate.debug.memoryAnalyzer.controller = &gamestate.entity_controller;

    sf::Clock framerateTimer;
    sf::Time  frameTime;
    real32    frameSpeed=0.5;
    while(gamestate.window.isOpen())
    {
        {
            NAMED_BLOCK("Program", 1);
            {
                TIMED_BLOCK(1);
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
                GAME_UPDATE_AND_RENDER(gamestate, input, frameSpeed);
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
            }
            gamestate.debug.debugFrameSnapshotArray[gamestate.debug.debugSnapshotIndex].duration = framerateTimer.getElapsedTime();
            {
                NAMED_BLOCK("Idle", 1);
                if(framerateTimer.getElapsedTime().asMicroseconds() < 16666)
                {

                    sf::sleep(sf::microseconds(16666)-framerateTimer.getElapsedTime());
                }
                frameTime = framerateTimer.restart();
                frameSpeed = frameTime.asMicroseconds()/16666.0;
                gamestate.debug.lastRecordedFrameRate = 1000000.0 / frameTime.asMicroseconds();
            }
        }
        collateDebugEventFrameData(gamestate.debug);
    }

    CleanUpGameState(gamestate);
    return 0;
}

debug_profile_record DebugProfileRecordArray[__COUNTER__];
const uint32 kTotalRecordCount=__COUNTER__-1;
