#include "game.h"
#include <format>
#include <print>

namespace gin
{
    EntityId GameState::spawn_entity(Vec2i tile_pos, std::string_view archetype)
    {
        auto& L = get_L();

        // Create new Lua entity
        sol::table L_ent = L.do_file(std::format("scripts/entities/{}.lua", archetype));
        sol::table table = L_ent["new"]();
        EntityId eid = table["eid"];

        // Add Position
        Vec2i epos{tile_pos.x * TILE_PIXELS, tile_pos.y * TILE_PIXELS};
        positions.try_emplace(eid, tile_pos);

        // Add Collision
        collisions.try_emplace(eid);

        // Add Movement
        movements.try_emplace(eid, 1);

        // Add Stateful
        statefuls.try_emplace(eid);

        // Add Script
        scripts.try_emplace(eid, table);

        // Add Sprite
        sol::table L_sprite = table["sprite"];
        std::string tileset_key = L_sprite["tileset_key"];
        std::string anim_key = L_sprite["anim_key"];
        sprites.try_emplace(eid, tileset_key, anim_key);

        return eid;
    }

    void GameState::despawn_entity(EntityId eid)
    {
        positions.erase(eid);
        collisions.erase(eid);
        movements.erase(eid);
        statefuls.erase(eid);
        scripts.erase(eid);
        sprites.erase(eid);
    }

    void GameState::init(int w, int h)
    {
        camera = Recti(0, 0, w, h);

        std::println("game was init.");
    }

    bool GameState::in_sim_region(Vec2f pos)
    {
        return (pos.x >= camera.left() && pos.y >= camera.top() && pos.x <= camera.right() && pos.y <= camera.bottom());
    }

    void GameState::handle_face_command(EntityId eid, const Vec2i& dir, std::string_view anim_key, int cooldown)
    {
        auto& stateful = statefuls.at(eid);
        if (stateful.state == EntityState::Idle && stateful.cooldown == 0)
        {
            auto& position = positions.at(eid);
            auto& sprite = sprites.at(eid);

            position.dir = dir;
            stateful.cooldown = cooldown;
            sprite.prev_anim_key = anim_key;
            sprite.set_animation(anim_key);
        }
    }

    void GameState::handle_walk_command(EntityId eid, std::string_view anim_key, int cooldown)
    {
        auto& stateful = statefuls.at(eid);
        if (stateful.state == EntityState::Idle && stateful.cooldown == 0)
        {
            auto& position = positions.at(eid);
            auto& movement = movements.at(eid);
            auto& sprite = sprites.at(eid);

            stateful.state = EntityState::Walking;
            stateful.cooldown = cooldown;

            sprite.prev_anim_key = sprite.anim_key;
            sprite.set_animation(anim_key);

            movement.walk_tile = position.tile_pos + position.dir;
            movement.vel = position.dir;
        }
    }

    void GameState::update_reaps()
    {
        /*
        for (const auto& eid : sim_ents)
        {
            const auto& stateful = statefuls.at(eid);
            if (stateful.reap)
                despawn_entity(eid);
        }*/
    }

    void GameState::update_positions()
    {
        for (auto& [eid, position] : positions)
        {
            if (in_sim_region(position.pos))
                position.update();
        }
    }

    void GameState::update_statefuls()
    {
        for (auto& [eid, position] : positions)
        {
            if (!in_sim_region(position.pos))
                continue;

            auto& movement = movements.at(eid);
            auto& stateful = statefuls.at(eid);
            auto& sprite = sprites.at(eid);

            stateful.cooldown = std::max(0, --stateful.cooldown);

            switch (stateful.command)
            {
                case EntityCommand::FaceNorth:
                    handle_face_command(eid, Vec2i::north(), "idle_north", 8);
                    break;
                case EntityCommand::FaceEast:
                    handle_face_command(eid, Vec2i::east(), "idle_east", 8);
                    break;
                case EntityCommand::FaceSouth:
                    handle_face_command(eid, Vec2i::south(), "idle_south", 8);
                    break;
                case EntityCommand::FaceWest:
                    handle_face_command(eid, Vec2i::west(), "idle_west", 8);
                    break;
                case EntityCommand::WalkNorth:
                    handle_walk_command(eid, "walk_north");
                    break;
                case EntityCommand::WalkEast:
                    handle_walk_command(eid, "walk_east");
                    break;
                case EntityCommand::WalkSouth:
                    handle_walk_command(eid, "walk_south");
                    break;
                case EntityCommand::WalkWest:
                    handle_walk_command(eid, "walk_west");
                    break;
                case EntityCommand::Attack:
                    /*
                    if (stateful.state == EntityState::Idle && stateful.cooldown == 0)
                    {
                        stateful.cooldown = 20;
                        stateful.state = EntityState::Attacking;
                        if (entity.dir == Vec2i::north())
                            set_entity_animation(eid, "attack_north", game);
                        else if (entity.dir == Vec2i::east())
                            set_entity_animation(eid, "attack_east", game);
                        else if (entity.dir == Vec2i::south())
                            set_entity_animation(eid, "attack_south", game);
                        else if (entity.dir == Vec2i::west())
                            set_entity_animation(eid, "attack_west", game);
                    }*/
                    break;
                case EntityCommand::Interact:
                    if (stateful.state == EntityState::Idle && stateful.cooldown == 0)
                    {
                        stateful.state = EntityState::Interacting;
                        stateful.cooldown = 8;
                    }
                    break;
                case EntityCommand::None:
                default:
                    break;
            }
            stateful.command = EntityCommand::None;
        }
    }

