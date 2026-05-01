#include "geometry.h"
#include "gin.h"
#include "sol/sol.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_gamecontroller.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <cstddef>
#include <format>
#include <print>
#include <algorithm>
#include <unordered_map>

namespace gin
{
    static constexpr const char* CHARSET = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

    static int get_glyph_advance(TTF_Font* font, char ch)
    {
        SDL_Rect r{0, 0, 0, 0};
        int advance = 0;
        TTF_GlyphMetrics32(font, ch, &r.x, &r.y, &r.w, &r.h, &advance);

        return advance;
    }

    static int get_text_width(TTF_Font* font, std::string_view text)
    {
        int width = 0;
        for (const char& ch : text)
        {
            SDL_Rect r{0, 0, 0, 0};
            int advance = 0;
            TTF_GlyphMetrics32(font, ch, &r.x, &r.y, &r.w, &r.h, &advance);

            width += r.w + advance;
        }

        return width;
    }

    static void draw_text(SDL_Renderer* renderer, Glyphsheet& glyphsheet, std::string_view text, int x, int y)
    {
        int ASCII_START = static_cast<int>(' ');
        int idx = 0;
        SDL_Rect src = {0, 0, glyphsheet.ptsize, glyphsheet.ptsize - 1};
        SDL_Rect dst = {x, y, glyphsheet.ptsize, glyphsheet.ptsize};

        for (const auto& ch : text)
        {
            int cellIdx = (ch - ASCII_START);
            int cellX = (cellIdx % glyphsheet.cols) * glyphsheet.ptsize;
            int cellY = (cellIdx / glyphsheet.cols) * glyphsheet.ptsize;
            src.x = cellX;
            src.y = cellY + 2;

            int advance = 0;
            if (idx > 0)
            {
                char prevch = text.at(idx - 1);
                SDL_Rect r{0, 0, 0, 0};
                TTF_GlyphMetrics32(glyphsheet.font.get(), prevch, &r.x, &r.y, &r.w, &r.h, &advance);
            }
            dst.x += advance;

            uint8_t r, g, b, a;
            SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);
            SDL_SetTextureColorMod(glyphsheet.texture.get(), r, g, b);
            SDL_SetTextureAlphaMod(glyphsheet.texture.get(), a);

            SDL_RenderCopy(renderer, glyphsheet.texture.get(), &src, &dst);

            ++idx;
        }
    }

    static void draw_chunk_tiles(SDL_Renderer* renderer, GameState& game, Vec2i& topleft, const WorldChunk& chunk, const Texture& texture, const Tileset& tileset)
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
                SDL_RenderCopy(renderer, texture.get(), &src, &dst);
            }
        }
    }

    static void draw_entities(SDL_Renderer* renderer, GameState& game, const TextureMap& textures, const TilesetMap& tilesets)
    {
        for (EntityId eid : game.sim_ents)
        {
            const auto& sprite = game.sprites.at(eid);

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
            SDL_RenderCopy(renderer, texture.get(), &src, &dst);
        }
    }

    static void draw_game(SDL_Renderer* renderer, GameState& game, const TextureMap& textures, const TilesetMap& tilesets)
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
                        draw_chunk_tiles(renderer, game, topleft, chunk.value(), texture, tileset);
                    }
                }

                chunkpos.x++;
            }
            chunkpos.y++;
            chunkpos.x = prev_chunkpos.x;
        }
        
        draw_entities(renderer, game, textures, tilesets);
    }

    static void draw_all_messages(SDL_Renderer* renderer, const std::vector<std::string>& all_messages, Glyphsheet& glyphs)
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

    void draw_speech(SDL_Renderer* renderer, std::string_view speech, Glyphsheet& glyphs)
    {
        SDL_Rect rect{(1080 / 2) - 100, 960 - 150, 400, 120};

        SDL_SetRenderDrawColor(renderer, 0x40, 0, 0, 0xff);
        SDL_RenderFillRect(renderer, &rect);

        SDL_SetRenderDrawColor(renderer, 0xaa, 0xaa, 0, 0xff);
        SDL_RenderDrawRect(renderer, &rect);

        draw_text(renderer, glyphs, speech, rect.x + 10, rect.y + 10);
    }

    void draw_overmap(SDL_Renderer* renderer, State& state)
    {
        SDL_Rect map_rect{ 400, 120, 640, 640 };
        SDL_RenderCopy(state.renderer.get(), state.overmap_texture.get(), nullptr, &map_rect);

        const auto& player = state.game.entities.at(state.game.player_eid);
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
        SDL_SetRenderDrawColor(renderer, 0xaa, 0xaa, 0, 0xff);
        SDL_RenderDrawRect(state.renderer.get(), &mp_rect);
        SDL_RenderFillRect(state.renderer.get(), &mp_rect2);
    }

    void draw(State& state)
    {
        SDL_SetRenderDrawColor(state.renderer.get(), 0x2a, 0, 0, 0xff);
        SDL_RenderClear(state.renderer.get());

        //if (!state.is_paused)
        {
            SDL_SetRenderTarget(state.renderer.get(), state.game_texture.get());

            SDL_SetRenderDrawColor(state.renderer.get(), 0, 0, 0, 0xff);
            SDL_RenderClear(state.renderer.get());
            draw_game(state.renderer.get(), state.game, state.assets.textures, state.tilesets);

            SDL_SetRenderTarget(state.renderer.get(), NULL);
            
            SDL_RenderCopy(state.renderer.get(), state.game_texture.get(), nullptr, nullptr);
        }

        if (state.opt_speech.has_value())
        {
            draw_speech(state.renderer.get(), state.opt_speech.value(), state.game_glyphs);
        }

        if (state.show_overmap)
            draw_overmap(state.renderer.get(), state);

        draw_all_messages(state.renderer.get(), state.all_messages, state.game_glyphs);

        SDL_SetRenderDrawColor(state.renderer.get(), 0xff, 0xff, 0, 0xff);
        draw_text(state.renderer.get(), state.sys_glyphs, std::format("{}", state.perf_counter.avg_fps), 1280 - 20, 5);
        draw_text(state.renderer.get(), state.sys_glyphs, std::format("{:.2f}ms", state.perf_counter.game_ms), 1280 - 55, 22);

        //const auto& ppos = state.game.entities.at(state.game.player_eid);
        //draw_text(state.renderer.get(), state.game_glyphs, std::format("{}x{}", ppos.tile_pos.x, ppos.tile_pos.y), 10, 10);
        //draw_text(state.renderer.get(), state.game_glyphs, std::format("{}x{}", ppos.next_tile_pos.x, ppos.next_tile_pos.y), 10, 30);
    }
} // namespace gin
