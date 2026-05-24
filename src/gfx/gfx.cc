#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <memory>
#include <print>
#include <functional>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string_view>
#include "gfx.h"
#include "../lua/lua.h"
#include "../_core/fs.h"

namespace gin
{
    static constexpr const char* CHARSET = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

    SDL_Surface* create_surface(uint width, uint height)
    {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        uint rmask = 0xff000000;
        uint gmask = 0x00ff0000;
        uint bmask = 0x0000ff00;
        uint amask = 0x000000ff;
#else
        uint rmask = 0x000000ff;
        uint gmask = 0x0000ff00;
        uint bmask = 0x00ff0000;
        uint amask = 0xff000000;
#endif

        const int BPP = 32;
        auto* surface = SDL_CreateRGBSurface(0, width, height, BPP, rmask, gmask, bmask, amask);

        return surface;
    }

    Glyphsheet create_glyphsheet(SDL_Renderer* renderer, std::string_view font_path, int ptsize)
    {
        auto glyphsheet = Glyphsheet();

        std::string charset = CHARSET;
        uint size_sq = std::ceil(std::sqrt(charset.size()));
        glyphsheet.ptsize = ptsize;
        glyphsheet.width = size_sq * ptsize;
        glyphsheet.height = size_sq * ptsize;
        glyphsheet.ncells = charset.size();
        glyphsheet.cols = (glyphsheet.width / ptsize);
        glyphsheet.font = Font(TTF_OpenFont(font_path.data(), ptsize), TTF_CloseFont);

        auto sheet_surface = Surface(create_surface(glyphsheet.width, glyphsheet.height), SDL_FreeSurface);

        int x = 0;
        int y = 0;
        SDL_Color white{0xff, 0xff, 0xff, 0xff};
        for (int idx = 0; idx < glyphsheet.ncells; ++idx)
        {
            const char s[] = {CHARSET[idx], '\0'};
            SDL_Surface* glyph_surface = TTF_RenderUTF8_Blended(glyphsheet.font.get(), s, white);

            SDL_Rect dest = {x, y, glyphsheet.ptsize, glyphsheet.ptsize};
            SDL_BlitSurface(glyph_surface, nullptr, sheet_surface.get(), &dest);

            SDL_FreeSurface(glyph_surface);

            x += glyphsheet.ptsize;
            if (x >= glyphsheet.width)
            {
                y += glyphsheet.ptsize;
                x = 0;
            }
        }

        glyphsheet.texture = Texture(SDL_CreateTextureFromSurface(renderer, sheet_surface.get()), SDL_DestroyTexture);

        return glyphsheet;
    }

    int get_glyph_advance(TTF_Font* font, char ch)
    {
        SDL_Rect r{0, 0, 0, 0};
        int advance = 0;
        TTF_GlyphMetrics32(font, ch, &r.x, &r.y, &r.w, &r.h, &advance);

        return advance;
    }

    int GfxState::get_text_width(std::string_view text, std::string_view glyphs_key)
    {
        const auto& glyphsheet = glyphsheets.at(glyphs_key.data());

        int width = 0;
        for (const char& ch : text)
        {
            SDL_Rect r{0, 0, 0, 0};
            int advance = 0;
            TTF_GlyphMetrics32(glyphsheet.font.get(), ch, &r.x, &r.y, &r.w, &r.h, &advance);

            width += r.w + advance;
        }

        return width;
    }

