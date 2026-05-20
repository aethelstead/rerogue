#include "gin.h"
#include <SDL2/SDL_gamecontroller.h>
#include <SDL2/SDL_keycode.h>
#include <algorithm>

namespace gin
{
    bool State::events()
    {
        // Copy all of the down keys from the last frame
        //input.prev_keys.clear();
        //input.prev_keys.insert(input.keys.begin(), input.keys.end());
        //input.keys.clear();

        bool result = true;
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0)
        {
            switch (e.type)
            {
                case SDL_QUIT:
                    result = false;
                    break;
                case SDL_KEYDOWN:
                    //input.keys.emplace(e.key.keysym.sym);
                    input.L_io["key_down"](e.key.keysym.sym, false);
                    break;
                case SDL_KEYUP:
                    input.L_io["key_up"](e.key.keysym.sym);
                    break;
                case SDL_CONTROLLERBUTTONDOWN:
                    input.L_io["pad_down"](e.cbutton.button, false);
                    break;
                case SDL_CONTROLLERBUTTONUP:
                    break;
            }
        }

        return result;
    }
} // namespace gin
