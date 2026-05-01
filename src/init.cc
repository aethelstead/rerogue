#include "geometry.h"
#include "gin.h"
#include "sol/sol.hpp"
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

namespace gin
{
    static constexpr const char* CHARSET = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

    SDL_Surface* create_surface(uint width, uint height)
    {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        uint rmask = 0xff000000;
        uint gmask = 0x00ff0000;
        uint bmask = 0x0000ff00;
        uint amask = 0x000000ff;
#else
        uint rmask = 0x000000ff;
        uint gmask = 0x0000ff00;
        uint bmask = 0x00ff0000;
        uint amask = 0xff000000;
#endif

        const int BPP = 32;
        auto* surface = SDL_CreateRGBSurface(0, width, height, BPP, rmask, gmask, bmask, amask);

        return surface;
    }

    Glyphsheet create_glyphsheet(SDL_Renderer* renderer, std::string_view font_path, int ptsize)
    {
        auto glyphsheet = Glyphsheet();

        std::string charset = CHARSET;
        uint size_sq = std::ceil(std::sqrt(charset.size()));
        glyphsheet.ptsize = ptsize;
        glyphsheet.width = size_sq * ptsize;
        glyphsheet.height = size_sq * ptsize;
        glyphsheet.ncells = charset.size();
        glyphsheet.cols = (glyphsheet.width / ptsize);
        glyphsheet.font = Font(TTF_OpenFont(font_path.data(), ptsize), TTF_CloseFont);

        auto sheet_surface = Surface(create_surface(glyphsheet.width, glyphsheet.height), SDL_FreeSurface);

        int x = 0;
        int y = 0;
        SDL_Color white{0xff, 0xff, 0xff, 0xff};
        for (int idx = 0; idx < glyphsheet.ncells; ++idx)
        {
            const char s[] = {CHARSET[idx], '\0'};
            SDL_Surface* glyph_surface = TTF_RenderUTF8_Blended(glyphsheet.font.get(), s, white);

            SDL_Rect dest = {x, y, glyphsheet.ptsize, glyphsheet.ptsize};
            SDL_BlitSurface(glyph_surface, nullptr, sheet_surface.get(), &dest);

            SDL_FreeSurface(glyph_surface);

            x += glyphsheet.ptsize;
            if (x >= glyphsheet.width)
            {
                y += glyphsheet.ptsize;
                x = 0;
            }
        }

        glyphsheet.texture = Texture(SDL_CreateTextureFromSurface(renderer, sheet_surface.get()), SDL_DestroyTexture);

        return glyphsheet;
    }

    static void foreach_directory_entry(std::string_view dir, std::function<void(std::filesystem::path fspath)> fn)
    {
        namespace fs = std::filesystem;
        for (const auto& entry : fs::directory_iterator(dir))
            if (fs::is_regular_file(entry))
                fn(entry.path());
    }

    static void load_assets(Assets& assets, SDL_Renderer* renderer)
    {
        foreach_directory_entry("assets/img", [&](const auto& fspath)
        {
            auto texture = Texture(IMG_LoadTexture(renderer, fspath.c_str()), SDL_DestroyTexture);
            assets.textures.try_emplace(fspath, std::move(texture));
        });

        /*
        @TODO: LOAD MUSIC!
        foreach_directory_entry("assets/bgm", [&](const auto& fspath)
        {
            auto bgm = Bgm(Mix_LoadMUS(fspath.c_str()), Mix_FreeMusic);
            assets.bgms.try_emplace(fspath, std::move(bgm));
        });*/

        foreach_directory_entry("assets/sfx", [&](const auto& fspath)
        {
            auto sfx = Sfx(Mix_LoadWAV(fspath.c_str()), Mix_FreeChunk);
            assets.sfxs.try_emplace(fspath, std::move(sfx));
        });
    }

    static void load_tilesets(State& state)
    {
        foreach_directory_entry("scripts/tilesets", [&](const auto& fspath)
        {
            sol::table L_tileset = state.L.do_file(fspath.c_str());
            int ncells = L_tileset.get_or("n_cells", 0);
            int cellsper = L_tileset.get_or("cells_per_row", 0);
            std::string texture_key = L_tileset.get_or<std::string>("texture_key", "");
            if (ncells == 0 || cellsper == 0)
                return;

            TileDataMap tiles;
            TileAnimationMap anim_map;
            sol::table L_tiles = L_tileset.get<sol::table>("tiles");
            for (auto [idx, val] : L_tiles.pairs())
            {
                if (!idx.is<int>() || !val.is<sol::table>())
                    return;

                sol::table L_tile = val;

                TileData td;
                td.solid = L_tile.get_or("solid", false);
                td.duration = L_tile.get_or("duration", 0);
                td.anim_key = L_tile.get_or<std::string>("anim_key", "");
                if (!td.anim_key.empty())
                {
                    anim_map.try_emplace(td.anim_key, idx.as<int>());
                }

                if (L_tile["entity_key"].valid() && L_tile["entity_key"].is<std::string>())
                {
                    td.entity_key = L_tile.get_or<std::string>("entity_key", "");
                }

                sol::table L_frames = L_tile["frames"];
                for (auto [_, fval] : L_frames.pairs())
                    td.frames.emplace_back(fval.as<int>());

                tiles.try_emplace(idx.as<int>(), td);
            }

            state.tilesets.try_emplace(fspath.stem().string(), ncells, cellsper, texture_key, tiles, anim_map);
        });
    }

