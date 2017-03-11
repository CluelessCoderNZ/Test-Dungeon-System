#ifndef GAME_INPUT_H
#define GAME_INPUT_H
#include "game_consts.h"
#include <SFML/Window.hpp>

#define INPUT_ACTION_MAXCOUNT 10
#define KEY_ACTION_MAXCOUNT 7
#define MOUSE_ACTION_MAXCOUNT 3
enum InputAction
{
    INPUT_UP,
    INPUT_DOWN,
    INPUT_LEFT,
    INPUT_RIGHT,
    INPUT_DEBUG_TOGGLE,
    INPUT_DEBUG_ACTION_1,
    INPUT_DEBUG_ACTION_2,
    MOUSE_LEFTCLICK,
    MOUSE_MIDDLECLICK,
    MOUSE_RIGHTCLICK
};

enum ButtonTransitionState
{
    BUTTON_SAME,
    BUTTON_PRESSED,
    BUTTON_RELEASED
};

struct ButtonState
{
    bool                  isDown=false;
    ButtonTransitionState state;
};

struct AnalogButtonState
{
    real32 delta = 0;
    ButtonTransitionState state;
};

struct InputState
{
    ButtonState button[INPUT_ACTION_MAXCOUNT];

    AnalogButtonState mouseWheel;

    sf::Vector2f mouse_globalPos;
    sf::Vector2i mouse_screenPos;

    ButtonState action(InputAction input)
    {
        return button[(uint16)input];
    }
};

struct Keybind
{
    sf::Keyboard::Key key[KEY_ACTION_MAXCOUNT];

    sf::Keyboard::Key getKey(InputAction input)
    {
        return key[(uint16)input];
    }

    void setKey(InputAction input, sf::Keyboard::Key newKey)
    {
        key[(uint16)input] = newKey;
    }
};


InputState pollForKeyboardInput(InputState input);


#endif /* end of include guard: GAME_INPUT_H */
