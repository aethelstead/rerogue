#include <print>
#include "lua.h"

sol::state& get_L()
{   
    static sol::state init_once = []() 
        {
            sol::state L;
            L.open_libraries(
                sol::lib::base, 
                sol::lib::package, 
                sol::lib::coroutine, 
                sol::lib::string, 
                sol::lib::math, 
                sol::lib::table
            );

            std::println("lua was init.");

            return L;
        }();

    return init_once;
}