local Base = require('scripts.entities._base')

local Crow = {}
Crow.__index = Crow

setmetatable(Crow, { __index = Base })

function Crow:new()
    local obj = Base:new()

    obj.archetype = 'crow'
    
    obj.sprite.tileset_key = 'crow'

    setmetatable(obj, Crow)
    return obj
end

return Crow
