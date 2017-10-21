#ifndef DEBUG_UI_CPP_INCLUDED
#define DEBUG_UI_CPP_INCLUDED

#include "debug_ui.h"

void GUI_WarningText::alert(string text, sf::Color colour, uint32 duration)
{
    display_text     = text;
    display_colour   = colour;
    display_duration = duration;

    timer.restart();
}

void GUI_WarningText::show()
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


#endif // DEBUG_UI_CPP_INCLUDED
