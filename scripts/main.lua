local M = {}

local player = {}

function gin.init()
    -- init gfx
    gin.gfx.set_window_title('rerogue')
    gin.gfx.resize_window(1280, 960)

    -- init mix
    gin.mix.set_master_vol(gin.mix.MAX_VOL)
    gin.mix.set_sfx_vol(gin.mix.MAX_VOL)
    gin.mix.set_bgm_vol(gin.mix.MAX_VOL)
    --gin.mix.mute()
end

function gin.update()

end

function gin.on_world_load()
    player = gin.spawn_entity("player", 100, 100)
end

function gin.io.key_down(key, is_repeat)
    
    if key == gin.io.KEYBOARD.KeyP then
        gin.gui.toggle_pause()
    elseif key == gin.io.KEYBOARD.KeyM then
        gin.gui.toggle_overmap()
    elseif key == gin.io.KEYBOARD.KeyW then
        if is_repeat then
            player.walk()
        else
            player.face_north()
        end
    elseif key == gin.io.KEYBOARD.KeyD then
        if is_repeat then
            player.walk()
        else
            player.face_east()
        end
    elseif key == gin.io.KEYBOARD.KeyS then
        if is_repeat then
            player.walk()
        else
            player.face_south()
        end
    elseif key == gin.io.KEYBOARD.KeyA then
        if is_repeat then
            player.walk()
        else
            player.face_west()
        end
    end
end

function gin.io.pad_down(key, is_repeat)
    
    if key == gin.io.GAMEPAD.BtnStart then
        gin.gui.toggle_pause()
    elseif key == gin.io.GAMEPAD.BtnY then
        gin.gui.toggle_overmap()
    elseif key == gin.io.GAMEPAD.DpadUp then
        if is_repeat then
            player.walk()
        else
            player.face_north()
        end
    elseif key == gin.io.GAMEPAD.DpadRight then
        if is_repeat then
            player.walk()
        else
            player.face_east()
        end
    elseif key == gin.io.GAMEPAD.DpadDown then
        if is_repeat then
            player.walk()
        else
            player.face_south()
        end
    elseif key == gin.io.GAMEPAD.DpadLeft then
        if is_repeat then
            player.walk()
        else
            player.face_west()
        end
    end
end

return M
