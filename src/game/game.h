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

    struct EntityPhysics
    {
        Vec2f pos;
        Vec2f next_pos;
        Vec2i dir;
        Vec2i vel;
        Vec2i tile_pos;
        Vec2i chunk_pos;
        Vec2i collide_dir;
        double speed;
        bool in_sim = false;

        EntityPhysics(Vec2i tpos, Vec2i d, double spd)
        {
            tile_pos = tpos;
            dir = d;
            speed = spd;

            pos.x = tile_pos.x * TILE_PIXELS;
            pos.y = tile_pos.y * TILE_PIXELS;
            next_pos = pos;

            chunk_pos.x = tile_pos.x / CHUNK_TILES;
            chunk_pos.y = tile_pos.y / CHUNK_TILES;
        }

        void position()
        {
            tile_pos.x = (pos.x + 2) / TILE_PIXELS;
            tile_pos.y = (pos.y + 2) / TILE_PIXELS;
            chunk_pos.x = tile_pos.x / CHUNK_TILES;
            chunk_pos.y = tile_pos.y / CHUNK_TILES;
        }

        void collision()
        {

        }

        void move()
        {
            pos.x += (vel.x - collide_dir.x) * speed;
            pos.y += (vel.y - collide_dir.y) * speed;
        }
    };

    struct EntitySprite
    {
        std::string anim_key;
        std::string prev_anim_key;
        std::string tileset_key;
        Vec2i view_pos;
        int frameidx = 0;
        int framems = 0;
        int anim_id = 0;
        bool in_view = false;

        EntitySprite(std::string_view tileset_key, std::string_view anim_key) : tileset_key(tileset_key), anim_key(anim_key) 
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

    struct Entity
    {
        EntityId eid;
        EntityPhysics phys;
        EntitySprite sprite;
        sol::table L_ent;
        int state = 0;
        int prev_state = 0;
    };

    struct GameState
    {
        bool is_paused = false;
        Recti camera;
        ChunkMap chunks;

        EntityId player_eid = 0;
        std::unordered_map<EntityId, Entity> ents;

        sol::table spawn_entity(Vec2i tile_pos, std::string_view archetype);
        void despawn_entity(EntityId eid);

        bool in_sim_region(Vec2f pos);

        void init(int w, int h);

        void update(double dt, const TilesetMap& tilesets);

        void update_camera(const Vec2f& target_pos);
    };
} // namespace gin