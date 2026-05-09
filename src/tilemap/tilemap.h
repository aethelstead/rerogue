#pragma once

#include <array>
#include <string>
#include <vector>
#include <optional>
#include <unordered_map>

namespace gin
{
    inline const int TILE_PIXELS = 16;
    inline const int CHUNK_TILES = 32;
    inline const int CHUNK_PIXELS = CHUNK_TILES * TILE_PIXELS;
    inline const int WORLD_CHUNKS = 32;
    inline const int WORLD_TILES = WORLD_CHUNKS * CHUNK_TILES;
    inline const int WORLD_PIXELS = WORLD_CHUNKS * CHUNK_PIXELS;
    
    using Tilemap = std::array<std::array<int, CHUNK_TILES>, CHUNK_TILES>;

    struct TileData
    {
        bool is_wall = false;
        int duration = 0;
        int frameidx = 0;
        int framems = 0;
        std::string anim_key;
        std::vector<int> frames;
        std::optional<std::string> entity_key;
    };
    using TileDataMap = std::unordered_map<int, TileData>;
    using TileAnimationMap = std::unordered_map<std::string, int>;

    struct Tileset
    {
        int n_cells = 0;
        int cells_per_row = 0;
        std::string texture_key;
        TileDataMap tiles;
        TileAnimationMap anim_map;
    };
    using TilesetMap = std::unordered_map<std::string, Tileset>;

    struct WorldChunk
    {
        Tilemap tiles;
        std::string type;

        WorldChunk() {}

        WorldChunk(Tilemap& t) : tiles(t) {}
    };

    using ChunkMap = std::array<std::array<std::optional<WorldChunk>, WORLD_CHUNKS>, WORLD_CHUNKS>;

    TilesetMap load_tilesets();

    ChunkMap load_chunks(const Tileset& world_tileset);
}