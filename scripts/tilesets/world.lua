local M = {}

M.texture_key = 'assets/img/world.png'

M.n_cells = 64
M.cells_per_row = 8

M.tiles = {}

M.tiles[1] = { is_wall = false, frames = { 1 } }
M.tiles[2] = { is_wall = false, frames = { 2 } }
M.tiles[3] = { is_wall = true,  frames = { 3 } }
M.tiles[4] = { is_wall = true, frames = { 4 } }
M.tiles[5] = { is_wall = false, frames = { 5 } }
M.tiles[6] = { is_wall = false, frames = { 6 } }
M.tiles[7] = { is_wall = false, frames = { 7 } }
M.tiles[8] = { is_wall = false, frames = { 8 } }

M.tiles[9] = { is_wall = true,  frames = { 9, 10 }, duration = 500 }
M.tiles[10] = { is_wall = true,  frames = { 10 } }
M.tiles[11] = { is_wall = true,  frames = { 11 } }
M.tiles[12] = { is_wall = true,  frames = { 12 } }
M.tiles[13] = { is_wall = false, frames = { 13 } }
M.tiles[14] = { is_wall = false, frames = { 14 } }
M.tiles[15] = { is_wall = false, frames = { 15 } }
M.tiles[16] = { is_wall = false, frames = { 16 } }

M.tiles[17] = { is_wall = true, frames = { 17 }, anim_key = 'chest_closed', duration = 100, entity_key = 'chest' }
M.tiles[18] = { is_wall = true, frames = { 18 }, anim_key = 'chest_open', duration = 100 }
M.tiles[19] = { is_wall = true, frames = { 19 } }
M.tiles[20] = { is_wall = true, frames = { 20 } }
M.tiles[21] = { is_wall = false, frames = { 21 } }
M.tiles[22] = { is_wall = false, frames = { 22 } }
M.tiles[23] = { is_wall = true,  frames = { 23 } }
M.tiles[24] = { is_wall = false, frames = { 24 } }

M.tiles[25] = { is_wall = false, frames = { 25 }, anim_key = 'door_closed', duration = 100, entity_key = 'door' }
M.tiles[26] = { is_wall = false, frames = { 26 }, anim_key = 'door_open', duration = 100 }
M.tiles[27] = { is_wall = true, frames = { 27 } }
M.tiles[28] = { is_wall = true, frames = { 28 } }
M.tiles[29] = { is_wall = true, frames = { 29 } }
M.tiles[30] = { is_wall = true, frames = { 30 } }
M.tiles[31] = { is_wall = true, frames = { 31 } }
M.tiles[32] = { is_wall = true, frames = { 32 } }

return M