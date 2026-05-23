local M = {}

local player = {}

function gin.init()
    -- init gfx
    gin.gfx.set_window_title('rerogue')
    gin.gfx.resize_window(1280, 960)
    gin.gfx.set_vsync(true)

    -- init mix
    gin.mix.set_master_vol(gin.mix.MAX_VOL)
    gin.mix.set_sfx_vol(gin.mix.MAX_VOL)
    gin.mix.set_bgm_vol(gin.mix.MAX_VOL)
    --gin.mix.mute()
end

function gin.update()

end

function gin.on_world_load()
    player = gin.spawn_entity("player", 68, 68)
end

function gin.io.key_down(key, is_repeat)
    
    if key == gin.io.KEYBOARD.KeyW then
        player.go_north(player)
    elseif key == gin.io.KEYBOARD.KeyD then
        player.go_east(player)
    elseif key == gin.io.KEYBOARD.KeyS then
        player.go_south(player)
    elseif key == gin.io.KEYBOARD.KeyA then
        player.go_west(player)
    elseif key == gin.io.KEYBOARD.KeyE and not is_repeat then
        player.interact(player)
    elseif key == gin.io.KEYBOARD.KeySpace and not is_repeat then
        player.attack(player)
    elseif key == gin.io.KEYBOARD.KeyP and not is_repeat then
        gin.gui.toggle_pause()
    elseif key == gin.io.KEYBOARD.KeyM and not is_repeat then
        gin.gui.toggle_overmap()
    end
end

function gin.io.key_up(key)
end

function gin.io.pad_down(key, is_repeat)

    if key == gin.io.GAMEPAD.DpadUp then
        player.go_north(player)
    elseif key == gin.io.GAMEPAD.DpadRight then
        player.go_east(player)
    elseif key == gin.io.GAMEPAD.DpadDown then
        player.go_south(player)
    elseif key == gin.io.GAMEPAD.DpadLeft then
        player.go_west(player)
    elseif key == gin.io.GAMEPAD.BtnA and not is_repeat then
        player.interact(player)
    elseif key == gin.io.GAMEPAD.BtnRightShldr and not is_repeat then
        player.attack(player)
    elseif key == gin.io.GAMEPAD.BtnStart and not is_repeat then
        gin.gui.toggle_pause()
    elseif key == gin.io.GAMEPAD.BtnY and not is_repeat then
        gin.gui.toggle_overmap()
    end
end

function gin.io.pad_up(key)
end

return M
