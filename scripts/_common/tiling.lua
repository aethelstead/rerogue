local M = {}

M.Type = {
    None = 0,

    Grass = 1,
    Floor = 2,
    Wall = 3,
    Sign = 4,
    Dungeon = 5,

    Water = 9,
    Water2 = 10,
    Tree = 11,
    Tree2 = 12,

    Chest = 17,
    Chest2 = 18,
    Tree3 = 19,
    Tree4 = 20,
    
    Door = 25,
    Door2 = 26,
    Table = 27,
    Stool = 28
}

M.gen_tiles = function (v, w, h)
    local tiles = {}
    
    for y = 0, h - 1 do
        for x = 0, w - 1 do
            local idx = (y * w) + x
            tiles[idx + 1] = v
        end
    end

    return tiles
end

M.blit_tiles = function (source, sourceWidth, target, targetWidth, start_idx)
    local x = 0
    local y = 0
    for k, v in ipairs(source)
    do
        local idx = start_idx + (y * targetWidth) + x
        target[idx] = v

        x = x + 1
        if x >= sourceWidth then
            x = 0
            y = y + 1
        end
    end
end

return M