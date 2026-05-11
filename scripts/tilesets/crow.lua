local M = {}

M.texture_key = 'assets/img/crow.png'

M.n_cells = 36
M.cells_per_row = 6

M.tiles = {}

M.tiles[1] = { is_wall = true, frames = { 1 }, anim_key = 'idle_south', duration = 100 }
M.tiles[2] = { is_wall = true, frames = { 1, 2, 1, 3 }, anim_key = 'walk_south', duration = 100 }
M.tiles[4] = { is_wall = true, frames = { 1, 4, 5, 4 }, anim_key = 'attack_south', duration = 100 }

M.tiles[7] = { is_wall = true, frames = { 7 }, anim_key = 'idle_north', duration = 100 }
M.tiles[8] = { is_wall = true, frames = { 7, 8, 7, 9 }, anim_key = 'walk_north', duration = 100 }
M.tiles[10] = { is_wall = true, frames = { 7, 10, 11, 7 }, anim_key = 'attack_north', duration = 100 }

M.tiles[13] = { is_wall = true, frames = { 13 }, anim_key = 'idle_east', duration = 100 }
M.tiles[14] = { is_wall = true, frames = { 13, 14, 13, 15 }, anim_key = 'walk_east', duration = 100 }
M.tiles[16] = { is_wall = true, frames = { 13, 16, 17, 13 }, anim_key = 'attack_east', duration = 100 }

M.tiles[19] = { is_wall = true, frames = { 19 }, anim_key = 'idle_west', duration = 100 }
M.tiles[20] = { is_wall = true, frames = { 19, 20, 19, 21 }, anim_key = 'walk_west', duration = 100 }
M.tiles[22] = { is_wall = true, frames = { 19, 22, 23, 19}, anim_key = 'attack_west', duration = 100 }

return M