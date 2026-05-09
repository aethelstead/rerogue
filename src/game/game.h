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

    struct Position
    {
        Vec2f pos;
        Vec2i dir;
        Vec2i tile_pos;
        Vec2i chunk_pos;

        Position(Vec2i tile_pos) : pos(Vec2f(tile_pos.x * TILE_PIXELS, tile_pos.y * TILE_PIXELS)), dir(Vec2i::south()), tile_pos(tile_pos), chunk_pos(Vec2i(tile_pos.x / CHUNK_TILES, tile_pos.y / CHUNK_TILES)) {}

        void update()
        {
            tile_pos.x = (pos.x + 2) / TILE_PIXELS;
            tile_pos.y = (pos.y + 2) / TILE_PIXELS;
            chunk_pos.x = tile_pos.x / CHUNK_TILES;
            chunk_pos.y = tile_pos.y / CHUNK_TILES;
        }
    };

    struct Movement
    {
        Vec2i vel;
        float speed = 0;
        Vec2i walk_tile;

        Movement(int speed) : speed(speed) {}
    };

    struct Collision
    {
        bool is_wall = true;
        Vec2i dir;
        std::array<EntityId, 32> opps;
    };

    enum class EntityCommand
    {
        None,

        FaceNorth,
        FaceEast,
        FaceSouth,
        FaceWest,

        WalkNorth,
        WalkEast,
        WalkSouth,
        WalkWest,

        Attack,
        Interact
    };

    enum class EntityState
    {
        Idle,
        Walking,
        Attacking,
        Interacting
    };

    struct Stateful
    {
        EntityCommand command = EntityCommand::None;
        EntityState state = EntityState::Idle;
        int cooldown = 0;
        bool reap = false;
    };

    struct Script
    {
        sol::table L_ent;

        Script(sol::table tbl) : L_ent(tbl) {}
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

    /*
    struct Entity
    {
        EntityId eid;
        bool in_sim = false;

        Position& position;
        Collision& collision;
        Movement& movement;
        Script& script;
        Sprite& sprite;
    };*/

    struct GameState
    {
        bool is_paused = false;
        Recti camera;
        ChunkMap chunks;

        EntityId player_eid = 0;

        std::unordered_map<EntityId, Position> positions;
        std::unordered_map<EntityId, Collision> collisions;
        std::unordered_map<EntityId, Movement> movements;
        std::unordered_map<EntityId, Stateful> statefuls;
        std::unordered_map<EntityId, Script> scripts;
        std::unordered_map<EntityId, Sprite> sprites;

        EntityId spawn_entity(Vec2i tile_pos, std::string_view archetype);

        void despawn_entity(EntityId eid);

        bool in_sim_region(Vec2f pos);

        void init(int w, int h);

        void handle_face_command(EntityId eid, const Vec2i& dir, std::string_view anim_key, int cooldown = 0);
        void handle_walk_command(EntityId eid, std::string_view anim_key, int cooldown = 0);

        void update(double dt, const TilesetMap& tilesets);

        void update_reaps();
        void update_positions();
        void update_statefuls();
        void update_collisions(const Tileset& world_tileset);
        void update_movements();
        void update_scripts();
        void update_sprites(double dt, const TilesetMap& tilesets);

        void update_camera(const Vec2f& target_pos);
    };
} // namespace gin