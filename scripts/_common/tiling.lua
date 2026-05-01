local M = {}

M.Type = {
    None = 0,

    Grass = 1,
    Floor = 2,
    Wall = 3,
    Sign = 4,
    Water = 5,
    Water2 = 6,
    Tree = 7,
    Tree2 = 8,
    Chest = 9,
    Chest2 = 10,
    Tree3 = 11,
    Tree4 = 12,
    Door = 13,
    Door2 = 14,
    Table = 15,
    Stool = 16
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