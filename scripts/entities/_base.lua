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
        state_key = 'idle',
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

    obj.state_trans['idle'] = {
        animation = string.format("idle_"..self.dir_key)
    }
    obj.state_trans['walk'] = {
        animation = string.format("walk_"..self.dir_key)
    }
    obj.state_trans['attack'] = {
        animation = string.format("walk_"..self.dir_key)
        key_frame_idx = 2,
        on_key_frame = function(you, them) end
    }
    obj.state_trans['interact'] = {
        animation = "attack",
        key_frame_idx = 1,
        on_key_frame = function(you, them) end
    }

    setmetatable(obj, self)
    return obj
end

function Base:is_idle()
    return self.state == gin.ENT_STATE.Idle
end

function Base:set_idle()
    print('IDLE')
    self.state = gin.ENT_STATE.Idle
    self.vel.x = 0
    self.vel.y = 0
end

function Base:go_north()
    if self.is_idle(self) then
        if self.dir.y < 0 then
            -- Set the direction and walk
            self.dir.x = 0
            self.dir.y = -1
            self.vel.x = self.dir.x
            self.vel.y = self.dir.y
            self.state = gin.ENT_STATE.Walking
        else
            -- Just set the direction
            self.dir.x = 0
            self.dir.y = -1
        end
    end
end

function Base:go_east()
    if self.is_idle(self) then
        if self.dir.x > 0 then
            -- Set the direction and walk
            self.dir.x = 1
            self.dir.y = 0
            self.vel.x = self.dir.x
            self.vel.y = self.dir.y
            self.state = gin.ENT_STATE.Walking
        else
            -- Just set the direction
            self.dir.x = 1
            self.dir.y = 0
        end
    end
end

function Base:go_south()
    if self.is_idle(self) then
        if self.dir.y > 0 then
            -- Set the direction and walk
            self.dir.x = 0
            self.dir.y = 1
            self.vel.x = self.dir.x
            self.vel.y = self.dir.y
            self.state = gin.ENT_STATE.Walking
        else
            -- Just set the direction
            self.dir.x = 0
            self.dir.y = 1
        end
    end
end

function Base:go_west()
    if self.is_idle(self) then
        if self.dir.x < 0 then
            -- Set the direction and walk
            self.dir.x = -1
            self.dir.y = 0
            self.vel.x = self.dir.x
            self.vel.y = self.dir.y
            self.state = gin.ENT_STATE.Walking
        else
            -- Just set the direction
            self.dir.x = -1
            self.dir.y = 0
        end
    end
end

function Base:attack()
    print(self.archetype .. " attack!")
    self.state = gin.ENT_STATE.Attacking
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

end]] --

return Base
