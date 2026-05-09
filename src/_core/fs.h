#pragma once

#include <string>
#include <filesystem>
#include <functional>

namespace gin
{
    void foreach_in_dir(std::string_view dir, std::function<void(std::filesystem::path fspath)> fn);
}