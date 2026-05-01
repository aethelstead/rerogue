
#include <SDL2/SDL_ttf.h>
#include <cstdlib>
#include <memory>
#include <chrono>
#include <print>
#include "gin.h"

int main(int argc, char** argv)
{
    using clock = std::chrono::high_resolution_clock;

    if ((SDL_Init(SDL_INIT_EVERYTHING) != 0) ||
        (IMG_Init(IMG_INIT_PNG | IMG_INIT_WEBP) == 0 || TTF_Init() == -1) ||
        (Mix_Init(MIX_INIT_MP3) == 0 || Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 2048) == -1) ||
        (TTF_Init() != 0))
    {
        std::println("Failed to init platform. Application will now exit.");
        return EXIT_FAILURE;
    }

    auto state = std::make_unique<gin::State>();
    gin::init(*state);

    double game_dt = 0.0;
    auto start_time = clock::now();
    while (true)
    {
        auto curr_time = clock::now();
        std::chrono::duration<float> elapsed = curr_time - start_time;
        start_time = curr_time;
        state->perf_counter.dt = elapsed.count();
        state->perf_counter.frame_count++;
        state->perf_counter.avg_fps = (int)(1.0 / state->perf_counter.dt);
        state->perf_counter.frame_ms = state->perf_counter.dt * 1000;

        if (gin::events(*state) == false)
            break;

        auto game_start = clock::now();
        if (gin::update(*state) == false)
            break;

        gin::draw(*state);
        auto game_end = clock::now();
        std::chrono::duration<float> game_elapsed = game_end - game_start;
        state->perf_counter.game_ms = game_elapsed.count() * 1000;

        SDL_RenderPresent(state->renderer.get());
    }

    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;
}
