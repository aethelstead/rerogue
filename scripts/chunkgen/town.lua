local Tiling = require('scripts._common.tiling')

local M = {}

M.genchunk = function (chunkX, chunkY)
    local chunk = {
        type = 'town',
        tiles = Tiling.gen_tiles(Tiling.Type.Grass, G.CHUNK_TILES, G.CHUNK_TILES)
    }

    local house_tiles = {
        3, 3, 3, 3, 3, 3, 3,
        3, 2, 2, 2, 2, 9, 3,
        3, 2, 2, 2, 2, 2, 3,
        3, 2, 2, 2, 2, 2, 3,
        3, 3, 3, 13, 3, 3, 3,
    }
    local start_idx = (11 * 32) + 8
    Tiling.blit_tiles(house_tiles, 7, chunk.tiles, 32, start_idx)
    start_idx = (11 * 32) + 18
    Tiling.blit_tiles(house_tiles, 7, chunk.tiles, 32, start_idx)
    start_idx = (21 * 32) + 8
    Tiling.blit_tiles(house_tiles, 7, chunk.tiles, 32, start_idx)
    start_idx = (21 * 32) + 18
    Tiling.blit_tiles(house_tiles, 7, chunk.tiles, 32, start_idx)


    G.spawn_entity('villager', 41, 44)
    G.spawn_entity('villager', 51, 44)
    G.spawn_entity('villager', 41, 54)
    G.spawn_entity('villager', 51, 54)

    --G.spawn_entity('rat', 640, 640)
    --G.spawn_entity('rat', 642, 634)
    --G.spawn_entity('rat', 644, 636)
    --G.spawn_entity('rat', 640, 638)
    --G.spawn_entity('rat', 648, 640)

    return chunk
end

return M
