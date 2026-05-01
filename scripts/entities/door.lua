local Base = require('scripts.entities._base')

local Door = {}
Door.__index = Door

setmetatable(Door, { __index = Base })

function Door:new()
    local obj = Base:new()

    obj.name = 'door'
    obj.type = 'door'
    obj.tileset_key = 'world'
    obj.anim_key = 'door_closed'

    obj.open = false

    setmetatable(obj, Door)
    return obj
end

function Door:interact()
    if self.open then
        self.open = false
        G.set_entity_animation(self.eid, 'door_closed')
        G.set_entity_collision(self.eid, true)
    else 
        self.open = true
        G.set_entity_animation(self.eid, 'door_open')  
        G.set_entity_collision(self.eid, false)
    end
    G.play_sfx('assets/sfx/caseOpen.wav')
end

return Door
