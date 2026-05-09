#pragma once

#include "_core/geometry.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_gamecontroller.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <array>
#include <climits>
#include <cstdlib>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "gfx/gfx.h"
#include "input/input.h"
#include "mixer/mixer.h"
#include "game/game.h"
#include "tilemap/tilemap.h"

namespace gin
{
    struct GuiState
    {
        Recti view;
        bool show_overmap = false;
        std::vector<std::string> all_messages;
        std::optional<std::string> opt_speech;
    };

    struct State
    {
        TilesetMap tilesets;

        GfxState gfx;
        InputState input;
        MixerState mix;
        GameState game;
        GuiState gui;

        bool init();

        bool events();

        bool update(double dt);
        
        void draw();
        void draw_game(const TextureMap& textures, const TilesetMap& tilesets);
        void draw_chunk_tiles(Vec2i& topleft, const WorldChunk& chunk, const Texture& texture, const Tileset& tileset);
        void draw_sprites(const TextureMap& textures, const TilesetMap& tilesets);

        void draw_gui();
        void draw_overmap();
    };
} // namespace gin
