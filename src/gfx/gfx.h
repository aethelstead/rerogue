#pragma once

#include <string>
#include <memory>
#include <functional>
#include <unordered_map>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include "../_core/geometry.h"
#include "../tilemap/tilemap.h"

namespace gin
{
    inline const int GAMEVIEW_WIDTH = 320;
    inline const int GAMEVIEW_HEIGHT = 240;

    inline const int FONT_SIZE_SMALL = 16;
    inline const int FONT_SIZE_LARGE = 32;

    inline const int MESSAGES_HEIGHT = 160;
    inline const int MESSAGES_LINES = 6;

    // The square root of the total number of chunks that fit into the Game view e.g: 2x2
    inline const int SQRT_VIEW_CHUNKS = 2;

    using Window = std::unique_ptr<SDL_Window, std::function<void(SDL_Window*)>>;
    using Renderer = std::unique_ptr<SDL_Renderer, std::function<void(SDL_Renderer*)>>;

    using Surface = std::unique_ptr<SDL_Surface, std::function<void(SDL_Surface*)>>;

    using Texture = std::unique_ptr<SDL_Texture, std::function<void(SDL_Texture*)>>;
    using TextureMap = std::unordered_map<std::string, Texture>;

    using Font = std::unique_ptr<TTF_Font, std::function<void(TTF_Font*)>>;
    struct Glyphsheet
    {
        int ptsize = 0;
        int width = 0;
        int height = 0;
        int ncells = 0;
        int cols = 0;

        Font font;
        Texture texture;
    };
    using GlyphsheetMap = std::unordered_map<std::string, Glyphsheet>;

    struct GfxState
    {
        Window window;
        Renderer renderer;
        TextureMap textures;
        GlyphsheetMap glyphsheets;

        void init();
        void init_overmap_texture(const ChunkMap& chunks, const Tileset& world_tileset);

        int get_text_width(std::string_view text, std::string_view glyphs_key);

        void draw_text(std::string_view text, std::string_view glyphs_key, int x, int y);
    };
}