#include <print>
#include "tilemap.h"
#include "../_core/fs.h"
#include "../_core/geometry.h"
#include "../lua/lua.h"

namespace gin
{
    TilesetMap load_tilesets()
    {
        TilesetMap tilesets;

        auto& L = get_L();
        foreach_in_dir("scripts/tilesets", [&](const auto& fspath)
        {
            sol::table L_tileset = L.do_file(fspath.c_str());
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
                td.is_wall = L_tile.get_or("is_wall", false);
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

            tilesets.try_emplace(fspath.stem().string(), ncells, cellsper, texture_key, tiles, anim_map);
            std::println("Loaded tileset from {}", fspath.stem().string());
        });

        return tilesets;
    }

    ChunkMap load_chunks(const Tileset& world_tileset)
    {
        ChunkMap chunks;
        auto& L = get_L();

        Vec2i chunk_start{0, 0};
        Vec2i chunk_end{WORLD_CHUNKS, WORLD_CHUNKS};
        sol::table L_world = L.do_file("scripts/chunkgen/world.lua");
        L_world["gen_world"]();
        sol::table L_chunks = L_world["chunks"];
        for (int cy = chunk_start.y; cy < chunk_end.y; ++cy)
        {
            for (int cx = chunk_start.x; cx < chunk_end.x; ++cx)
            {
                int idx = (cy * WORLD_CHUNKS) + cx;
                sol::table L_chunk = L_chunks[idx + 1];

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
                        //game.spawn_entity(tile_pos, td.entity_key.value());
                    }

                    i++;
                    if (i >= CHUNK_TILES)
                    {
                        i = 0;
                        j++;
                    }
                }

                chunks[cx][cy] = chunk;
            }
        }

        return chunks;
    }
} // namespace gin
