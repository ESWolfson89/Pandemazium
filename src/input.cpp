// Copyright Eric Wolfson 2016-2017
// See LICENSE.txt (GPLv3)

#include "input.h"

input::input()
{
    delta.set(0,0);
}

void input::pollEvent()
{
    while(SDL_PollEvent(&evt) != 0)
    {
        if (evt.type == SDL_KEYDOWN)
        if (evt.key.repeat == 0)
        {
            if (evt.key.keysym.sym == SDLK_a)
                pickup_key_pressed = true;
            if (evt.key.keysym.sym == SDLK_v)
                use_level_feature_pressed = true;
            if (evt.key.keysym.sym == SDLK_z)
                off_ladder_key_pressed = true;
            if (evt.key.keysym.sym == SDLK_SPACE)
                toggle_carry_item_pressed = true;
            if (evt.key.keysym.sym == SDLK_p)
                pause_key_pressed = true;
            if (evt.key.keysym.sym == SDLK_EQUALS)
                plus_key_pressed = true;
            if (evt.key.keysym.sym == SDLK_MINUS)
                minus_key_pressed = true;
        }
    }
}

void input::processKey()
{
    delta_key_pressed = jump_key_pressed = pickup_key_pressed = fire_key_pressed = quit_key_pressed = false;
    on_ladder_key_pressed = off_ladder_key_pressed = down_ladder_key_pressed = use_level_feature_pressed = false;
    select_key_pressed = pause_key_pressed = shift_key_pressed = toggle_carry_item_pressed = false;
    plus_key_pressed = minus_key_pressed = false;

    pollEvent();

    const Uint8 *key_states = SDL_GetKeyboardState( NULL );

    if (key_states[SDL_SCANCODE_RETURN])
    {
       select_key_pressed = true;
    }
    if (key_states[SDL_SCANCODE_ESCAPE])
    {
       quit_key_pressed = true;
    }
    if (key_states[SDL_SCANCODE_Z])
    {
       jump_key_pressed = true;
    }
    if (key_states[SDL_SCANCODE_X])
    {
       fire_key_pressed = true;
    }
    if (key_states[SDL_SCANCODE_LSHIFT])
    {
       shift_key_pressed = true;
    }
    if (key_states[SDL_SCANCODE_LEFT])
    {
       setDelta(point(-1.0,0.0));
       return;
    }
    if (key_states[SDL_SCANCODE_RIGHT])
    {
       setDelta(point(1.0,0.0));
       return;
    }
    if (key_states[SDL_SCANCODE_UP])
    {
       if (!off_ladder_key_pressed)
           on_ladder_key_pressed = true;
       setDelta(point(0.0,-1.0));
    }
    if (key_states[SDL_SCANCODE_DOWN])
    {
       if (!off_ladder_key_pressed)
           down_ladder_key_pressed = true;
       setDelta(point(0.0,1.0));
    }
}

void input::setDelta(point p)
{
    delta_key_pressed = true;
    delta.set(p.x(),p.y());
}

point input::getDelta()
{
    return delta;
}

bool input::toggleCarryItemKeyPressed()
{
    return toggle_carry_item_pressed;
}

bool input::shiftKeyPressed()
{
    return shift_key_pressed;
}

bool input::pickupKeyPressed()
{
    return pickup_key_pressed;
}

bool input::jumpKeyPressed()
{
    return jump_key_pressed;
}

bool input::deltaKeyPressed()
{
    return delta_key_pressed;
}

bool input::fireKeyPressed()
{
    return fire_key_pressed;
}

bool input::onLadderKeyPressed()
{
    return on_ladder_key_pressed;
}

bool input::offLadderKeyPressed()
{
    return off_ladder_key_pressed;
}

bool input::useLevelFeaturePressed()
{
    return use_level_feature_pressed;
}

bool input::downLadderKeyPressed()
{
    return down_ladder_key_pressed;
}

bool input::selectKeyPressed()
{
    return select_key_pressed;
}

bool input::quitKeyPressed()
{
    return quit_key_pressed;
}

bool input::pauseKeyPressed()
{
    return pause_key_pressed;
}

bool input::plusKeyPressed()
{
    return plus_key_pressed;
}

bool input::minusKeyPressed()
{
    return minus_key_pressed;
}

bool input::noKeyPressed()
{
    return !(jump_key_pressed || delta_key_pressed);
}
