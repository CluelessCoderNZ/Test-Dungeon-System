#ifndef DEBUG_UI_H_INCLUDED
#define DEBUG_UI_H_INCLUDED
#include "IMGUI/imgui.h"
#include "IMGUI/imgui-SFML.h"
#include "IMGUI/imgui_internal.h"
#include <SFML/System.hpp>
#include "game_consts.h"

class GUI_WarningText
{
    public:
    void alert(string text, sf::Color colour=sf::Color::Transparent, uint32 duration=3000);

    void show();

    bool        fade_out=true;
    uint32      fade_out_duration=1000;

    string      display_text;
    sf::Color   display_colour;
    uint32      display_duration=0;
    sf::Clock   timer;
};

#endif // DEBUG_UI_H_INCLUDED
