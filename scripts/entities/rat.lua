local Base = require('scripts.entities._base')

local Rat = {}
Rat.__index = Rat

setmetatable(Rat, { __index = Base })

function Rat:new()
    local obj = Base:new()

    obj.archetype = 'rat'
    obj.sprite.tileset_key = 'rat'

    obj.stats.max_hp = 5
    obj.hp = obj.stats.max_hp

    setmetatable(obj, Rat)
    return obj
end

function Rat:interact()
    G.push_message(self.name .. ' ignored you')
end

function Rat:think(opps)
    for idx, opp in ipairs(opps) do
        if opp.archetype ~= 'player' then
            break
        end
        
        G.face_opponent(self.eid, opp.eid)
    end

    -- random number between 1 and 4 for direction
    local rnd = math.random(300) + 1
    if rnd == 1 then
        G.command_entity(self.eid, G.EntityCommand.WalkNorth)
    elseif rnd == 2 then
        G.command_entity(self.eid, G.EntityCommand.WalkEast)
    elseif rnd == 3 then
        G.command_entity(self.eid, G.EntityCommand.WalkSouth)
    elseif rnd == 4 then
        G.command_entity(self.eid, G.EntityCommand.WalkWest)
    else
        -- Do nothing
    end
end

return Rat
