#include "_core/geometry.h"
#include "game/game.h"
#include "gin.h"
#include "tilemap/tilemap.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_blendmode.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <array>
#include <climits>
#include <cstdint>
#include <filesystem>
#include <format>
#include <functional>
#include <memory>
#include <optional>
#include <print>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <cmath>
#include "lua/lua.h"

namespace gin
{
    bool State::init()
    {
        auto& L = get_L();
        L.do_file("scripts/global.lua");
        std::println("Did global.lua");


        sol::table L_G = L["G"];
        int ww = L_G["WINDOW_WIDTH"].get_or(0);
        int wh = L_G["WINDOW_HEIGHT"].get_or(0);
        int gw = L_G["GAMEVIEW_WIDTH"].get_or(0);
        int gh = L_G["GAMEVIEW_HEIGHT"].get_or(0);

        gfx.init();
        input.init();
        mix.init();
        game.init(gw, gh);

        gui.view.w = ww;
        gui.view.h = wh;

        L_G["toggle_overmap"] = [&]()
        {
            gui.show_overmap = !gui.show_overmap;
            game.is_paused = (gui.show_overmap);
        };
        L_G["toggle_paused"] = [&]()
        {
            game.is_paused = !game.is_paused;
        };
        /*
        const auto& L_G = L["G"];
        L_G["push_message"] = [&](const char* msg) { all_messages.emplace_back(msg); };
        L_G["show_speech"] = [&](const char* msg)
        {
            opt_speech = msg;
            game.is_paused = true;
        };
        L_G["play_sfx"] = [&](const char* path)
        {
            const auto& sfx = assets.sfxs.at(path);
            Mix_PlayChannel(-1, sfx.get(), 0);
        };*/
        L_G["spawn_entity"] = [&](const char* archetype, int tileX, int tileY) { game.spawn_entity(Vec2i{tileX, tileY}, archetype); };
        L_G["spawn_player"] = [&](int tileX, int tileY) 
        { 
            if (game.player_eid == 0)
            {
                game.player_eid = game.spawn_entity(Vec2i{tileX, tileY}, "player");
            }
            else
            {
                std::println("The one-and-only player entity has already been spawned.");
            }
        };
        /*L_G["reap_entity"] = [&](int eid)
        {
            auto& stateful = game.entities.at(eid);
            stateful.reap = true;
        };*/
        L_G["command_entity"] = [&](EntityId eid, int command) 
            { 
                if (game.is_paused == false)
                {
                    auto& stateful = game.statefuls.at(eid);
                    stateful.command = (EntityCommand)command;
                }
            };
        L_G["command_player"] = [&](int command) 
        { 
            if (game.is_paused == false)
            {
                auto& stateful = game.statefuls.at(game.player_eid);
                stateful.command = (EntityCommand)command;
            }
        };

        tilesets = load_tilesets();

        const auto& world_tileset = tilesets.at("world");
        game.chunks = load_chunks(world_tileset);
        
        gfx.init_overmap_texture(game.chunks, world_tileset);

        L.do_file("scripts/game.lua");
        L_G["on_init"]();

        return true;
    }

} // namespace gin
