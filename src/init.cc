#include "_core/geometry.h"
#include "game/game.h"
#include "gin.h"
#include "lua/lua.h"
#include "tilemap/tilemap.h"
#include <SDL2/SDL_video.h>
#include <unordered_map>

namespace gin
{
    bool State::init()
    {
        auto& L = get_L();
        L.do_file("scripts/global.lua");
        sol::table L_gin = L["gin"];

        gfx.init();
        input.init();
        mix.init();
        game.init(GAMEVIEW_WIDTH, GAMEVIEW_HEIGHT);

        L.do_file("scripts/main.lua");
        L_gin["init"]();


        SDL_GetWindowSize(gfx.window.get(), &gui.view.w, &gui.view.h);
        auto L_gui = L_gin["gui"].get<sol::table>();
        L_gui["toggle_pause"] = [&]()
            {
                game.is_paused = !game.is_paused;
                gui.show_pause = game.is_paused;
            };
        L_gui["toggle_overmap"] = [&]()
            {
                game.is_paused = !game.is_paused;
                gui.show_overmap = game.is_paused;
            };

        tilesets = load_tilesets();

        const auto& world_tileset = tilesets.at("world");
        game.chunks = load_chunks(world_tileset);

        gfx.init_overmap_texture(game.chunks, world_tileset);

        return true;
    }

} // namespace gin
