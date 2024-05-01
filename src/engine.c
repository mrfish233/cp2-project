#include "SDL.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) {
    AppContext ctx = {NULL, NULL, NULL, NULL, NULL, 800, 600, 500, 100};
    initSDL(&ctx);
    loadTextures(&ctx);

    int close = 0, currentTexture = 1;
    char inputText[100] = "Enter Age: ";
    SDL_Event e;

    while (!close) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                close = 1;
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_RETURN) {
                    printf("Age entered: %s\n", inputText + 11);
                    break;
                } else if (e.key.keysym.sym == SDLK_BACKSPACE && strlen(inputText) > 11) {
                    inputText[strlen(inputText) - 1] = '\0';
                } else if ((e.key.keysym.sym >= SDLK_0 && e.key.keysym.sym <= SDLK_9) || e.key.keysym.sym == SDLK_KP_MINUS) {
                    int len = strlen(inputText);
                    inputText[len] = e.key.keysym.sym;
                    inputText[len + 1] = '\0';
                }
            }
        }

        SDL_RenderClear(ctx.renderer);
        if (currentTexture == 1) {
            renderImage(ctx.renderer, ctx.texture1, ctx.image_height);
        } else {
            renderImage(ctx.renderer, ctx.texture2, ctx.image_height);
        }
        currentTexture = 1 - currentTexture;  // Toggle between 1 and 0
        renderText(&ctx, inputText);

        SDL_RenderPresent(ctx.renderer);
        SDL_Delay(33);  // 30幀
    }

    cleanUp(&ctx);
    return 0;
}
