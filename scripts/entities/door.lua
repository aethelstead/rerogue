local Base = require('scripts.entities._base')

local Door = {}
Door.__index = Door

setmetatable(Door, { __index = Base })

function Door:new()
    local obj = Base:new()

    obj.archetype = 'door'

    obj.sprite.tileset_key = 'world'
    obj.sprite.anim_key = 'door_closed'

    obj.open = false

    setmetatable(obj, Door)
    return obj
end

function Door:interact()
    if self.open then
        self.open = false
        self.sprite.set_animation('door_closed')
        self.collision.is_wall = true
    else 
        self.open = true
        self.sprite.set_animation('door_open')
        self.collision.is_wall = false
    end

    G.play_sfx('assets/sfx/caseOpen.wav')
end

return Door
