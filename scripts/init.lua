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
    }
}

-- Set by C++
G.push_message = nil
G.show_speech = nil
G.show_overmap = nil
G.play_sfx = nil

G.spawn_entity = nil
G.reap_entity = nil
G.command_entity = nil
G.command_player = nil
G.face_opponent = nil

G.set_entity_animation = nil
G.set_entity_collision = nil
