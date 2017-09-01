#ifndef GAME_ANIMATION_CPP
#define GAME_ANIMATION_CPP
#include "game_animation.h"

void Animation::animateTexture(resource_handle texture, real32 millisecondTime, sf::Vector2u frame_size, sf::IntRect texture_area)
{
    type = Animation::ANIMATE_TEXTURE;
    frame_texture.texture = texture;
    frame_texture.size = frame_size;

    if(texture_area==sf::IntRect(0,0,0,0))
    {
        sf::Vector2u texture_size = ResourceManager::instance().getTexture(texture).getSize();
        frame_texture.area = sf::IntRect(0, 0, texture_size.x, texture_size.y);
    }else{
        frame_texture.area    = texture_area;
    }
    frame_texture.frame_area = sf::Vector2u(floor(frame_texture.area.width/frame_size.x), floor(frame_texture.area.height/frame_size.y));
    frame_texture.frame_count = frame_texture.frame_area.x*frame_texture.frame_area.y;


    timer.restart(millisecondTime);
}

void Animation::animateTexture(string filename, real32 millisecondTime, sf::Vector2u frame_size, sf::IntRect texture_area)
{
    animateTexture(ResourceManager::instance().load(filename), millisecondTime, frame_size, texture_area);
}

void Animation::animateScale(sf::Vector2f start_value, sf::Vector2f end_value, real32 millisecondTime)
{
    type                = Animation::ANIMATE_TWEEN_SCALE;
    scale.start_value   = start_value;
    scale.end_value     = end_value;
    timer.restart(millisecondTime);
}

void Animation::animateColour(sf::Color start_value, sf::Color end_value, real32 millisecondTime)
{
    type                = Animation::ANIMATE_TWEEN_COLOUR;
    colour.start_value  = start_value;
    colour.end_value    = end_value;
    timer.restart(millisecondTime);
}

void Animation::animateSprite(real32 t, sf::Sprite &sprite)
{
    switch(type)
    {
        case Animation::ANIMATE_TEXTURE:
        {
            if(sprite.getTexture() == &ResourceManager::instance().getTexture(frame_texture.texture))
            {
                sprite.setTexture(ResourceManager::instance().getTexture(frame_texture.texture));
            }

            uint32 current_frame = floor(timer.getValue(t)*frame_texture.frame_count);
            sprite.setTextureRect(sf::IntRect(
                frame_texture.area.left + (current_frame%frame_texture.frame_area.x)*frame_texture.frame_size.x,
                frame_texture.area.top  + (current_frame/frame_texture.frame_area.x)*frame_texture.frame_size.y,
                frame_texture.frame_size.x,
                frame_texture.frame_size.y
            ));
        }break;
        case Animation::ANIMATE_TWEEN_SCALE:
        {
            sprite.setScale(
                interpolate(scale.start_value.x , scale.end_value.x, timer.getValue(t)),
                interpolate(scale.start_value.y , scale.end_value.y, timer.getValue(t))
            );
        }break;
        case Animation::ANIMATE_TWEEN_SCALE:
        {
            sprite.setColor(
                interpolate(colour.start_value , colour.end_value, timer.getValue(t))
            );
        }break;
    }
}

#endif
