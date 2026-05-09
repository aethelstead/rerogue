#include <print>
#include <SDL2/SDL_hints.h>
#include "input.h"

namespace gin
{
    void InputState::init()
    {
        SDL_SetHint(SDL_HINT_GAMECONTROLLER_USE_BUTTON_LABELS, "0");
        SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_JOY_CONS, "0");
        SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt");
        for (int i = 0; i < SDL_NumJoysticks(); ++i)
        {
            if (!SDL_IsGameController(i))
                continue;

            gamepad = Gamepad(SDL_GameControllerOpen(i), SDL_GameControllerClose);
            if (gamepad)
            {
                //std::println("Opened game controller: {}", SDL_GameControllerName(gamepad.get()));
            }

            // @TODO: Only one controller for now... should always be the first?
            break;
        }

        auto& L = get_L();
        sol::table L_input = L.do_file("scripts/input.lua");
        sol::table L_vkeys = L_input["bind"]();
        for (auto [idx, val] : L_vkeys.pairs())
        {
            VKey vkey;
            sol::table L_vkey = val;
            vkey.downFn = L_vkey["downFn"];
            vkey.heldFn = L_vkey["heldFn"];
            vkey.upFn = L_vkey["upFn"];

            vkeys.try_emplace(idx.as<int>(), vkey);
        }

        std::println("input was init.");
    }
}
