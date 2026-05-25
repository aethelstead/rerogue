#pragma once

#include <array>
#include <string>
#include <vector>
#include <optional>
#include <unordered_map>
#include "../lua/lua.h"

namespace gin
{
    constexpr inline const int TILE_PIXELS = 16;
    constexpr inline const int CHUNK_TILES = 32;
    constexpr inline const int CHUNK_PIXELS = CHUNK_TILES * TILE_PIXELS;
    constexpr inline const int WORLD_CHUNKS = 32;
    constexpr inline const int WORLD_TILES = WORLD_CHUNKS * CHUNK_TILES;
    constexpr inline const int WORLD_PIXELS = WORLD_CHUNKS * CHUNK_PIXELS;

    constexpr inline const int TILEMAP_LAYERS = WORLD_CHUNKS * CHUNK_PIXELS;
    
    using TileLayer = std::array<int, 1024>;

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
        std::vector<TileLayer> layers;
        std::string type;

        WorldChunk() {}

        WorldChunk(std::vector<TileLayer>& t) : layers(t) {}
    };

    using ChunkMap = std::array<std::array<std::optional<WorldChunk>, WORLD_CHUNKS>, WORLD_CHUNKS>;

    TilesetMap load_tilesets();

    ChunkMap load_chunks(const Tileset& world_tileset);
}