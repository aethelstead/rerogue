local Base = require('scripts.entities._base')

local Rat = {}
Rat.__index = Rat

setmetatable(Rat, { __index = Base })

function Rat:new()
    local obj = Base:new()

    obj.archetype = 'rat'
    obj.sprite.tileset_key = 'rat'

    obj.stats.max_hp = 5
    obj.hp = obj.stats.max_hp

    setmetatable(obj, Rat)
    return obj
end

return Rat
