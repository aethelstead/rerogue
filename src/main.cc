#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <cstdlib>
#include <memory>
#include <chrono>
#include <print>
#include "gin.h"

int main(int argc, char** argv)
{
    if ((SDL_Init(SDL_INIT_EVERYTHING) != 0) ||
        (IMG_Init(IMG_INIT_PNG | IMG_INIT_WEBP) == 0 || TTF_Init() == -1) ||
        (Mix_Init(MIX_INIT_MP3) == 0 || Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 2048) == -1) ||
        (TTF_Init() != 0))
    {
        std::println("Failed to init platform. Application will now exit.");
        return EXIT_FAILURE;
    }

    auto state = std::make_unique<gin::State>();
    state->init();

    using clock = std::chrono::high_resolution_clock;
    auto start = clock::now();
    double dt = 0.0;
    double avg_fps = 0;

    while (true)
    {
        auto now = clock::now();
        std::chrono::duration<double> elapsed = now - start;
        start = now;

        dt = elapsed.count();
        avg_fps = (1.0 / dt);

        auto game_start = clock::now();

        if (state->events() == false)
            break;
        if (state->update(dt) == false)
            break;

        state->draw();

        auto game_now = clock::now();
        std::chrono::duration<double> game_elapsed = game_now - game_start;

        SDL_SetRenderDrawColor(state->gfx.renderer.get(), 0xff, 0xff, 0, 0xff);
        state->gfx.draw_text(std::format("FPS: {:.0f}", avg_fps), "consola_small", 10, 10);
        state->gfx.draw_text(std::format("Took: {:.2f}", game_elapsed.count() * 1000), "consola_small", 10, 30);

        SDL_RenderPresent(state->gfx.renderer.get());
    }

    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;
}
