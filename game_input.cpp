#ifndef GAME_INPUT_CPP
#define GAME_INPUT_CPP

ButtonTransitionState getButtonTriggerState(bool oldState, bool currentState)
{
    if(oldState != currentState)
    {
        if(currentState == false)
        {
            return BUTTON_RELEASED;
        }else
        {
            return BUTTON_PRESSED;
        }
    }

    return BUTTON_SAME;
}

InputState pollForKeyboardInput(InputState old, Keybind keybind)
{
    InputState current;

    // Update key states
    for(uint16 i = 0; i < KEY_ACTION_MAXCOUNT; i++)
    {
        current.button[i].isDown = sf::Keyboard::isKeyPressed(keybind.key[i]);
    }
    current.button[(uint32)MOUSE_LEFTCLICK].isDown      = sf::Mouse::isButtonPressed(sf::Mouse::Left);
    current.button[(uint32)MOUSE_MIDDLECLICK].isDown    = sf::Mouse::isButtonPressed(sf::Mouse::Middle);
    current.button[(uint32)MOUSE_RIGHTCLICK].isDown     = sf::Mouse::isButtonPressed(sf::Mouse::Right);

    for(uint16 i = 0; i < INPUT_ACTION_MAXCOUNT; i++)
    {
        current.button[i].state  = getButtonTriggerState(old.button[i].isDown, current.button[i].isDown);
    }

    return current;
}

#endif /* end of include guard: GAME_INPUT_CPP */
