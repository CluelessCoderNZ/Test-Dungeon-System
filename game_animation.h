#ifndef GAME_ANIMATION_H
#define GAME_ANIMATION_H
#include "game_timer.h"
#include "game_timer.cpp"
#include "resource_manager.h"
#include "resource_manager.cpp"

class Animation
{
    public:
        enum Animation_type
        {
            ANIMATE_TEXTURE,
            ANIMATE_TWEEN_SCALE,
            ANIMATE_TWEEN_COLOUR
        };

        struct TextureFrame
        {
            resource_handle texture;
            sf::IntRect     texture_area;
            sf::Vector2u    size;
            sf::Vector2u    frame_area;
            uint32          frame_count;
        };

        struct ScaleData
        {
            sf::Vector2f start_value;
            sf::Vector2f end_value;
        };

        struct ColourData
        {
            sf::Color   start_value;
            sf::Color   end_value;
        };

        void animateTexture(resource_handle texture, real32 millisecondTime, sf::Vector2u frame_size, sf::IntRect texture_area=sf::IntRect(0,0,0,0));
        void animateTexture(string filename, real32 millisecondTime, sf::Vector2u frame_size, sf::IntRect texture_area=sf::IntRect(0,0,0,0));
        void animateScale(sf::Vector2f start_value, sf::Vector2f end_value, real32 millisecondTime);
        void animateColour(sf::Color start_value, sf::Color end_value, real32 millisecondTime);
        void animateSprite(real32 t, sf::Sprite &sprite);

        GameTimer                   timer;
        Animation::Animation_type   type;

        union
        {
            Animation::TextureFrame frame_texture;
            Animation::ScaleData    scale;
            Animation::ColourData   colour;
        };
};

#endif
