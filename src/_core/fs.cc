#include "fs.h"

namespace gin
{
    void foreach_in_dir(std::string_view dir, std::function<void(std::filesystem::path fspath)> fn)
    {
        namespace fs = std::filesystem;
        for (const auto& entry : fs::directory_iterator(dir))
            if (fs::is_regular_file(entry))
                fn(entry.path());
    }
}