    EntityId spawn_entity(State& state, std::string_view type, Vec2i tile_pos)
    {
        /*
        if (tile_pos.x < 0 || tile_pos.x > WORLD_TILES ||
            tile_pos.y < 0 || tile_pos.y > WORLD_TILES)
        {
            std::println("Cannot spawn '{}' entity outside of world bounds.", type);
            return 0;
        }*/

        sol::table L_ent = state.L.do_file(std::format("scripts/entities/{}.lua", type));
        sol::table table = L_ent["new"]();
        EntityId eid = table["eid"];
        std::string tileset_key = table["tileset_key"];
        std::string anim_key = table["anim_key"];

        Vec2i epos{tile_pos.x * TILE_PIXELS, tile_pos.y * TILE_PIXELS};
        state.game.entities.try_emplace(eid, eid, epos, type, table, anim_key);
        state.game.sprites.try_emplace(eid, tileset_key);

        return eid;
    }

    bool create_overmap_texture(State& state)
    {
        auto surface = Surface(create_surface(WORLD_CHUNKS, WORLD_CHUNKS), SDL_FreeSurface);
        uint32_t* pixels = (uint32_t*)surface->pixels;

        for (int y = 0; y < WORLD_CHUNKS; ++y)
        {
            for (int x = 0; x < WORLD_CHUNKS; ++x)
            {
                size_t idx = (y * surface->pitch / 4) + x;

                uint32_t colour = 0xff000000; // Black
                const auto& opt_chunk = state.game.chunks[x][y];
                if (opt_chunk.has_value())
                {
                    const auto& chunk = opt_chunk.value();
                    if (chunk.type == "overworld")
                        colour = 0xff006600; // Forest green
                    else if (chunk.type == "town")
                        colour = 0xff444444; // Grey
                }

                pixels[idx] = colour;
            }
        }

        state.overmap_texture = Texture(SDL_CreateTextureFromSurface(state.renderer.get(), surface.get()), SDL_DestroyTexture);

        return true;
    }

    bool init(State& state)
    {
        // Open Lua libs
        state.L.open_libraries(sol::lib::base, sol::lib::package, sol::lib::coroutine, sol::lib::string, sol::lib::math, sol::lib::table);

        // Do the init script
        state.L.do_file("scripts/init.lua");

        const auto& L_G = state.L["G"];
        L_G["push_message"] = [&](const char* msg) { state.all_messages.emplace_back(msg); };
        L_G["show_speech"] = [&](const char* msg)
        {
            state.opt_speech = msg;
            state.is_paused = true;
        };
        L_G["show_overmap"] = [&]()
        {
            state.show_overmap = !state.show_overmap;
            state.is_paused = (state.show_overmap);
        };
        L_G["play_sfx"] = [&](const char* path)
        {
            const auto& sfx = state.assets.sfxs.at(path);
            Mix_PlayChannel(-1, sfx.get(), 0);
        };
        L_G["spawn_entity"] = [&](const char* type, int tileX, int tileY) { spawn_entity(state, type, Vec2i{tileX, tileY}); };
        L_G["reap_entity"] = [&](int eid)
        {
            auto& stateful = state.game.entities.at(eid);
            stateful.reap = true;
        };
        L_G["command_entity"] = [&](EntityId eid, int command) 
            { 
                if (state.is_paused == false)
                    command_entity(eid, (EntityCommand)command, state.game);
            };
        L_G["command_player"] = [&](int command) 
        { 
            if (state.is_paused == false)
                command_entity(state.game.player_eid, (EntityCommand)command, state.game); 
        };
        L_G["set_entity_animation"] = [&](EntityId eid, const char* anim_key) { set_entity_animation(eid, anim_key, state.game); };
        L_G["set_entity_collision"] = [&](EntityId eid, bool on)
        {
            auto& entity = state.game.entities.at(eid);
            entity.is_collide = on;
        };
        L_G["face_opponent"] = [&](EntityId eid, EntityId opp_eid)
        {
            const auto& ent = state.game.entities.at(eid);
            const auto& opp_ent = state.game.entities.at(opp_eid);

            Vec2i d{
                opp_ent.pos.x - ent.pos.x,
                opp_ent.pos.y - ent.pos.y
            };
            double mag = std::sqrt((double)(d.x * d.x) + (double)(d.y * d.y));
            if (mag == 0)
                return;
            
            Vec2i ud{
                d.x / (int)mag, 
                d.y / (int)mag
            };

            if (ud.x < 1)
                command_entity(eid, EntityCommand::FaceEast, state.game);
            else if (ud.x > 1)
                command_entity(eid, EntityCommand::FaceWest, state.game);
            else if (ud.y < 1)
                command_entity(eid, EntityCommand::FaceSouth, state.game);
            else if (ud.y > 1)
                command_entity(eid, EntityCommand::FaceNorth, state.game);
        };

        // Init Window and Renderer
        std::string title = L_G["WINDOW_TITLE"];
        int ww = L_G["WINDOW_WIDTH"].get_or(0);
        int wh = L_G["WINDOW_HEIGHT"].get_or(0);
        state.window = Window(SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ww, wh, 0), SDL_DestroyWindow);
        state.renderer = Renderer(SDL_CreateRenderer(state.window.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC), SDL_DestroyRenderer);

