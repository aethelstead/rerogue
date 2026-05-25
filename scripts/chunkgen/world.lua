local Tiling = require('scripts._common.tiling')

local M = {}

M.chunks = {}

M.gen_world = function()
    -- Sea
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

    -- Land
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


    -- Trees
    --[[
    for y = 3, 30 do
        for x = 3, 30 do
            local chidx = ((y - 1) * 32) + x
            for y = 0, 32 - 1, 2 do
                for x = 0, 32 - 1, 2 do

                    local rnd = math.random(1, 10)
                    if rnd == 1 then
                        local idx = (y * 32) + x
                        M.chunks[chidx].layers[2][idx + 1] = 11
                        M.chunks[chidx].layers[2][idx + 2] = 12
                        M.chunks[chidx].layers[2][idx + 1 + 32] = 19
                        M.chunks[chidx].layers[2][idx + 2 + 32] = 20
                    end

                end
            end
        end
    end]]--

    local chidx = (2 * 32) + 3;
    M.chunks[chidx].layers[2][35] = 11;
    M.chunks[chidx].layers[2][36] = 12;
    M.chunks[chidx].layers[2][35 + 32] = 19;
    M.chunks[chidx].layers[2][36 + 32] = 20;


    M.chunks[chidx].layers[2][37 + 96] = Tiling.Type.Dungeon
    M.chunks[chidx].layers[2][39 + 96] = Tiling.Type.Sign

end

return M
