#include <print>
#include "mixer.h"
#include "../_core/fs.h"

namespace gin
{
    void MixerState::init()
    {
        foreach_in_dir("assets/bgm", [&](const auto& fspath)
        {
            auto bgm = Bgm(Mix_LoadMUS(fspath.c_str()), Mix_FreeMusic);
            bgms.try_emplace(fspath, std::move(bgm));
            std::println("Loaded bgm from {}", fspath.stem().string());
        });

        foreach_in_dir("assets/sfx", [&](const auto& fspath)
        {
            auto sfx = Sfx(Mix_LoadWAV(fspath.c_str()), Mix_FreeChunk);
            sfxs.try_emplace(fspath, std::move(sfx));
            std::println("Loaded sfx from {}", fspath.stem().string());
        });

        std::println("mixer was init.");
    }
}
