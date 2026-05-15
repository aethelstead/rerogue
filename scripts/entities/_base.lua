local Base = {}
Base.__index = Base

local prev_id = 0

function Base:new()
    prev_id = prev_id + 1
    local obj = {
        eid = prev_id,
        archetype = '???',
        tile = { x = 0, y = 0 },
        dir = { x = 0, y = 1 },
        speed = 1,
        is_wall = false,
        tileset_key = '???',
        anim_key = 'idle_south',
        hp = 10,
        stats = {
            max_hp = 10,
            max_sp = 10,
            max_mp = 0,

            strength = 1,
            vitality = 1,
            endurance = 1,
        }
    }

    setmetatable(obj, self)
    return obj
end

function Base:face_north()
    self.dir.x = 0
    self.dir.y = -1
end
function Base:face_east()
    self.dir.x = 1
    self.dir.y = 0
end
function Base:face_south()
    self.dir.x = 0
    self.dir.y = 1
end
function Base:face_west()
    self.dir.x = -1
    self.dir.y = 0
end

function Base:walk()
    self.vel = self.dir
end

function Base:tick(dt)
    local ms = math.floor(dt * 1000)
end

--[[
function Base:swing()
    --G.play_sfx('assets/sfx/dullSwoosh1.wav')
end

function Base:hit()
    --G.play_sfx('assets/sfx/click.wav')
end

function Base:hurt()
    self.hp = self.hp - 1
    if self.hp <= 0 then
        --G.reap_entity(self.eid)
    end
end

function Base:interact()
    --G.push_message('???')
end

function Base:think(opps)

end]]--

return Base
