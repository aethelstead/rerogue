local Base = {}
Base.__index = Base

local prev_id = 0

function Base:new()
    prev_id = prev_id + 1
    local obj = {
        eid = prev_id,
        archetype = '???',
        position = {
            tile = { x = 0, y = 0 },
            dir = { x = 0, y = 1 }
        },
        movement = {
            speed = 1
        },
        collision = {
            is_wall = false
        },
        stateful = {

        },
        sprite = {
            tileset_key = '???',
            anim_key = 'idle_south'
        },
        stats = {
            max_hp = 10,
            max_sp = 10,
            max_mp = 0,
    
            strength = 1,
            vitality = 1,
            endurance = 1
        },
        hp = 10
    }

    setmetatable(obj, self)
    return obj
end

function Base:swing()
    G.play_sfx('assets/sfx/dullSwoosh1.wav')
end
function Base:hit()
    G.play_sfx('assets/sfx/click.wav')
end
function Base:hurt()
    self.hp = self.hp - 1
    if self.hp <= 0 then
        G.reap_entity(self.eid)
    end
end

function Base:interact()
    G.push_message('???')
end

function Base:think(opps)
    
end

return Base
