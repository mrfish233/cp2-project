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

void cleanUp(AppContext* ctx) {
    SDL_DestroyTexture(ctx->texture1);
    SDL_DestroyTexture(ctx->texture2);
    TTF_CloseFont(ctx->font);
    SDL_DestroyRenderer(ctx->renderer);
    SDL_DestroyWindow(ctx->window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

void initSDL(AppContext* ctx) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        fprintf(stderr, "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        SDL_Quit();
        exit(1);
    }

    if (TTF_Init() == -1) {
        fprintf(stderr, "TTF_Init: %s\n", TTF_GetError());
        SDL_Quit();
        exit(1);
    }

    ctx->window = SDL_CreateWindow("SDL App", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ctx->window_width, ctx->window_height, SDL_WINDOW_SHOWN);
    ctx->renderer = SDL_CreateRenderer(ctx->window, -1, SDL_RENDERER_ACCELERATED);
    ctx->font = TTF_OpenFont("/home/goose/cp2-project/third-party/font/Noto_Sans_TC/NotoSansTC-VariableFont_wght.ttf", 24);

    if (ctx->font == NULL) {
        fprintf(stderr, "Failed to load font: %s\n", TTF_GetError());
        cleanUp(ctx);  // Ensure we properly close down SDL
        exit(1);
    }
}

void loadTextures(AppContext* ctx) {
    ctx->texture1 = IMG_LoadTexture(ctx->renderer, "a24.bmp");

    if (ctx->texture1 == NULL) {
        fprintf(stderr, "Failed to load texture a24.bmp: %s\n", IMG_GetError());
        cleanUp(ctx);
        exit(1);
    }

    ctx->texture2 = IMG_LoadTexture(ctx->renderer, "aa-24.bmp");

    if (ctx->texture2 == NULL) {
        fprintf(stderr, "Failed to load texture aa-24.bmp: %s\n", IMG_GetError());
        cleanUp(ctx);
        exit(1);
    }
}

void renderText(AppContext* ctx, const char* text) {
    SDL_Color color = {255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(ctx->font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(ctx->renderer, surface);
    SDL_Rect rect = {0, ctx->image_height, surface->w, surface->h};
    SDL_RenderCopy(ctx->renderer, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void renderImage(SDL_Renderer* renderer, SDL_Texture* texture, int image_height) {
    SDL_Rect src = {0, 0, 800, image_height};
    SDL_RenderCopy(renderer, texture, NULL, &src);
}
