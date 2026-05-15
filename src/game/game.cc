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
        }

        // Add Position
        Vec2i epos{tile_pos.x * TILE_PIXELS, tile_pos.y * TILE_PIXELS};
        WorldEntity ent;
        ent.tile_pos = epos;
        ent.table = table;
        ents.try_emplace(eid, ent);

        // Add Sprite
        std::string tileset_key = table["tileset_key"];
        std::string anim_key = table["anim_key"];
        sprites.try_emplace(eid, tileset_key, anim_key);

        return table;
    }

    void GameState::despawn_entity(EntityId eid)
    {
        ents.erase(eid);
        sprites.erase(eid);
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

        std::println("game was init.");
    }

    bool GameState::in_sim_region(Vec2f pos)
    {
        return (pos.x >= camera.left() && pos.y >= camera.top() && pos.x <= camera.right() && pos.y <= camera.bottom());
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

    void GameState::update_ents(double dt)
    {
        for (auto& [eid, ent] : ents)
        {
            if (in_sim_region(ent.pos))
            {
                ent.update();
                ent.table["tick"](dt);
            }
        }
    }

    void GameState::update_sprites(double dt, const TilesetMap& tilesets)
    {
        for (auto& [eid, ent] : ents)
        {
            if (!in_sim_region(ent.pos))
                continue;

            auto& sprite = sprites.at(eid);
            sprite.in_view = true;

            sprite.view_pos.x = ent.pos.x - camera.x;
            sprite.view_pos.y = ent.pos.y - camera.y;

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

        /*
        for (auto& [eid, position] : positions)
        {
            if (!in_sim_region(position.pos))
                continue;
        }*/

        //update_reaps();
        update_ents(dt);
        //update_statefuls();
        //update_collisions(world_tileset);
        //update_movements();
        update_sprites(dt, tilesets);

        const auto& player = ents.at(player_eid);
        update_camera(player.pos);
    }

} // namespace gin
