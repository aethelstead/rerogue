local M = {}

-- These must match SDL2!
local VKEYS = {
    BtnA = 0,
    BtnB = 1,
    BtnX = 2,
    BtnY = 3,
    BtnBack = 4,
    BtnGuide = 5,
    BtnStart = 6,
    BtnLeftStick = 7,
    BtnRightStick = 8,
    BtnLeftShldr = 9,
    BtnRightShldr = 10,
    DpadUp = 11,
    DpadDown = 12,
    DpadLeft = 13,
    DpadRight = 14,
    -- 15 to 21 reserved by SDL2
    Number1 = 31,
    Number2 = 32,
    Number3 = 33,
    Number4 = 34,
    Number5 = 35,
    Number6 = 36,
    Number7 = 37,
    Number8 = 38,
    Number9 = 39,
    Number0 = 40,
}

local function bind_vkey(downFn, heldFn, upFn)
    local NOOP = function() end

    if downFn == nil then
        downFn = NOOP
    end
    if heldFn == nil then
        heldFn = NOOP
    end
    if upFn == nil then
        upFn = NOOP
    end
        
    return { downFn = downFn, heldFn = heldFn, upFn = upFn }
end


function M.bind()
    local vkeys = {}

    vkeys[VKEYS.BtnA] = bind_vkey(
        function() G.command_player(G.EntityCommand.Interact) end
    )
    vkeys[VKEYS.BtnB] = bind_vkey()
    vkeys[VKEYS.BtnX] = bind_vkey()
    vkeys[VKEYS.BtnY] = bind_vkey(
        function() G.show_overmap() end
    )

    vkeys[VKEYS.DpadUp] = bind_vkey(
        function() G.command_player(G.EntityCommand.FaceNorth) end,
        function() G.command_player(G.EntityCommand.WalkNorth) end
    )
    vkeys[VKEYS.DpadDown] = bind_vkey(
        function() G.command_player(G.EntityCommand.FaceSouth) end,
        function() G.command_player(G.EntityCommand.WalkSouth) end
    )
    vkeys[VKEYS.DpadLeft] = bind_vkey(
        function() G.command_player(G.EntityCommand.FaceWest) end,
        function() G.command_player(G.EntityCommand.WalkWest) end
    )
    vkeys[VKEYS.DpadRight] = bind_vkey(
        function() G.command_player(G.EntityCommand.FaceEast) end,
        function() G.command_player(G.EntityCommand.WalkEast) end
    )

    vkeys[VKEYS.BtnLeftShldr] = bind_vkey()
    vkeys[VKEYS.BtnRightShldr] = bind_vkey(
        function() G.command_player(G.EntityCommand.Attack) end
    )

    return vkeys
end

return M
