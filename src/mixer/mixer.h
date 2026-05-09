#pragma once

#include <string>
#include <memory>
#include <functional>
#include <unordered_map>
#include <SDL2/SDL_mixer.h>

namespace gin
{
    using Bgm = std::unique_ptr<Mix_Music, std::function<void(Mix_Music*)>>;
    using BgmMap = std::unordered_map<std::string, Bgm>;

    using Sfx = std::unique_ptr<Mix_Chunk, std::function<void(Mix_Chunk*)>>;
    using SfxMap = std::unordered_map<std::string, Sfx>;

    struct MixerState
    {
        BgmMap bgms;
        SfxMap sfxs;

        void init();
    };
}
