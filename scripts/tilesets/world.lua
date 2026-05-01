local M = {}

M.texture_key = 'assets/img/world.png'

M.n_cells = 16
M.cells_per_row = 4

M.tiles = {}

M.tiles[1] = { solid = false, frames = { 1 } }
M.tiles[2] = { solid = false, frames = { 2 } }
M.tiles[3] = { solid = true,  frames = { 3 } }
M.tiles[4] = { solid = false, frames = { 4 } }

M.tiles[5] = { solid = true,  frames = { 5, 6 }, duration = 500 }
M.tiles[6] = { solid = true,  frames = { 6 } }
M.tiles[7] = { solid = true,  frames = { 7 } }
M.tiles[8] = { solid = true,  frames = { 8 } }

M.tiles[9] = { solid = true, frames = { 9 }, anim_key = 'chest_closed', duration = 100, entity_key = 'chest' }
M.tiles[10] = { solid = true, frames = { 10 }, anim_key = 'chest_open', duration = 100 }
M.tiles[11] = { solid = true, frames = { 11 } }
M.tiles[12] = { solid = true, frames = { 12 } }

M.tiles[13] = { solid = false, frames = { 13 }, anim_key = 'door_closed', duration = 100, entity_key = 'door' }
M.tiles[14] = { solid = false, frames = { 14 }, anim_key = 'door_open', duration = 100 }
M.tiles[15] = { solid = true, frames = { 15 } }
M.tiles[16] = { solid = true, frames = { 16 } }

return M