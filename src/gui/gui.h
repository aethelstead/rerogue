#pragma once

#include <vector>
#include <optional>
#include <string>
#include "../_core/geometry.h"

namespace gin
{
    struct GuiState
    {
        Recti view;
        bool show_pause = false;
        bool show_overmap = false;
        std::vector<std::string> all_messages;
        std::optional<std::string> opt_speech;
    };
}