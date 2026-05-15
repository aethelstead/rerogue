#include "gfx/gfx.h"
#include "gin.h"
#include <SDL2/SDL_render.h>

namespace gin
{
    void State::draw()
    {
        SDL_SetRenderDrawColor(gfx.renderer.get(), 0, 0, 0, 0xff);
        SDL_RenderClear(gfx.renderer.get());

        //if (game.is_paused == false)
        {
            const auto& game_texture = gfx.textures.at("_game");
            SDL_SetRenderTarget(gfx.renderer.get(), game_texture.get());

            SDL_SetRenderDrawColor(gfx.renderer.get(), 0, 0, 0, 0xff);
            SDL_RenderClear(gfx.renderer.get());
            draw_game(gfx.textures, tilesets);

            SDL_SetRenderTarget(gfx.renderer.get(), NULL);
            
            SDL_RenderCopy(gfx.renderer.get(), game_texture.get(), nullptr, nullptr);
        }

        draw_gui();
    }

    void State::draw_game(const TextureMap& textures, const TilesetMap& tilesets)
    {
        // Top-left corner chunk pos
        Vec2i chunkpos{ game.camera.x / CHUNK_PIXELS, game.camera.y / CHUNK_PIXELS };
        Vec2i prev_chunkpos = chunkpos;
        const auto& tileset = tilesets.at("world");
        const auto& texture = textures.at(tileset.texture_key);
    
        for (int y = 0; y < SQRT_VIEW_CHUNKS; ++y)
        {
            for (int x = 0; x < SQRT_VIEW_CHUNKS; ++x)
            {
                if (chunkpos.x < WORLD_CHUNKS && chunkpos.y < WORLD_CHUNKS)
                {
                    const auto& chunk = game.chunks[chunkpos.x][chunkpos.y];
                    if (chunk.has_value())
                    {
                        Vec2i topleft{ x * CHUNK_PIXELS, y * CHUNK_PIXELS };
                        draw_chunk_tiles(topleft, chunk.value(), texture, tileset);
                    }
                }

                chunkpos.x++;
            }
            chunkpos.y++;
            chunkpos.x = prev_chunkpos.x;
        }
        
        draw_sprites(textures, tilesets);
    }

    void State::draw_chunk_tiles(Vec2i& topleft, const WorldChunk& chunk, const Texture& texture, const Tileset& tileset)
    {
        Vec2i start{
            0,
            0,
        };
        Vec2i end{ 
            CHUNK_TILES, 
            CHUNK_TILES
        };

        for (int y = start.y; y < end.y; ++y)
        {
            for (int x = start.x; x < end.x; ++x)
            {
                const auto& td = tileset.tiles.at(chunk.tiles[x][y]);
                // Don't render tile entities as tiles
                if (td.entity_key.has_value())
                    continue;

                int frame = td.frames.at(td.frameidx) - 1;

                SDL_Rect src{
                    (frame % tileset.cells_per_row) * TILE_PIXELS, 
                    (frame / tileset.cells_per_row) * TILE_PIXELS, 
                    TILE_PIXELS, 
                    TILE_PIXELS
                };
                SDL_Rect dst{
                    topleft.x + (x * TILE_PIXELS) - (game.camera.x % CHUNK_PIXELS), 
                    topleft.y + (y * TILE_PIXELS) - (game.camera.y % CHUNK_PIXELS), 
                    TILE_PIXELS, 
                    TILE_PIXELS
                };
                SDL_RenderCopy(gfx.renderer.get(), texture.get(), &src, &dst);
            }
        }
    }

    void State::draw_sprites(const TextureMap& textures, const TilesetMap& tilesets)
    {
        for (auto [eid, sprite] : game.sprites)
        {
            if (!sprite.in_view)
                continue;

            const auto& tileset = tilesets.at(sprite.tileset_key);
            const auto& texture = textures.at(tileset.texture_key);
            const auto& td = tileset.tiles.at(sprite.anim_id);
            int frame = td.frames.at(sprite.frameidx) - 1;

            SDL_Rect src{ 
                (frame % tileset.cells_per_row) * TILE_PIXELS, 
                (frame / tileset.cells_per_row) * TILE_PIXELS, 
                TILE_PIXELS, 
                TILE_PIXELS
            };
            SDL_Rect dst{
                sprite.view_pos.x, 
                sprite.view_pos.y, 
                TILE_PIXELS, 
                TILE_PIXELS
            };
            SDL_RenderCopy(gfx.renderer.get(), texture.get(), &src, &dst);
        }
    }