    void GfxState::draw_text(std::string_view text, std::string_view glyphs_key, int x, int y)
    {
        const auto& glyphsheet = glyphsheets.at(glyphs_key.data());

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
            SDL_GetRenderDrawColor(renderer.get(), &r, &g, &b, &a);
            SDL_SetTextureColorMod(glyphsheet.texture.get(), r, g, b);
            SDL_SetTextureAlphaMod(glyphsheet.texture.get(), a);

            SDL_RenderCopy(renderer.get(), glyphsheet.texture.get(), &src, &dst);

            ++idx;
        }
    }

    void GfxState::init()
    {
        sol::table L_gfx = get_L()["gin"].get<sol::table>()["gfx"].get<sol::table>();
        L_gfx["get_window_title"] = [&]()
            {
                return SDL_GetWindowTitle(window.get());
            };
        L_gfx["set_window_title"] = [&](const char* title)
            {
                SDL_SetWindowTitle(window.get(), title);
                std::println("Set window title");
            };
        L_gfx["resize_window"] = [&](int w, int h)
            {
                SDL_SetWindowSize(window.get(), w, h);
            };
        L_gfx["get_window_width"] = [&]()
            {
                int w = 0;
                int h = 0;
                SDL_GetWindowSize(window.get(), &w, &h);
                return w;
            };
        L_gfx["get_window_height"] = [&]()
            {
                int w = 0;
                int h = 0;
                SDL_GetWindowSize(window.get(), &w, &h);
                return w;
            };
        L_gfx["get_vsync"] = [&]()
            {
                return true;
            };
        L_gfx["set_vsync"] = [&](bool vsync)
            {
                int iv = (vsync) ? 1 : 0;
                SDL_RenderSetVSync(renderer.get(), iv);
            };

        window = Window(SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 0), SDL_DestroyWindow);

        renderer = Renderer(SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_ACCELERATED), SDL_DestroyRenderer);

        foreach_in_dir("assets/img", [&](const auto& fspath)
        {
            auto texture = Texture(IMG_LoadTexture(renderer.get(), fspath.c_str()), SDL_DestroyTexture);
            textures.try_emplace(fspath, std::move(texture));
            std::println("Loaded texture from {}", fspath.stem().string());
        });
        
        // Add the _game texture
        auto pxfmt = SDL_GetWindowPixelFormat(window.get());
        textures.try_emplace(
            "_game", 
            Texture(SDL_CreateTexture(renderer.get(), pxfmt, SDL_TEXTUREACCESS_TARGET, GAMEVIEW_WIDTH, GAMEVIEW_HEIGHT), SDL_DestroyTexture)
        );
        std::println("Created game texture with size: {}x{}", GAMEVIEW_WIDTH, GAMEVIEW_HEIGHT);

        // Add fonts
        glyphsheets.try_emplace("metamorphous_small", create_glyphsheet(renderer.get(), "assets/font/metamorphous.ttf", FONT_SIZE_SMALL));
        glyphsheets.try_emplace("consola_small", create_glyphsheet(renderer.get(), "assets/font/consola.ttf", FONT_SIZE_SMALL));
        glyphsheets.try_emplace("metamorphous_large", create_glyphsheet(renderer.get(), "assets/font/metamorphous.ttf", FONT_SIZE_LARGE));
        glyphsheets.try_emplace("consola_large", create_glyphsheet(renderer.get(), "assets/font/consola.ttf", FONT_SIZE_LARGE));

        std::println("gfx was init.");
    }

    void GfxState::init_overmap_texture(const ChunkMap& chunks, const Tileset& world_tileset)
    {
        auto surface = Surface(create_surface(WORLD_CHUNKS, WORLD_CHUNKS), SDL_FreeSurface);
        uint32_t* pixels = (uint32_t*)surface->pixels;

        for (int y = 0; y < WORLD_CHUNKS; ++y)
        {
            for (int x = 0; x < WORLD_CHUNKS; ++x)
            {
                size_t idx = (y * surface->pitch / 4) + x;

                uint32_t colour = 0xff000000; // Black
                /*
                const auto& opt_chunk = chunks[x][y];
                if (opt_chunk.has_value())
                {
                    auto& chunk = opt_chunk.value();
                    int v = chunk.tiles[0][0];
                    if (chunk.type == "overworld")
                        colour = 0xff006600; // Forest green
                    else if (chunk.type == "town")
                        colour = 0xff444444; // Grey
                    else if (chunk.type == "sea")
                        colour = 0xffaa0000; // Blue
                }*/

                pixels[idx] = colour;
            }
        }

        textures.try_emplace(
            "_overmap",
            Texture(SDL_CreateTextureFromSurface(renderer.get(), surface.get()), SDL_DestroyTexture)
        );

        std::println("Added _overmap texture");
    }
}
