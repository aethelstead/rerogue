#include "gin.h"
#include <SDL2/SDL_gamecontroller.h>
#include <SDL2/SDL_keycode.h>
#include <algorithm>

namespace gin
{
    bool State::events()
    {
        // Reset map of keys from the last frame.
        input.prev_keys.clear();
        input.prev_keys.insert(input.keys.begin(), input.keys.end());

        // Reset map of (gamepad) buttons from the last frame.
        input.prev_btns.clear();
        input.prev_btns.insert(input.btns.begin(), input.btns.end());

        // Poll SDL input
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
                    input.keys.emplace(e.key.keysym.sym);
                    break;
                case SDL_KEYUP:
                    input.keys.erase(e.key.keysym.sym);   
                    break;
                case SDL_CONTROLLERBUTTONDOWN:
                    input.btns.emplace(e.cbutton.button);
                    break;
                case SDL_CONTROLLERBUTTONUP:
                    input.btns.erase(e.cbutton.button);
                    break;
            }
        }

        // Execute the lua input handler functions for keys...
        for (int key : input.keys)
        {
            bool is_repeat = input.prev_keys.count(key);
            input.L_io["key_down"](key, is_repeat);
        }
        // ... then for input
        for (int btn : input.btns)
        {
            bool is_repeat = input.prev_btns.count(btn);
            input.L_io["pad_down"](btn, is_repeat);
        }

        return result;
    }
} // namespace gin
