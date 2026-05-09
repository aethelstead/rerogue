#pragma once

#include <memory>
#include <functional>
#include <unordered_map>
#include <SDL2/SDL_gamecontroller.h>
#include <SDL2/SDL_keycode.h>
#include "../lua/lua.h"

namespace gin
{
    using Gamepad = std::unique_ptr<SDL_GameController, std::function<void(SDL_GameController*)>>;

    struct VKey
    {
        sol::function downFn;
        sol::function heldFn;
        sol::function upFn;

        bool down = false;
        bool prev_down = false;
    };
    
    struct InputState
    {
        Gamepad gamepad;
        std::unordered_map<int, VKey> vkeys;

        void init();
    };
}