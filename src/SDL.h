#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    SDL_Texture* texture1;
    SDL_Texture* texture2;
    int window_width;
    int window_height;
    int image_height;
    int text_area_height;
} AppContext;


void cleanUp(AppContext* ctx);
void renderImage(SDL_Renderer* renderer, SDL_Texture* texture, int image_height);
void initSDL(AppContext* ctx);
void loadTextures(AppContext* ctx);
void renderText(AppContext* ctx, const char* text); 