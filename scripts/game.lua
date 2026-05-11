function G.on_init()
    G.spawn_player(100, 100)
end

function G.on_input(key, pad)
    local handle_keyboard = function(key)
        if key.code == 1 and key.state == 1 then

        end
    end

    local handle_gamepad = function(pad)
        if pad.code == 1 and pad.state == 1 then
            
        end
    end

    handle_keyboard(key)
    handle_gamepad(pad)
    
end

function G.on_update(dt)

end
