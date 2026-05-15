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
                std::println("Opened game controller: {}", SDL_GameControllerName(gamepad.get()));
            }

            // @TODO: Only one controller for now... should always be the first?
            break;
        }

        auto L_gin = get_L()["gin"].get<sol::table>();
        L_io = L_gin["io"].get<sol::table>();

        std::println("input was init.");
    }
}
