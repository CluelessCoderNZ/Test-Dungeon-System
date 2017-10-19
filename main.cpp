#include <iostream>
#include <SFML/Graphics.hpp>
#include "game_platform.h"
#include "debug_utils.h"
#include "resource_manager.h"
#include "IMGUI/imgui.h"
#include "IMGUI/imgui-SFML.h"

using namespace std;


// Entry Point
int main(int argc, char* argv[])
{
    generateByteSizeDataOfComponents();

    GameState gamestate;
    gamestate.window.create(sf::VideoMode(1024,768), "Entity Test System");

    ImGui::SFML::Init(gamestate.window);

    ImVec4* colors = ImGui::GetStyle().Colors;
    ImGui::GetStyle().FrameRounding = 4;
    ImGui::GetStyle().GrabRounding = 4;
    ImGui::GetIO().MouseDrawCursor = true;

    colors[ImGuiCol_Text]                   = ImVec4(0.31f, 0.25f, 0.24f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
    colors[ImGuiCol_ChildWindowBg]          = ImVec4(0.68f, 0.68f, 0.68f, 0.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
    colors[ImGuiCol_Border]                 = ImVec4(0.70f, 0.70f, 0.70f, 0.26f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.63f, 0.71f, 0.73f, 0.56f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.95f, 0.33f, 0.14f, 0.47f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.97f, 0.31f, 0.13f, 0.81f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.42f, 0.75f, 1.00f, 0.53f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.66f, 0.84f, 0.97f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.40f, 0.65f, 0.80f, 0.20f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.59f, 0.71f, 0.79f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.86f, 0.89f, 0.92f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.72f, 0.82f, 0.88f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.64f, 0.80f, 0.91f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.67f, 0.69f, 0.92f, 1.00f);
    colors[ImGuiCol_ComboBg]                = ImVec4(1.00f, 1.00f, 1.00f, 0.99f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.39f, 0.39f, 0.39f, 0.69f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.37f, 0.47f, 0.46f, 1.00f);
    colors[ImGuiCol_Button]                 = ImVec4(0.98f, 0.82f, 0.35f, 1.00f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.86f, 0.64f, 0.00f, 1.00f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.99f, 1.00f, 0.66f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.19f, 0.74f, 0.90f, 0.45f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.15f, 0.38f, 0.74f, 0.45f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.49f, 0.86f, 0.96f, 0.45f);
    colors[ImGuiCol_Separator]              = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.30f, 0.38f, 0.44f, 1.00f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.17f, 0.57f, 0.82f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.54f, 0.54f, 0.54f, 0.71f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.29f, 0.29f, 0.29f, 0.78f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.67f, 0.79f, 0.81f, 0.90f);
    colors[ImGuiCol_CloseButton]            = ImVec4(0.24f, 0.59f, 0.83f, 0.50f);
    colors[ImGuiCol_CloseButtonHovered]     = ImVec4(0.96f, 0.37f, 0.37f, 0.78f);
    colors[ImGuiCol_CloseButtonActive]      = ImVec4(0.71f, 0.28f, 0.28f, 0.78f);
    colors[ImGuiCol_PlotLines]              = ImVec4(0.15f, 0.61f, 0.27f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(0.91f, 0.93f, 0.56f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.47f, 0.75f, 0.28f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(0.43f, 0.66f, 0.27f, 1.00f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.00f, 0.35f, 1.00f, 0.39f);
    colors[ImGuiCol_ModalWindowDarkening]   = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);



    //gamestate.window.setFramerateLimit(60);
    //gamestate.window.setVerticalSyncEnabled(true);

    gamestate.entity_controller.entity_storage.addChunk();

    loadItemListFromConfigFile(gamestate.item_manager, "Resources/Config/item_list.json");
    gamestate.debug.game_itemManager = &gamestate.item_manager;

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

    gamestate.current_map       = generateRandomGenericDungeonUsingMapFlow(random_engine, "Resources/Config/World_Data/Room_Types/room_data.json");
    gamestate.player            = createPlayerEntity(gamestate.current_map, gamestate.entity_controller, Entity_Component_Position(sf::Vector2f(5,5), 0));

    gamestate.camera_fixedView  = scaleRect(getRoomGroupBounds(gamestate.current_map, gamestate.current_map.room[((Entity_Component_Position*)getEntityComponent(gamestate.entity_controller, gamestate.player, EC_POSITION))->room].room_group), sf::Vector2f(gamestate.current_map.tileSize.x, gamestate.current_map.tileSize.y));
    gamestate.camera_follow = gamestate.player;

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
             NAMED_BLOCK("Program", 1, 3);
            {
                 TIMED_BLOCK(1, 4);
                {
                     NAMED_BLOCK("pollForKeyboardInput", 1, 5);
                    if(input.mouse_screenPos == sf::Mouse::getPosition(gamestate.window))
                    {
                        input.mouse_stillFrameCount++;
                    }else{
                        input.mouse_stillFrameCount=0;
                    }
                    input = pollForKeyboardInput(input, keybind);
                    input.mouse_screenPos = sf::Mouse::getPosition(gamestate.window);
                }

                {
                     NAMED_BLOCK("OSEventHandling", 1, 6);
                    sf::Event event;
                    while(gamestate.window.pollEvent(event))
                    {
                        ImGui::SFML::ProcessEvent(event);
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
                }
                bool test=true;


                gamestate.window.clear();
                ImGui::SFML::Update(gamestate.window, frameTime);

                GAME_UPDATE_AND_RENDER(gamestate, input, frameSpeed);

                ImGui::SFML::Render(gamestate.window);
                {
                    NAMED_BLOCK("OSWindowDisplay", 1, 7);
                    gamestate.window.display();
                }
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
                 NAMED_BLOCK("Idle", 1, 8);
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
    delete &ResourceManager::instance();
    return 0;
}
