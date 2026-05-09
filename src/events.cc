#include "gin.h"
#include <SDL2/SDL_gamecontroller.h>
#include <SDL2/SDL_keycode.h>

int keycode_to_vkey(SDL_Keycode code)
{
    int vkey = -1;

    switch (code)
    {
        case SDLK_w:
            vkey = SDL_CONTROLLER_BUTTON_DPAD_UP;
            break;
        case SDLK_s:
            vkey = SDL_CONTROLLER_BUTTON_DPAD_DOWN;
            break;
        case SDLK_a:
            vkey = SDL_CONTROLLER_BUTTON_DPAD_LEFT;
            break;
        case SDLK_d:
            vkey = SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
            break;

        case SDLK_e:
            vkey = SDL_CONTROLLER_BUTTON_A;
            break;
        case SDLK_r:
            vkey = SDL_CONTROLLER_BUTTON_B;
            break;
        case SDLK_f:
            vkey = SDL_CONTROLLER_BUTTON_X;
            break;
        case SDLK_m:
            vkey = SDL_CONTROLLER_BUTTON_Y;
            break;

        case SDLK_SPACE:
            vkey = SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
            break;
        case SDLK_LCTRL:
            vkey = SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
            break;
    }

    return vkey;
}

namespace gin
{
    bool State::events()
    {
        SDL_Event e;
        int code = -1;
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
                return false;
            else if (e.type == SDL_KEYDOWN)
            {
                code = keycode_to_vkey(e.key.keysym.sym);
                if (input.vkeys.count(code))
                    input.vkeys[code].down = true;
            }
            else if (e.type == SDL_KEYUP)
            {
                code = keycode_to_vkey(e.key.keysym.sym);
                if (input.vkeys.count(code))
                    input.vkeys[code].down = false;
            }
            else if (e.type == SDL_CONTROLLERBUTTONDOWN)
            {
                code = e.cbutton.button;
                if (input.vkeys.count(code))
                    input.vkeys[code].down = true;
            }
            else if (e.type == SDL_CONTROLLERBUTTONUP)
            {
                code = e.cbutton.button;
                if (input.vkeys.count(code))
                    input.vkeys[code].down = false;
            }
        }

        for (auto& [idx, vkey] : input.vkeys)
        {
            if (vkey.down && !vkey.prev_down)
                vkey.downFn();
            if (vkey.down && vkey.prev_down)
                vkey.heldFn();
            else if (!vkey.down && vkey.prev_down)
                vkey.upFn();

            vkey.prev_down = vkey.down;
        }

        return true;
    }
} // namespace gin
