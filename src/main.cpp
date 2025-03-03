#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_init.h>

#include "bad_engine.h"

#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */

BAD_Engine bad;
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_SetAppMetadata("bad-day", "1.0", "popov");
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("bad day", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer))
    {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!TTF_Init())
    {
        SDL_Log("Couldn't initialise SDL_ttf: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    font = TTF_OpenFontIO(SDL_IOFromConstMem(tiny_ttf, tiny_ttf_len), true, 18.0f);
    if (!font)
    {
        SDL_Log("Couldn't open font: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    bad.init(renderer, window, font);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    switch (event->type)
    {
        case SDL_EVENT_MOUSE_MOTION:
        {
            SDL_FPoint mousePosition;
            SDL_GetMouseState(&mousePosition.x, &mousePosition.y);
            bad.mouseInput(0, mousePosition);
            break;
        }
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        {
            SDL_FPoint mousePosition;
            SDL_GetMouseState(&mousePosition.x, &mousePosition.y);
            bad.mouseInput(event->button.button, mousePosition);
            break;
        }
        case SDL_EVENT_QUIT:
        {
            return SDL_APP_SUCCESS;
            break;
        }
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    bad.iterate();
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    bad.quit();
    /* runs once on app quit */
}
