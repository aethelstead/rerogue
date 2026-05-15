#pragma once

#include <string>
#include <cstdint>
#include <optional>
#include <unordered_map>
#include "../lua/lua.h"
#include "../_core/geometry.h"
#include "../tilemap/tilemap.h"

namespace gin
{
    using EntityId = uint;

    struct WorldEntity
    {
        Vec2f pos;
        Vec2i dir;
        Vec2i tile_pos;
        Vec2i chunk_pos;
        sol::table table;

        void update()
        {
            tile_pos.x = (pos.x + 2) / TILE_PIXELS;
            tile_pos.y = (pos.y + 2) / TILE_PIXELS;
            chunk_pos.x = tile_pos.x / CHUNK_TILES;
            chunk_pos.y = tile_pos.y / CHUNK_TILES;
        }
    };

    struct Sprite
    {
        std::string anim_key;
        std::string prev_anim_key;
        std::string tileset_key;
        bool in_view = false;
        Vec2i view_pos;
        int frameidx = 0;
        int framems = 0;
        int anim_id = 0;

        Sprite(std::string_view tileset_key, std::string_view anim_key) : tileset_key(tileset_key), anim_key(anim_key) 
        {
            prev_anim_key = anim_key;
        }

        void set_animation(std::string_view anim)
        {
            anim_key = anim;
            frameidx = 0;
            framems = 0;
        }
    };

    struct GameState
    {
        bool is_paused = false;
        Recti camera;
        ChunkMap chunks;

        EntityId player_eid = 0;

        std::unordered_map<EntityId, WorldEntity> ents;
        std::unordered_map<EntityId, Sprite> sprites;

        sol::table spawn_entity(Vec2i tile_pos, std::string_view archetype);
        void despawn_entity(EntityId eid);

        bool in_sim_region(Vec2f pos);

        void init(int w, int h);

        void update(double dt, const TilesetMap& tilesets);

        void update_reaps();
        void update_ents(double dt);
        void update_sprites(double dt, const TilesetMap& tilesets);

        void update_camera(const Vec2f& target_pos);
    };
} // namespace gin