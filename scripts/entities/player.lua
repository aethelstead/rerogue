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

function Player:interact()
    local opp = gin.get_opponent(self.eid)
    if opp ~= nil then
        opp.on_interact(opp)
    end
end

return Player
