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
    player = gin.spawn_entity("player", 100, 100)
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
    elseif key == gin.io.KEYBOARD.KeyE then
        player.interact(player)
    elseif key == gin.io.KEYBOARD.KeySpace then
        player.attack(player)
    elseif key == gin.io.KEYBOARD.KeyP then
        gin.gui.toggle_pause()
    elseif key == gin.io.KEYBOARD.KeyM then
        gin.gui.toggle_overmap()
    end
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
    elseif key == gin.io.GAMEPAD.BtnA then
        player.interact(player)
    elseif key == gin.io.GAMEPAD.BtnRightShldr then
        player.attack(player)
    elseif key == gin.io.GAMEPAD.BtnStart then
        gin.gui.toggle_pause()
    elseif key == gin.io.GAMEPAD.BtnY then
        gin.gui.toggle_overmap()
    end
end

return M
