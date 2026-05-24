local Tiling = require('scripts._common.tiling')

local M = {}

M.chunks = {}

M.gen_world = function()
    for y = 1, 32 do
        for x = 1, 32 do
            local idx = ((y - 1) * 32) + x
                M.chunks[idx] = {
                    type = 'sea',
                    layers = {
                        Tiling.gen_tiles(Tiling.Type.Water, gin.CHUNK_TILES, gin.CHUNK_TILES),
                        Tiling.gen_tiles(0, gin.CHUNK_TILES, gin.CHUNK_TILES),
                        Tiling.gen_tiles(0, gin.CHUNK_TILES, gin.CHUNK_TILES)
                    }
                }
        end
    end

    for y = 3, 30 do
        for x = 3, 30 do
            local idx = ((y - 1) * 32) + x
                M.chunks[idx] = {
                    type = 'overworld',
                    layers = {
                        Tiling.gen_tiles(Tiling.Type.Grass, gin.CHUNK_TILES, gin.CHUNK_TILES),
                        Tiling.gen_tiles(0, gin.CHUNK_TILES, gin.CHUNK_TILES),
                        Tiling.gen_tiles(0, gin.CHUNK_TILES, gin.CHUNK_TILES)
                    }
                }
        end
    end

    --[[
    local n_towns = math.random(4) + 6
    for i = 1, n_towns do
        local x = math.random(24) + 4
        local y = math.random(24) + 4
        local idx = ((y - 1) * 32) + x
        M.chunks[idx] = Town.genchunk(x, y)
    end]]--
    
    --local idx = ((4 - 1) * 32) + 4
    --M.chunks[idx] = Town.genchunk(4, 4)

    --M.chunks[idx].tiles[1] = Tiling.Type.Dungeon
end

return M
