#include "game.h"
#include <format>
#include <print>

namespace gin
{
    sol::table GameState::spawn_entity(Vec2i tile_pos, std::string_view archetype)
    { 
        if (player_eid != 0 && archetype == "player")
        {
            std::println("The one and only player has already spawned in.");
            return 0;
        }

        // Create new Lua entity
        sol::table L_ent = get_L().do_file(std::format("scripts/entities/{}.lua", archetype));
        sol::table table = L_ent["new"]();
        EntityId eid = table["eid"];

        if (archetype == "player")
        {
            player_eid = eid;
            std::println("Set player id: {}", player_eid);
        }

        // Add Physics
        EntityPhysics phys(tile_pos, Vec2i::south(), 1);

        // Add Sprite
        std::string tileset_key = table["tileset_key"];
        EntitySprite sprite(tileset_key);

        sprite.anim_key = "idle_south";
        sprite.prev_anim_key = sprite.anim_key;

        ents.try_emplace(eid, eid, phys, sprite, table);

        return table;
    }

    void GameState::despawn_entity(EntityId eid)
    {
        ents.erase(eid);
    }

    void GameState::init(int w, int h)
    {
        camera = Recti(0, 0, w, h);

        auto L_gin = get_L()["gin"].get<sol::table>();
        L_gin["pause_game"] = [&]() { is_paused = true; };
        L_gin["resume_game"] = [&]() { is_paused = false; };
        L_gin["spawn_entity"] = [&](const char* archetype, int tx, int ty) 
            { 
                return spawn_entity(Vec2i(tx, ty), archetype);
            };
        L_gin["reap_entity"] = [&](EntityId eid) {  };
        L_gin["set_sprite_animation"] = [&](EntityId eid, const char* anim_key) 
            {
                auto& ent = ents.at(eid);
                ent.sprite.set_animation(anim_key);
            };
        L_gin["set_phys_busy"] = [&](EntityId eid, int nframes)
            {
                auto& ent = ents.at(eid);
                ent.phys.busy_frames = nframes;
            };
        L_gin["is_phys_busy"] = [&](EntityId eid)
            {
                auto& ent = ents.at(eid);
                return (ent.phys.busy_frames > 0);
            };

        std::println("game was init.");
    }

    bool GameState::in_sim_region(Vec2f pos)
    {
        return (pos.x >= camera.left() && pos.y >= camera.top() && pos.x <= camera.right() && pos.y <= camera.bottom());
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

        // Update entity positions
        for (auto& [eid, ent] : ents)
            if (ent.phys.in_sim = in_sim_region(ent.phys.pos); ent.phys.in_sim)
                ent.phys.position();

        // Pre-update entity scripts
        for (auto& [eid, ent] : ents)
        {
            if (!ent.phys.in_sim)
                continue;

            sol::table L_dir = ent.table["dir"];
            ent.phys.dir.x = L_dir["x"].get<int>();
            ent.phys.dir.y = L_dir["y"].get<int>();

            sol::table L_vel = ent.table["vel"];
            ent.phys.vel.x = L_vel["x"].get<int>();
            ent.phys.vel.y = L_vel["y"].get<int>();

            // Only true when the velocity is set
            if (ent.phys.pos == ent.phys.next_pos)
            {
                // Set the next_pos
                ent.phys.next_pos.x = ent.phys.pos.x + (ent.phys.vel.x * TILE_PIXELS);
                ent.phys.next_pos.y = ent.phys.pos.y + (ent.phys.vel.y * TILE_PIXELS);
            }
        }

        // Check entity collision
        for (auto& [eid, ent] : ents)
        {
            if (!ent.phys.in_sim)
                continue;

            // Reset the collision from last frame
            ent.phys.collide_dir = Vec2i::zero();

            // Map bounds checking
            if ((ent.phys.next_pos.x < 0 && ent.phys.vel.x < 0) ||
                (ent.phys.next_pos.y < 0 && ent.phys.vel.y < 0) ||
                (ent.phys.next_pos.x > WORLD_PIXELS - TILE_PIXELS && ent.phys.vel.x > 0) ||
                (ent.phys.next_pos.y > WORLD_PIXELS - TILE_PIXELS && ent.phys.vel.y > 0))
            {
                ent.phys.next_pos = ent.phys.pos;
                
                ent.phys.collide_dir.x = ent.phys.dir.x * -1;
                ent.phys.collide_dir.y = ent.phys.dir.y * -1;
            }

            // Wall tile checking
            Vec2i ntile = ent.phys.tile_pos + ent.phys.dir;
            const auto& opt_chunk = chunks[ent.phys.chunk_pos.x][ent.phys.chunk_pos.y];
            if (opt_chunk.has_value())
            {
                const auto& chunk = opt_chunk.value();
                Vec2i nltile{ntile.x % CHUNK_TILES, ntile.y % CHUNK_TILES};
                int tile_id = chunk.tiles[nltile.x][nltile.y];
                const auto& td = world_tileset.tiles.at(tile_id);
                if (td.is_wall)
                {
                    ent.phys.next_pos = ent.phys.pos;

                    ent.phys.collide_dir.x = ent.phys.dir.x * -1;
                    ent.phys.collide_dir.y = ent.phys.dir.y * -1;
                }
            }
        }

        // Move entities
        for (auto& [eid, ent] : ents)
        {
            if (!ent.phys.in_sim)
                continue;

            ent.phys.pos.x += (ent.phys.vel.x + ent.phys.collide_dir.x) * ent.phys.speed;
            ent.phys.pos.y += (ent.phys.vel.y + ent.phys.collide_dir.y) * ent.phys.speed;

            if ((ent.phys.pos.y + 1 > ent.phys.next_pos.y && ent.phys.vel.y > 0) || 
                (ent.phys.pos.y - 1 < ent.phys.next_pos.y && ent.phys.vel.y < 0) ||
                (ent.phys.pos.x + 1 > ent.phys.next_pos.x && ent.phys.vel.x > 0) || 
                (ent.phys.pos.x - 1 < ent.phys.next_pos.x && ent.phys.vel.x < 0) )
            {
                ent.phys.pos = ent.phys.next_pos;
                ent.table["stop"](ent.table);
            }
        }

        // Update sprites
        for (auto& [eid, ent] : ents)
        {
            ent.sprite.in_view = ent.phys.in_sim;
            if (!ent.phys.in_sim)
                continue;

            ent.sprite.view_pos.x = ent.phys.pos.x - camera.x;
            ent.sprite.view_pos.y = ent.phys.pos.y - camera.y;

            const auto& tileset = tilesets.at(ent.sprite.tileset_key);

            //if (!tileset.anim_map.count(ent.sprite.anim_key))
            //    continue;

            ent.sprite.anim_id = tileset.anim_map.at(ent.sprite.anim_key);
            const auto& td = tileset.tiles.at(ent.sprite.anim_id);

            ent.sprite.framems += (dt * 1000);
            if (ent.sprite.framems >= td.duration)
            {
                ent.sprite.framems = 0;

                ent.sprite.frameidx++;
                if (ent.sprite.frameidx >= td.frames.size())
                {
                    ent.sprite.frameidx = 0;
                }
            }
        }


        for (auto& [eid, ent] : ents)
        {
            if (!ent.phys.in_sim)
                continue;

            ent.phys.busy_frames = std::max(0, --ent.phys.busy_frames);
            if (ent.phys.busy_frames == 0)
                ent.table["ready"] = true;
        }

        const auto& player = ents.at(player_eid);
        update_camera(player.phys.pos);
    }

} // namespace gin
