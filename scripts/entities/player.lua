local Base = require('scripts.entities._base')

local Player = {}
Player.__index = Player

setmetatable(Player, { __index = Base })

function Player:new()
    local obj = Base:new()

    --obj.name = '*Name Goes Here*'
    obj.archetype = 'player'

    obj.tileset_key = 'human'

    setmetatable(obj, Player)
    return obj
end

return Player