        // Init input
        SDL_SetHint(SDL_HINT_GAMECONTROLLER_USE_BUTTON_LABELS, "0");
        SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_JOY_CONS, "0");
        SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt");
        for (int i = 0; i < SDL_NumJoysticks(); ++i)
        {
            if (!SDL_IsGameController(i))
                continue;

            state.gamepad = Gamepad(SDL_GameControllerOpen(i), SDL_GameControllerClose);
            if (state.gamepad)
            {
                std::println("Opened game controller: {}", SDL_GameControllerName(state.gamepad.get()));
            }

            // @TODO: Only one controller for now... should be the first?
            break;
        }

        sol::table L_input = state.L.do_file("scripts/input.lua");
        sol::table L_vkeys = L_input["bind"]();
        for (auto [idx, val] : L_vkeys.pairs())
        {
            VKey vkey;
            sol::table L_vkey = val;
            vkey.downFn = L_vkey["downFn"];
            vkey.heldFn = L_vkey["heldFn"];
            vkey.upFn = L_vkey["upFn"];

            state.vkeys.try_emplace(idx.as<int>(), vkey);
        }

        load_assets(state.assets, state.renderer.get());
        load_tilesets(state);

        // Init Game
        int gw = L_G["GAMEVIEW_WIDTH"].get_or(0);
        int gh = L_G["GAMEVIEW_HEIGHT"].get_or(0);
        state.game.camera = Recti(0, 0, gw, gh);
        auto pxfmt = SDL_GetWindowPixelFormat(state.window.get());
        state.game_texture = Texture(SDL_CreateTexture(state.renderer.get(), pxfmt, SDL_TEXTUREACCESS_TARGET, gw, gh), SDL_DestroyTexture);

        Vec2i chunk_start{0, 0};
        Vec2i chunk_end{WORLD_CHUNKS, WORLD_CHUNKS};
        const auto& world_tileset = state.tilesets.at("world");
        sol::table L_townchunkgen = state.L.do_file("scripts/chunkgen/town.lua");
        sol::table L_overchunkgen = state.L.do_file("scripts/chunkgen/overworld.lua");
        for (int cy = chunk_start.y; cy < chunk_end.y; ++cy)
        {
            for (int cx = chunk_start.x; cx < chunk_end.x; ++cx)
            {
                sol::table L_chunk = L_overchunkgen["genchunk"](cx, cy);

                // @TODO: Delete this
                // Add town chunks somewhere else!
                if (cx == 1 && cy == 1)
                    L_chunk = L_townchunkgen["genchunk"](cx, cy);
                else if (cx == 10 && cy == 1)
                    L_chunk = L_townchunkgen["genchunk"](cx, cy);
                else if (cx == 20 && cy == 1)
                    L_chunk = L_townchunkgen["genchunk"](cx, cy);
                else if (cx == 30 && cy == 1)
                    L_chunk = L_townchunkgen["genchunk"](cx, cy);

                WorldChunk chunk;
                chunk.type = L_chunk["type"];

                int i = 0;
                int j = 0;
                sol::table L_tiles = L_chunk["tiles"];
                for (const auto& [_, tl] : L_tiles)
                {
                    int tval = tl.as<int>();
                    chunk.tiles[i][j] = tval;
                    const auto& td = world_tileset.tiles.at(tval);
                    if (td.entity_key.has_value())
                    {
                        Vec2i tile_pos{(cx * CHUNK_TILES) + i, (cy * CHUNK_TILES) + j};
                        spawn_entity(state, td.entity_key.value(), tile_pos);
                    }

                    i++;
                    if (i >= CHUNK_TILES)
                    {
                        i = 0;
                        j++;
                    }
                }

                state.game.chunks[cx][cy] = chunk;
            }
        }

        state.game.player_eid = spawn_entity(state, "player", Vec2i{40, 40});

        auto& player = state.game.entities.at(state.game.player_eid);
        state.game.camera.x = (int)player.pos.x - (state.game.camera.w / 2);
        state.game.camera.y = (int)player.pos.y - (state.game.camera.h / 2);

        state.sys_glyphs = create_glyphsheet(state.renderer.get(), "assets/font/consola.ttf", SYS_FONT_SIZE);
        state.game_glyphs = create_glyphsheet(state.renderer.get(), "assets/font/metamorphous.ttf", GAME_FONT_SIZE);

        if (!create_overmap_texture(state))
        {
            std::println("Failed to create surface for overmap texture.");
        }

        return true;
    }

} // namespace gin
