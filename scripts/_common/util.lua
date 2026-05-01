local M = {}

M.copy_table = function(t)
    local copy = {}
    for k, v in pairs(t) do copy[k] = v end
    return copy
end

return M