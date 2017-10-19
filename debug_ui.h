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
    void alert(string text, sf::Color colour=sf::Color::Transparent, uint32 duration=3000)
    {
        display_text     = text;
        display_colour   = colour;
        display_duration = duration;

        timer.restart();
    }

    void show()
    {
        if(timer.getElapsedTime().asMilliseconds() < display_duration)
        {
            if(display_colour==sf::Color::Transparent)
            {
                ImGui::TextUnformatted(display_text.c_str());
            }else{
                ImGui::TextColored(ImVec4(display_colour), "%s", display_text.c_str());
            }

        }else{
            if(fade_out && timer.getElapsedTime().asMilliseconds() < display_duration+fade_out_duration)
            {
                display_colour.a = 255-255*((real32)(timer.getElapsedTime().asMilliseconds()-display_duration)/fade_out_duration);
                if(display_colour==sf::Color::Transparent)
                {
                    ImGui::TextUnformatted(display_text.c_str());
                }else{
                    ImGui::TextColored(ImVec4(display_colour), "%s", display_text.c_str());
                }

            }else{
                ImGui::TextUnformatted("");
            }
        }
    }

    bool        fade_out=true;
    uint32      fade_out_duration=1000;

    string      display_text;
    sf::Color   display_colour;
    uint32      display_duration=0;
    sf::Clock   timer;
};

#endif // DEBUG_UI_H_INCLUDED