    void State::draw_gui()
    {
        if (gui.show_overmap)
        {
            draw_overmap();
        }
        // ...
        else if (gui.show_pause)
        {
            int txtw = gfx.get_text_width("Paused", "metamorphous_large");
            SDL_Rect box{
                (gui.view.w / 2 - txtw / 2),
                (gui.view.h / 2) - (FONT_SIZE_LARGE / 2),
                txtw + 20,
                FONT_SIZE_LARGE + 20
            };
            SDL_SetRenderDrawColor(gfx.renderer.get(), 0x40, 0, 0, 0xff);
            SDL_RenderFillRect(gfx.renderer.get(), &box);

            SDL_SetRenderDrawColor(gfx.renderer.get(), 0xaa, 0xaa, 0, 0xff);
            gfx.draw_text("Paused", "metamorphous_large", box.x + 5, box.y + 5);
        }
    }

    /*
    void State::draw_all_messages(SDL_Renderer* renderer, const std::vector<std::string>& all_messages, Glyphsheet& glyphs)
    {
        SDL_SetRenderDrawColor(renderer, 0x40, 0, 0, 0xff);
        int max = std::min((int)all_messages.size(), MESSAGES_LINES);

        for (int i = 0; i < max; ++i)
        {
            SDL_Rect rect{10, 10 + (i * 45), 270, 32};
            SDL_RenderFillRect(renderer, &rect);
        }
        SDL_SetRenderDrawColor(renderer, 0xaa, 0xaa, 0, 0xff);
        for (int i = 0; i < max; ++i)
        {
            SDL_Rect rect{10, 10 + (i * 45), 270, 32};
            SDL_RenderDrawRect(renderer, &rect);
        }

        for (int i = 0; i < max; ++i)
        {
            int idx = all_messages.size() - (max - i);
            const auto& txt = all_messages.at(idx);
            draw_text(renderer, glyphs, txt, 15, 15 + (i * 45));
        }
    }

    void State::draw_speech(SDL_Renderer* renderer, std::string_view speech, Glyphsheet& glyphs)
    {
        SDL_Rect rect{(1080 / 2) - 100, 960 - 150, 400, 120};

        SDL_SetRenderDrawColor(renderer, 0x40, 0, 0, 0xff);
        SDL_RenderFillRect(renderer, &rect);

        SDL_SetRenderDrawColor(renderer, 0xaa, 0xaa, 0, 0xff);
        SDL_RenderDrawRect(renderer, &rect);

        draw_text(renderer, glyphs, speech, rect.x + 10, rect.y + 10);
    }*/

    void State::draw_overmap()
    {
        SDL_Rect map_rect{ 
            (gui.view.w / 2) - 320, 
            (gui.view.h / 2) - 320, 
            640, 
            640 
        };
        const auto& texture = gfx.textures.at("_overmap");
        SDL_RenderCopy(gfx.renderer.get(), texture.get(), nullptr, &map_rect);

        /*
        const auto& player = game.positions.at(game.player_eid);
        SDL_Rect mp_rect{ 
            map_rect.x + (player.chunk_pos.x * 20), 
            map_rect.y + (player.chunk_pos.y * 20), 
            20, 
            20 
        };
        SDL_Rect mp_rect2{ 
            map_rect.x + (player.chunk_pos.x * 20) + 6, 
            map_rect.y + (player.chunk_pos.y * 20) + 6, 
            8, 
            8 
        };
        SDL_SetRenderDrawColor(gfx.renderer.get(), 0xaa, 0xaa, 0, 0xff);
        SDL_RenderDrawRect(gfx.renderer.get(), &mp_rect);
        SDL_RenderFillRect(gfx.renderer.get(), &mp_rect2);*/
    }
}