    void GameState::update_collisions(const Tileset& world_tileset)
    {
        for (auto& [eid, position] : positions)
        {
            if (!in_sim_region(position.pos))
                continue;

            auto& collision = collisions.at(eid);
            collision.dir = Vec2i::zero();

            Vec2i ntile(position.tile_pos.x + position.dir.x, position.tile_pos.y + position.dir.y);

            const auto& opt_chunk = chunks[position.chunk_pos.x][position.chunk_pos.y];

            // Map bounds checking
            if (ntile.x < 0 || ntile.y < 0 || ntile.x >= WORLD_TILES || ntile.y >= WORLD_TILES)
            {
                collision.dir.x = collision.dir.x * -1;
                collision.dir.y = collision.dir.y * -1;
            }
            else if (opt_chunk.has_value())
            {
                const auto& chunk = opt_chunk.value();
                Vec2i nltile{ntile.x % CHUNK_TILES, ntile.y % CHUNK_TILES};
                int tile_id = chunk.tiles[nltile.x][nltile.y];
                const auto& td = world_tileset.tiles.at(tile_id);
                if (td.is_wall)
                {
                    collision.dir.x = position.dir.x * -1;
                    collision.dir.y = position.dir.y * -1;
                }
            }

            /*
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
            }*/
        }
    }

    void GameState::update_movements()
    {
        for (auto& [eid, position] : positions)
        {
            if (!in_sim_region(position.pos))
                continue;

            auto& movement = movements.at(eid);
            auto& collision = collisions.at(eid);
            auto& stateful = statefuls.at(eid);
            auto& sprite = sprites.at(eid);

            if (stateful.state == EntityState::Walking)
            {
                Vec2f target_pos(movement.walk_tile.x * TILE_PIXELS, movement.walk_tile.y * TILE_PIXELS);

                if ((position.dir == Vec2i::north() && position.pos.y <= target_pos.y) || 
                    (position.dir == Vec2i::west() && position.pos.x <= target_pos.x) || 
                    (position.dir == Vec2i::south() && position.pos.y >= target_pos.y) ||
                    (position.dir == Vec2i::east() && position.pos.x >= target_pos.x) || 
                    collision.dir != Vec2i::zero())
                {
                    // position.pos = target_pos;
                    movement.vel = Vec2i::zero();
                    stateful.state = EntityState::Idle;
                    stateful.cooldown = 0;
                    sprite.set_animation(sprite.prev_anim_key);

                    collision.dir.x = 0;
                    collision.dir.y = 0;
                }
            }

            position.pos.x += (movement.vel.x + collision.dir.x) * movement.speed;
            position.pos.y += (movement.vel.y + collision.dir.y) * movement.speed;
        }
    }

    void GameState::update_scripts()
    {
        for (auto& [eid, position] : positions)
        {
            if (!in_sim_region(position.pos))
                continue;

            auto& script = scripts.at(eid);

            sol::table L_tile = script.L_ent["tile"];
            L_tile["x"] = position.tile_pos.x;
            L_tile["y"] = position.tile_pos.y;

            sol::table L_dir = script.L_ent["dir"];
            L_dir["x"] = position.dir.x;
            L_dir["y"] = position.dir.y;

            /*
            if (entity.cooldown == 0)
            {
                entity.table["think"](entity.table, entity.collision.opps);
            }*/
        }
    }

    void GameState::update_sprites(double dt, const TilesetMap& tilesets)
    {
        for (auto& [eid, position] : positions)
        {
            auto& sprite = sprites.at(eid);
            sprite.in_view = in_sim_region(position.pos);
            if (!sprite.in_view)
                continue;

            sprite.view_pos.x = position.pos.x - camera.x;
            sprite.view_pos.y = position.pos.y - camera.y;

            const auto& tileset = tilesets.at(sprite.tileset_key);
            sprite.anim_id = tileset.anim_map.at(sprite.anim_key);
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

    void GameState::update_camera(const Vec2f& target_pos)
    {
        int target_x = (int)target_pos.x + (TILE_PIXELS / 2);
        int target_y = (int)target_pos.y + (TILE_PIXELS / 2);

        camera.x = (target_x - (camera.w / 2));
        camera.y = (target_y - (camera.h / 2));

        camera.x = std::clamp(camera.x, 0, WORLD_PIXELS - camera.w);
        camera.y = std::clamp(camera.y, 0, WORLD_PIXELS - camera.h);
    }

    void GameState::update(double dt, const TilesetMap& tilesets)
    {
        const auto& world_tileset = tilesets.at("world");

        update_reaps();
        update_positions();
        update_statefuls();
        update_collisions(world_tileset);
        update_movements();
        // update_scripts();
        update_sprites(dt, tilesets);

        const auto& player = positions.at(player_eid);
        update_camera(player.pos);
    }

} // namespace gin
