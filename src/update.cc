#include "gin.h"

namespace gin
{
    static void update_tile_animations(Tileset& world_tileset, double dt)
    {
        for (auto& [key, td] : world_tileset.tiles)
        {
            td.framems += (dt * 1000);
            if (td.framems >= td.duration)
            {
                td.framems = 0;

                td.frameidx++;
                if (td.frameidx >= td.frames.size())
                {
                    td.frameidx = 0;
                }
            }
        }
    }

    bool State::update(double dt)
    {
        if (game.is_paused == false)
        {
            auto& world_tileset = tilesets.at("world");
            update_tile_animations(world_tileset, dt);

            game.update(dt, tilesets);
        }

        return true;
    }
} // namespace gin
