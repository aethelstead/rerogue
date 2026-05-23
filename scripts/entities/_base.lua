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
        vel = { x = 0, y = 0 },
        speed = 1,
        is_wall = false,
        tileset_key = '???',
        state = 'idle',
        dir_key = 'south',
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

function Base:set_animation()
    gin.set_sprite_animation(self.eid, string.format(self.state .. '_' .. self.dir_key))
end

function Base:is_busy()
    return gin.is_phys_busy(self.eid)
end

function Base:set_busy(nframes)
    gin.set_phys_busy(self.eid, nframes)
end

function Base:face_north()
    if not self.is_busy(self) then
        self.dir.x = 0
        self.dir.y = -1
        self.state = 'idle'
        self.dir_key = 'north'
        self.set_animation(self)
        self.set_busy(self, 20)
    end
end

function Base:face_east()
    if not self.is_busy(self) then
        self.dir.x = 1
        self.dir.y = 0
        self.state = 'idle'
        self.dir_key = 'east'
        self.set_animation(self)
        self.set_busy(self, 20)
    end
end

function Base:face_south()
    if not self.is_busy(self) then
        self.dir.x = 0
        self.dir.y = 1
        self.state = 'idle'
        self.dir_key = 'south'
        self.set_animation(self)
        self.set_busy(self, 20)
    end
end

function Base:face_west()
    if not self.is_busy(self) then
        self.dir.x = -1
        self.dir.y = 0
        self.state = 'idle'
        self.dir_key = 'west'
        self.set_animation(self)
        self.set_busy(self, 20)
    end
end

function Base:walk()
    if not self.is_busy(self) then
        self.vel.x = self.dir.x
        self.vel.y = self.dir.y
        self.state = 'walk'
        self.set_animation(self)
    end
end

function Base:go_north()
    if self.dir.y < 0 then
        self.walk(self)
    else
        self.face_north(self)
    end
end

function Base:go_east()
    if self.dir.x > 0 then
        self.walk(self)
    else
        self.face_east(self)
    end
end

function Base:go_south()
    if self.dir.y > 0 then
        self.walk(self)
    else
        self.face_south(self)
    end
end

function Base:go_west()
    if self.dir.x < 0 then
        self.walk(self)
    else
        self.face_west(self)
    end
end

function Base:stop()
    self.vel.x = 0
    self.vel.y = 0
end

function Base:idle()
    if self.vel.x ~= 0 or self.vel.y ~= 0 then
        self.state = 'idle'
        self.set_animation(self)
    end
end

function Base:attack()
    if not self.is_busy(self) then
        print(self.archetype .. " attack!")
        self.state = 'attack'
        self.set_animation(self)
        self.set_busy(self, 100)
    end
end

function Base:tick(dt)
    local ms = math.floor(dt * 1000)
end

return Base
