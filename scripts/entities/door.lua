local Base = require('scripts.entities._base')

local Door = {}
Door.__index = Door

setmetatable(Door, { __index = Base })

function Door:new()
    local obj = Base:new()

    obj.archetype = 'door'

    obj.tileset_key = 'world'
    --obj.anim_key = 'door_closed'

    obj.open = false

    setmetatable(obj, Door)
    return obj
end

function Door:on_interact()
    self.open = not self.open

    if self.open then
        gin.set_sprite_animation(self.eid, 'door_closed')
    else
        gin.set_sprite_animation(self.eid, 'door_open')
    end

    gin.mix.play_sfx('assets/sfx/caseOpen.wav')
end

return Door
