local Base = require('scripts.entities._base')

local Chest = {}
Chest.__index = Chest

setmetatable(Chest, { __index = Base })

function Chest:new()
    local obj = Base:new()

    obj.archetype = 'chest'

    obj.sprite.tileset_key = 'world'
    obj.sprite.anim_key = 'chest_closed'

    setmetatable(obj, Chest)
    return obj
end

function Chest:interact()
    --G.play_sfx('assets/sfx/caseOpen.wav')
    --G.set_entity_animation(self.eid, 'chest_open')
    --G.push_message('Chest contained X gold pieces')
end

return Chest
