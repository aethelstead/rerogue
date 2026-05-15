#pragma once

#include <memory>
#include <functional>
#include <set>
#include <SDL2/SDL_gamecontroller.h>
#include <SDL2/SDL_keycode.h>
#include "../lua/lua.h"

namespace gin
{
    using Gamepad = std::unique_ptr<SDL_GameController, std::function<void(SDL_GameController*)>>;
    
    struct InputState
    {
        Gamepad gamepad;
        std::set<int> prev_keys;
        std::set<int> keys;
        
        std::set<int> prev_btns;
        std::set<int> btns;

        sol::table L_io;

        void init();
    };
}