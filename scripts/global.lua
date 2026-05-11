G = {
    WINDOW_TITLE = 'rerogue',
    WINDOW_WIDTH = 1280,
    WINDOW_HEIGHT = 960,

    GAMEVIEW_WIDTH = 320,
    GAMEVIEW_HEIGHT = 240,

    TILE_PIXELS = 16,
    CHUNK_TILES = 32,
    CHUNK_PIXELS = 512,
    WORLD_CHUNKS = 100,
    WORLD_PIXELS = 51200,

    EntityCommand = {
        None = 0,

        FaceNorth = 1,
        FaceEast = 2,
        FaceSouth = 3,
        FaceWest = 4,

        WalkNorth = 5,
        WalkEast = 6,
        WalkSouth = 7,
        WalkWest = 8,

        Attack = 9,
        Interact = 10,
    },

    EntityState = {
        Idle = 0,
        Walking = 1,
        Attacking = 2,
        Interacting = 3
    },

    BtnState = {
        None = 0,
        Down = 1,
        Held = 2,
        Released = 3
    }
}

-- Overriden by C++
G.push_message = nil
G.show_speech = nil
G.toggle_overmap = nil
G.toggle_paused = nil
G.play_sfx = nil

function G.spawn_entity(archetype, tx, ty) end
function G.spawn_player(tx, ty) end
G.reap_entity = nil
function G.command_entity(eid, cmd) end
function G.command_player(cmd) end

G.face_opponent = nil

G.set_entity_animation = nil
G.set_entity_collision = nil

-- game.lua

