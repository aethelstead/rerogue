local Base = require('scripts.entities._base')

local Crow = {}
Crow.__index = Crow

setmetatable(Crow, { __index = Base })

function Crow:new()
    local obj = Base:new()

    obj.name = 'crow'
    obj.type = 'crow'
    obj.tileset_key = 'crow'

    setmetatable(obj, Crow)
    return obj
end

return Crow
