#pragma once

#include "geometry.h"
#include "sol/sol.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_gamecontroller.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <array>
#include <climits>
#include <cstdlib>
#include <format>
#include <iostream>
#include <memory>
#include <optional>
#include <print>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace gin
{
    using Window = std::unique_ptr<SDL_Window, std::function<void(SDL_Window*)>>;

    using Renderer = std::unique_ptr<SDL_Renderer, std::function<void(SDL_Renderer*)>>;

    using Surface = std::unique_ptr<SDL_Surface, std::function<void(SDL_Surface*)>>;

    using Texture = std::unique_ptr<SDL_Texture, std::function<void(SDL_Texture*)>>;
    using TextureMap = std::unordered_map<std::string, Texture>;

    using Font = std::unique_ptr<TTF_Font, std::function<void(TTF_Font*)>>;

    using Bgm = std::unique_ptr<Mix_Music, std::function<void(Mix_Music*)>>;
    using BgmMap = std::unordered_map<std::string, Bgm>;

    using Sfx = std::unique_ptr<Mix_Chunk, std::function<void(Mix_Chunk*)>>;
    using SfxMap = std::unordered_map<std::string, Sfx>;

    using Gamepad = std::unique_ptr<SDL_GameController, std::function<void(SDL_GameController*)>>;

    using EntityId = uint;

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

    enum class ButtonState : int
    {
        None = 0,

        Down,
        Up
    };

    // For input, just pretend that the keyboard is a 360 controller
    inline const std::unordered_map<SDL_Keycode, SDL_GameControllerButton> keycode_map = {
        {SDLK_w, SDL_CONTROLLER_BUTTON_DPAD_UP},
        {SDLK_s, SDL_CONTROLLER_BUTTON_DPAD_DOWN},
        {SDLK_a, SDL_CONTROLLER_BUTTON_DPAD_LEFT},
        {SDLK_d, SDL_CONTROLLER_BUTTON_DPAD_RIGHT},
        {SDLK_e, SDL_CONTROLLER_BUTTON_A},
        {SDLK_f, SDL_CONTROLLER_BUTTON_B},
        {SDLK_q, SDL_CONTROLLER_BUTTON_X},
        {SDLK_r, SDL_CONTROLLER_BUTTON_Y},
        {SDLK_RETURN, SDL_CONTROLLER_BUTTON_START},
        {SDLK_TAB, SDL_CONTROLLER_BUTTON_BACK},
        {SDLK_1, SDL_CONTROLLER_BUTTON_LEFTSHOULDER},
        {SDLK_4, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER},
    };

    inline const std::unordered_map<SDL_GameControllerButton, std::string> padbtn_map = {

        {SDL_CONTROLLER_BUTTON_DPAD_UP, "up"},
        {SDL_CONTROLLER_BUTTON_DPAD_DOWN, "down"},
        {SDL_CONTROLLER_BUTTON_DPAD_LEFT, "left"},
        {SDL_CONTROLLER_BUTTON_DPAD_RIGHT, "right"},
        {SDL_CONTROLLER_BUTTON_A, "a"},
        {SDL_CONTROLLER_BUTTON_B, "b"},
        {SDL_CONTROLLER_BUTTON_X, "x"},
        {SDL_CONTROLLER_BUTTON_Y, "y"},
        {SDL_CONTROLLER_BUTTON_START, "start"},
        {SDL_CONTROLLER_BUTTON_BACK, "back"},
        {SDL_CONTROLLER_BUTTON_LEFTSHOULDER, "l1"},
        {SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, "r1"}};

    // @TODO: Take from init.lua?
    inline const int TILE_PIXELS = 16;
    inline const int CHUNK_TILES = 32;
    inline const int CHUNK_PIXELS = CHUNK_TILES * TILE_PIXELS;
    inline const int WORLD_CHUNKS = 32;
    inline const int WORLD_TILES = WORLD_CHUNKS * CHUNK_TILES;
    inline const int WORLD_PIXELS = WORLD_CHUNKS * CHUNK_PIXELS;

    // The square root of the total number of chunks that fit into the Game view e.g: 2x2
    inline const int SQRT_VIEW_CHUNKS = 2;

    inline const int SYS_FONT_SIZE = 16;
    inline const int GAME_FONT_SIZE = 16;

    inline const int MESSAGES_HEIGHT = 160;
    inline const int MESSAGES_LINES = 6;

    inline const int ENTITY_TILE_RANGE = 4;

    using Tilemap = std::array<std::array<int, CHUNK_TILES>, CHUNK_TILES>;
    struct WorldChunk
    {
        Tilemap tiles;
        std::string type;

        WorldChunk() {}

        WorldChunk(Tilemap& t) : tiles(t) {}
    };

    struct TileData
    {
        bool solid = false;
        int duration = 0;
        int frameidx = 0;
        int framems = 0;
        std::string anim_key;
        std::vector<int> frames;
        std::optional<std::string> entity_key;
    };
    using TileDataMap = std::unordered_map<int, TileData>;
    using TileAnimationMap = std::unordered_map<std::string, int>;

    struct Tileset
    {
        int n_cells = 0;
        int cells_per_row = 0;
        std::string texture_key;
        TileDataMap tiles;
        TileAnimationMap anim_map;
    };
    using TilesetMap = std::unordered_map<std::string, Tileset>;

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

    struct Collision 
    {
        Vec2i dir;
        std::vector<sol::table> opps;
    };

    struct Entity
    {
        EntityId eid;

        Vec2i pos;
        Vec2i walk_pos;
        Vec2i dir;
        int speed = 0;
        std::string type;
        
        Vec2i tile_pos;
        Vec2i chunk_pos;
        
        sol::table table;
        std::string anim_key;

        EntityCommand command = EntityCommand::None;
        EntityState state = EntityState::Idle;

        int cooldown = 0;
        bool reap = false;
        Collision collision;
        std::optional<EntityId> opps_in_range;

        bool is_collide = true;

        Entity(EntityId eid, Vec2i pos, std::string_view type, sol::table table, std::string_view anim_key) : eid(eid), pos(pos), dir(Vec2i::south()), type(type), table(table), anim_key(anim_key)
        {
            tile_pos.x = pos.x / TILE_PIXELS;
            tile_pos.y = pos.y / TILE_PIXELS;

            chunk_pos.x = pos.x / CHUNK_PIXELS;
            chunk_pos.x = pos.x / CHUNK_PIXELS;

            speed = 1;

            walk_pos = pos;
        }
    };

    struct Sprite
    {
        std::string tileset_key;
        Vec2i view_pos;
        int frameidx = 0;
        int framems = 0;
        int anim_id = 0;

        Sprite(std::string_view tileset_key) : tileset_key(tileset_key) {}
    };

    using ChunkMap = std::array<std::array<std::optional<WorldChunk>, WORLD_CHUNKS>, WORLD_CHUNKS>;
    struct GameState
    {
        Recti camera;
        ChunkMap chunks;

        EntityId player_eid = 0;

        std::unordered_map<EntityId, Entity> entities;
        std::unordered_map<EntityId, Sprite> sprites;

        std::vector<EntityId> sim_ents;
    };

    struct PerfCounter
    {
        const int TARGET_FPS = 60;
        double dt = 0.0;
        int frame_count = 0;
        double frame_ms = 0;
        int avg_fps = 0;
        double game_ms = 0;
    };

    struct Assets
    {
        TextureMap textures;
        BgmMap bgms;
        SfxMap sfxs;
    };

    struct VKey
    {
        sol::function downFn;
        sol::function heldFn;
        sol::function upFn;

        bool down = false;
        bool prev_down = false;
    };

    struct State
    {
        sol::state L;
        Window window;
        Renderer renderer;
        Assets assets;
        PerfCounter perf_counter;

        Glyphsheet sys_glyphs;
        Glyphsheet game_glyphs;

        TilesetMap tilesets;

        Texture game_texture;
        Texture overmap_texture;
        bool is_paused = false;
        bool show_overmap = false;

        GameState game;

        std::vector<std::string> all_messages;
        std::optional<std::string> opt_speech;
        
        Gamepad gamepad;
        std::unordered_map<int, VKey> vkeys;
    };

    bool init(State& state);

    bool events(State& state);

    bool update(State& state);

    void draw(State& state);

    void command_entity(EntityId eid, EntityCommand command, GameState& game);
    void set_entity_animation(EntityId eid, std::string_view anim_key, GameState& game);

    EntityId spawn_entity(State& state, std::string_view type, Vec2i tile_pos);

} // namespace gin
