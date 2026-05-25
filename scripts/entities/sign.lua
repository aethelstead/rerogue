local Base = require('scripts.entities._base')

local Sign = {}
Sign.__index = Sign

setmetatable(Sign, { __index = Base })

function Sign:new()
    local obj = Base:new()

    obj.archetype = 'sign'
    --obj.tileset_key = 'rat'

    --obj.stats.max_hp = 5
    --obj.hp = obj.stats.max_hp

    setmetatable(obj, Sign)
    return obj
end

function Sign:on_interact()
    print('YOU READ SIGN')
end

return Sign
