#include "geometry.h"
#include "gin.h"
#include "sol/sol.hpp"
#include <algorithm>
#include <chrono>
#include <climits>
#include <memory>
#include <optional>
#include <print>
#include <tuple>
#include <unordered_map>

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

    void command_entity(EntityId eid, EntityCommand command, GameState& game)
    {
        auto& entity = game.entities.at(eid);
        entity.command = command;
    }

    void set_entity_animation(EntityId eid, std::string_view anim_key, GameState& game)
    {
        auto& entity = game.entities.at(eid);
        auto& sprite = game.sprites.at(eid);
        entity.anim_key = anim_key;
        sprite.frameidx = 0;
        sprite.framems = 0;
    }

    Collision find_collision(const Entity& entity, const Tileset& world_tileset, const GameState& game)
    {
        Collision collide;

        Vec2i ntile(entity.tile_pos.x + entity.dir.x, entity.tile_pos.y + entity.dir.y);

        // Map bounds checking
        if (ntile.x < 0 || ntile.y < 0 || ntile.x >= WORLD_TILES || ntile.y >= WORLD_TILES)
        {

            collide.dir.x = entity.dir.x * -1;
            collide.dir.y = entity.dir.y * -1;
            return collide;
        }

        const auto& opt_chunk = game.chunks[entity.chunk_pos.x][entity.chunk_pos.y];
        if (opt_chunk.has_value())
        {
            const auto& chunk = opt_chunk.value();
            Vec2i nltile{ntile.x % CHUNK_TILES, ntile.y % CHUNK_TILES};
            int tile_id = chunk.tiles[nltile.x][nltile.y];
            const auto& td = world_tileset.tiles.at(tile_id);
            if (td.solid)
            {
                collide.dir.x = entity.dir.x * -1;
                collide.dir.y = entity.dir.y * -1;
            }
        }

        const int TILE_RANGE = 3;
        for (EntityId opp_eid : game.sim_ents)
        {
            if (entity.eid == opp_eid)
                continue;
            const auto& opp_ent = game.entities.at(opp_eid);

            if (opp_ent.tile_pos.x >= entity.tile_pos.x - TILE_RANGE && opp_ent.tile_pos.x <= entity.tile_pos.x + TILE_RANGE && opp_ent.tile_pos.y >= entity.tile_pos.y - TILE_RANGE && opp_ent.tile_pos.y <= entity.tile_pos.y + TILE_RANGE)
            {
                collide.opps.emplace_back(opp_ent.table);

                if (ntile == opp_ent.tile_pos)
                {
                    if (opp_ent.is_collide)
                    {
                        collide.dir.x = entity.dir.x * -1;
                        collide.dir.y = entity.dir.y * -1;
                    }
                }
            }
        }

        return collide;
    }

    void update_entities(GameState& game, Tileset& world_tileset, double dt)
    {
        // Reap entities
        for (const auto& eid : game.sim_ents)
        {
            const auto& entity = game.entities.at(eid);
            if (entity.reap)
            {
                game.entities.erase(eid);
                game.sprites.erase(eid);
            }
        }

        // Update sim ents
        game.sim_ents.clear();
        for (const auto& [eid, entity] : game.entities)
        {
            if (entity.pos.x > game.camera.x - 100 && entity.pos.x < game.camera.x + game.camera.w + 100 && entity.pos.y > game.camera.y - 100 && entity.pos.y < game.camera.y + game.camera.h + 100)
            {
                game.sim_ents.emplace_back(eid);
            }
        }

        // Update positions
        for (EntityId eid : game.sim_ents)
        {
            auto& entity = game.entities.at(eid);

            entity.tile_pos.x = entity.pos.x / TILE_PIXELS;
            entity.tile_pos.y = entity.pos.y / TILE_PIXELS;

            entity.chunk_pos.x = entity.pos.x / CHUNK_PIXELS;
            entity.chunk_pos.y = entity.pos.y / CHUNK_PIXELS;
        }

        // Update scripts
        for (EntityId eid : game.sim_ents)
        {
            const auto& entity = game.entities.at(eid);

            sol::table L_tile = entity.table["tile"];
            L_tile["x"] = entity.tile_pos.x;
            L_tile["y"] = entity.tile_pos.y;

            sol::table L_dir = entity.table["dir"];
            L_dir["x"] = entity.dir.x;
            L_dir["y"] = entity.dir.y;

            if (entity.cooldown == 0)
            {
                entity.table["think"](entity.table, entity.collision.opps);
            }
        }

        // Update commands
        for (EntityId eid : game.sim_ents)
        {
            auto& entity = game.entities.at(eid);
            entity.cooldown = std::max(0, --entity.cooldown);

            switch (entity.command)
            {
                case EntityCommand::FaceNorth:
                    if (entity.state == EntityState::Idle)
                    {
                        entity.dir = Vec2i::north();
                        entity.walk_pos = entity.pos;

                        set_entity_animation(eid, "idle_north", game);

                        entity.cooldown = 8;
                    }
                    break;
                case EntityCommand::FaceEast:
                    if (entity.state == EntityState::Idle)
                    {
                        entity.dir = Vec2i::east();
                        entity.walk_pos = entity.pos;

                        set_entity_animation(eid, "idle_east", game);

                        entity.cooldown = 8;
                    }
                    break;
                case EntityCommand::FaceSouth:
                    if (entity.state == EntityState::Idle)
                    {
                        entity.dir = Vec2i::south();
                        entity.walk_pos = entity.pos;

                        set_entity_animation(eid, "idle_south", game);

                        entity.cooldown = 8;
                    }
                    break;
                case EntityCommand::FaceWest:
                    if (entity.state == EntityState::Idle)
                    {
                        entity.dir = Vec2i::west();
                        entity.walk_pos = entity.pos;

                        set_entity_animation(eid, "idle_west", game);

                        entity.cooldown = 8;
                    }
                    break;
                case EntityCommand::WalkNorth:
                    if (entity.state == EntityState::Idle && entity.cooldown == 0)
                    {
                        entity.dir = Vec2i::north();
                        entity.state = EntityState::Walking;

                        entity.walk_pos = entity.pos + (entity.dir * TILE_PIXELS);
                        set_entity_animation(eid, "walk_north", game);
                    }
                    break;
                case EntityCommand::WalkEast:
                    if (entity.state == EntityState::Idle && entity.cooldown == 0)
                    {
                        entity.dir = Vec2i::east();
                        entity.state = EntityState::Walking;

                        entity.walk_pos = entity.pos + (entity.dir * TILE_PIXELS);
                        set_entity_animation(eid, "walk_east", game);
                    }
                    break;
                case EntityCommand::WalkSouth:
                    if (entity.state == EntityState::Idle && entity.cooldown == 0)
                    {
                        entity.dir = Vec2i::south();
                        entity.state = EntityState::Walking;

                        entity.walk_pos = entity.pos + (entity.dir * TILE_PIXELS);
                        set_entity_animation(eid, "walk_south", game);
                    }
                    break;
                case EntityCommand::WalkWest:
                    if (entity.state == EntityState::Idle && entity.cooldown == 0)
                    {
                        entity.dir = Vec2i::west();
                        entity.state = EntityState::Walking;

                        entity.walk_pos = entity.pos + (entity.dir * TILE_PIXELS);
                        set_entity_animation(eid, "walk_west", game);
                    }
                    break;
                case EntityCommand::Attack:
                    if (entity.state == EntityState::Idle && entity.cooldown == 0)
                    {
                        entity.cooldown = 20;
                        entity.state = EntityState::Attacking;
                        if (entity.dir == Vec2i::north())
                            set_entity_animation(eid, "attack_north", game);
                        else if (entity.dir == Vec2i::east())
                            set_entity_animation(eid, "attack_east", game);
                        else if (entity.dir == Vec2i::south())
                            set_entity_animation(eid, "attack_south", game);
                        else if (entity.dir == Vec2i::west())
                            set_entity_animation(eid, "attack_west", game);
                    }
                    break;
                case EntityCommand::Interact:
                    if (entity.state == EntityState::Idle && entity.cooldown == 0)
                    {
                        entity.state = EntityState::Interacting;
                        entity.cooldown = 8;
                    }
                    break;
                case EntityCommand::None:
                default:
                    break;
            }
            entity.command = EntityCommand::None;

            entity.collision = find_collision(entity, world_tileset, game);

            switch (entity.state)
            {
                case EntityState::Walking:
                {
                    entity.pos += ((entity.dir + entity.collision.dir) * entity.speed);

                    if (entity.collision.dir != Vec2i::zero() || (entity.dir == Vec2i::east() && entity.pos.x >= entity.walk_pos.x) || (entity.dir == Vec2i::south() && entity.pos.y >= entity.walk_pos.y) || (entity.dir == Vec2i::north() && entity.pos.y <= entity.walk_pos.y) ||
                        (entity.dir == Vec2i::west() && entity.pos.x <= entity.walk_pos.x))
                    {

                        entity.walk_pos = entity.pos;
                        entity.state = EntityState::Idle;
                        if (entity.dir == Vec2i::north())
                            set_entity_animation(eid, "idle_north", game);
                        else if (entity.dir == Vec2i::east())
                            set_entity_animation(eid, "idle_east", game);
                        else if (entity.dir == Vec2i::south())
                            set_entity_animation(eid, "idle_south", game);
                        else if (entity.dir == Vec2i::west())
                            set_entity_animation(eid, "idle_west", game);
                    }

                    break;
                }
                case EntityState::Attacking:
                    if (entity.cooldown == 0)
                    {
                        entity.state = EntityState::Idle;
                        if (entity.dir == Vec2i::north())
                            set_entity_animation(eid, "idle_north", game);
                        else if (entity.dir == Vec2i::east())
                            set_entity_animation(eid, "idle_east", game);
                        else if (entity.dir == Vec2i::south())
                            set_entity_animation(eid, "idle_south", game);
                        else if (entity.dir == Vec2i::west())
                            set_entity_animation(eid, "idle_west", game);
                    }
                    else if (entity.cooldown == 4)
                    {
                        /*
                        if (entity.collision.opp_eid.has_value())
                        {
                            entity.table["hit"](entity.table);

                            auto& opp_ent = game.entities.at(entity.collision.opp_eid.value());
                            opp_ent.table["hurt"](opp_ent.table);
                        }*/
                    }
                    else if (entity.cooldown == 18)
                    {
                        entity.table["swing"](entity.table);
                    }
                    break;
                case EntityState::Interacting:
                    if (entity.cooldown == 0)
                    {
                        entity.state = EntityState::Idle;
                    }
                    else if (entity.cooldown == 6)
                    {
                        /*
                        if (entity.collision.opp_eid.has_value())
                        {
                            auto opp_ent = game.entities.at(entity.collision.opp_eid.value());
                            opp_ent.table["interact"](opp_ent.table);
                        }*/
                    }
                    break;
                case EntityState::Idle:
                default:
                    break;
            }
        }
    }

    void update_sprites(GameState& game, const TilesetMap& tilesets, double dt)
    {
        for (EntityId eid : game.sim_ents)
        {
            auto& entity = game.entities.at(eid);
            auto& sprite = game.sprites.at(eid);

            sprite.view_pos.x = entity.pos.x - game.camera.x;
            sprite.view_pos.y = entity.pos.y - game.camera.y;

            const auto& tileset = tilesets.at(sprite.tileset_key);
            sprite.anim_id = tileset.anim_map.at(entity.anim_key);
            const auto& td = tileset.tiles.at(sprite.anim_id);

            sprite.framems += (dt * 1000);
            if (sprite.framems >= td.duration)
            {
                sprite.framems = 0;

                sprite.frameidx++;
                if (sprite.frameidx >= td.frames.size())
                {
                    sprite.frameidx = 0;
                }
            }
        }
    }

    static void update_camera(const Vec2i& target_pos, Recti& camera)
    {
        int target_x = target_pos.x + (TILE_PIXELS / 2);
        int target_y = target_pos.y + (TILE_PIXELS / 2);

        camera.x = (target_x - (camera.w / 2));
        camera.y = (target_y - (camera.h / 2));

        camera.x = std::clamp(camera.x, 0, WORLD_PIXELS - camera.w);
        camera.y = std::clamp(camera.y, 0, WORLD_PIXELS - camera.h);
    }

    bool update(State& state)
    {
        if (state.is_paused)
            return true;

        auto& world_tileset = state.tilesets.at("world");
        update_tile_animations(world_tileset, state.perf_counter.dt);

        update_entities(state.game, world_tileset, state.perf_counter.dt);

        update_sprites(state.game, state.tilesets, state.perf_counter.dt);

        const auto& player = state.game.entities.at(state.game.player_eid);
        update_camera(player.pos, state.game.camera);

        return true;
    }
} // namespace gin
