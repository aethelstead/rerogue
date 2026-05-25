#include <SDL2/SDL_mixer.h>
#include <print>
#include "mixer.h"
#include "../_core/fs.h"
#include "../lua/lua.h"

namespace gin
{
    void MixerState::init()
    {
        // Load all BGM in assets folder
        foreach_in_dir("assets/bgm", [&](const auto& fspath)
        {
            auto bgm = Bgm(Mix_LoadMUS(fspath.c_str()), Mix_FreeMusic);
            bgms.try_emplace(fspath, std::move(bgm));
            std::println("Loaded bgm from {}", fspath.stem().string());
        });

        // Load all SFX in assets folder
        foreach_in_dir("assets/sfx", [&](const auto& fspath)
        {
            auto sfx = Sfx(Mix_LoadWAV(fspath.c_str()), Mix_FreeChunk);
            sfxs.try_emplace(fspath, std::move(sfx));
            std::println("Loaded sfx from {}", fspath.stem().string());
        });

        // Bind Lua functions
        sol::table L_mix = ((get_L()["gin"]).get<sol::table>())["mix"];
        L_mix["play_sfx"] = [&](const char* path)
            {
                auto& sfx = sfxs.at(path);
                Mix_PlayChannel(-1, sfx.get(), 0);
            };
        L_mix["play_bgm"] = [&](const char* path)
            { 
                auto& bgm = bgms.at(path);
                Mix_PlayMusic(bgm.get(), 0);
            };
        L_mix["pause_bgm"] = [&]()
            { 
                
            };
        L_mix["resume_bgm"] = [&]()
            { 
                
            };
        L_mix["stop_bgm"] = [&]()
            { 
                
            };
        L_mix["get_master_vol"] = [&]()
            { 
                
            };
        L_mix["get_sfx_vol"] = [&]()
            { 
                
            };
        L_mix["get_bgm_vol"] = [&]()
            { 
                
            };
        L_mix["set_master_vol"] = [&]()
            { 
                
            };
        L_mix["set_sfx_vol"] = [&]()
            { 
                
            };
        L_mix["set_bgm_vol"] = [&]()
            { 
                
            };
        L_mix["mute"] = [&]()
            { 
                
            };
        L_mix["unmute"] = [&]()
            { 
                
            };
        L_mix["toggle_mute"] = [&]()
            { 
                
            };

        std::println("mixer was init.");
    }
}
