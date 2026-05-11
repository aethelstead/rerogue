local Base = require('scripts.entities._base')

local Villager = {}
Villager.__index = Villager

setmetatable(Villager, { __index = Base })

function Villager:new()
    local obj = Base:new()
    
    obj.name = '*villager names goes here*'
    obj.archetype = 'villager'
    
    obj.sprite.tileset_key = 'human'

    setmetatable(obj, Villager)
    return obj
end

function Villager:interact()
    G.show_speech(self.name..': Greetings!')
end

return Villager
