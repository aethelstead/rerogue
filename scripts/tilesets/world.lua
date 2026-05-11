local M = {}

M.texture_key = 'assets/img/world.png'

M.n_cells = 16
M.cells_per_row = 4

M.tiles = {}

M.tiles[1] = { is_wall = false, frames = { 1 } }
M.tiles[2] = { is_wall = false, frames = { 2 } }
M.tiles[3] = { is_wall = true,  frames = { 3 } }
M.tiles[4] = { is_wall = false, frames = { 4 } }

M.tiles[5] = { is_wall = true,  frames = { 5, 6 }, duration = 500 }
M.tiles[6] = { is_wall = true,  frames = { 6 } }
M.tiles[7] = { is_wall = true,  frames = { 7 } }
M.tiles[8] = { is_wall = true,  frames = { 8 } }

M.tiles[9] = { is_wall = true, frames = { 9 }, anim_key = 'chest_closed', duration = 100, entity_key = 'chest' }
M.tiles[10] = { is_wall = true, frames = { 10 }, anim_key = 'chest_open', duration = 100 }
M.tiles[11] = { is_wall = true, frames = { 11 } }
M.tiles[12] = { is_wall = true, frames = { 12 } }

M.tiles[13] = { is_wall = false, frames = { 13 }, anim_key = 'door_closed', duration = 100, entity_key = 'door' }
M.tiles[14] = { is_wall = false, frames = { 14 }, anim_key = 'door_open', duration = 100 }
M.tiles[15] = { is_wall = true, frames = { 15 } }
M.tiles[16] = { is_wall = true, frames = { 16 } }

return M