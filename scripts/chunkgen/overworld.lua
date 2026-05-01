local Tiling = require('scripts._common.tiling')

local M = {}

local TREE_DENSITY = 6

M.genchunk = function (chunkX, chunkY)
    local chunk = {
        type = 'overworld',
        tiles = Tiling.gen_tiles(Tiling.Type.Grass, G.CHUNK_TILES, G.CHUNK_TILES)
    }

    -- Insert trees
    for y = 0, G.CHUNK_TILES - 1, 2 do
        for x = 0, G.CHUNK_TILES - 1, 2 do
            local rnd = math.random(TREE_DENSITY)
            if rnd == 1 then
                local idx = ((y * G.CHUNK_TILES) + x) + 1
                chunk.tiles[idx] = Tiling.Type.Tree
                chunk.tiles[idx + 1] = Tiling.Type.Tree2
                chunk.tiles[idx + G.CHUNK_TILES] = Tiling.Type.Tree3
                chunk.tiles[idx + G.CHUNK_TILES + 1] = Tiling.Type.Tree4
            end
        end
    end

    --G.spawn_entity('rat', (chunkX * G.CHUNK_TILES) + 21, (chunkY * G.CHUNK_TILES) + 21)
    --G.spawn_entity('rat', (chunkX * G.CHUNK_TILES) + 11, (chunkY * G.CHUNK_TILES) + 11)
    G.spawn_entity('rat', (chunkX * G.CHUNK_TILES), (chunkY * G.CHUNK_TILES) + 5)
    G.spawn_entity('rat', (chunkX * G.CHUNK_TILES), (chunkY * G.CHUNK_TILES) + 9)

    return chunk
end

return M
