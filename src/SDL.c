

// main.c
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string.h>
#include "SDL.h"
//結構定義==================================================================================================
//SDL.h中定義
//函式宣告==================================================================================================
//SDL.h中定義
//函數定義==================================================================================================
// 清理資源
void cleanUp(AppContext* ctx) {
    for (int i = 0; i < ctx->num_render_areas; i++) {
        for (int j = 0; j < 3; j++) {
            if (ctx->render_areas[i].textures[j]) {
                SDL_DestroyTexture(ctx->render_areas[i].textures[j]);
            }
        }
    }
    TTF_CloseFont(ctx->font);
    SDL_DestroyRenderer(ctx->renderer);
    SDL_DestroyWindow(ctx->window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    if (ctx->render_areas) {
        free(ctx->render_areas);
    }
}

// 初始化SDL
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
    ctx->font = TTF_OpenFont("/home/goose/cp2-project/third_party/font/Noto_Sans_TC/NotoSansTC-VariableFont_wght.ttf", 24);
//字體資料夾
    if (ctx->font == NULL) {
        fprintf(stderr, "Failed to load font: %s\n", TTF_GetError());
        cleanUp(ctx);  // 確保我們正確地關閉SDL
        exit(1);
    }
}

// 載入文字和圖像紋理
void loadTextures(AppContext* ctx) {
    for (int i = 0; i < ctx->num_render_areas; i++) {
        if (ctx->render_areas[i].textures[1] == NULL && strlen(ctx->render_areas[i].text) != 0) {
            SDL_Surface* surface = TTF_RenderText_Solid(ctx->font, ctx->render_areas[i].text, ctx->render_areas[i].textColor);
            ctx->render_areas[i].textures[1] = SDL_CreateTextureFromSurface(ctx->renderer, surface);
            SDL_FreeSurface(surface);
            if (ctx->render_areas[i].textures[1] == NULL) {
                fprintf(stderr, "Failed to create text texture: %s\n", SDL_GetError());
            }
        }
    }
}

// 渲染背景
void renderBackground(AppContext* ctx, RenderArea* area) {
    if (area->textures[0]) {
        SDL_RenderCopy(ctx->renderer, area->textures[0], NULL, &area->rect);
    } else {
        SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(ctx->renderer, &area->rect);
    }
}

// 渲染文字
void renderText(AppContext* ctx, RenderArea* area) {
    if (strlen(area->text) > 0) {
        SDL_Surface* surface = TTF_RenderText_Solid(ctx->font, area->text, area->textColor);
        area->textures[1] = SDL_CreateTextureFromSurface(ctx->renderer, surface);
        int texW = 0, texH = 0;
        SDL_QueryTexture(area->textures[1], NULL, NULL, &texW, &texH);
        SDL_Rect dstrect = {area->rect.x + area->textRect.x, area->rect.y + area->textRect.y, texW, texH};
        SDL_RenderCopy(ctx->renderer, area->textures[1], NULL, &dstrect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(area->textures[1]);
    }
}

// 渲染圖片
void renderImage(AppContext* ctx, RenderArea* area) {
    if (area->textures[2]) {
        SDL_Rect dstrect = {area->rect.x + area->imageRect.x, area->rect.y + area->imageRect.y, area->imageRect.w, area->imageRect.h};
        SDL_RenderCopy(ctx->renderer, area->textures[2], NULL, &dstrect);
    }
}

// 創建渲染區域
void createRenderArea(AppContext* ctx, int x, int y, int w, int h) {  
    printf("Creating render area at (%d, %d) with dimensions %dx%d\n", x, y, w, h);
    ctx->num_render_areas++;
    ctx->render_areas = realloc(ctx->render_areas, ctx->num_render_areas * sizeof(RenderArea));
    ctx->render_areas[ctx->num_render_areas - 1].rect = (SDL_Rect){x, y, w, h};
    memset(ctx->render_areas[ctx->num_render_areas - 1].textures, 0, sizeof(ctx->render_areas[ctx->num_render_areas - 1].textures));
    ctx->render_areas[ctx->num_render_areas - 1].currentTextureIndex = 0;
    strcpy(ctx->render_areas[ctx->num_render_areas - 1].text, "");
    ctx->render_areas[ctx->num_render_areas - 1].textColor = (SDL_Color){255, 255, 255};
    ctx->render_areas[ctx->num_render_areas - 1].textRect = (SDL_Rect){0, 0, w, h};  // 默認文字大小
    ctx->render_areas[ctx->num_render_areas - 1].imageRect = (SDL_Rect){0, 0, w, h};  // 默認圖片大小
}

// 渲染所有區域
void renderAreas(AppContext* ctx) {
    for (int i = 0; i < ctx->num_render_areas; i++) {
        renderBackground(ctx, &ctx->render_areas[i]);
        renderImage(ctx, &ctx->render_areas[i]);
        renderText(ctx, &ctx->render_areas[i]);
    }
}

// 設置渲染區域內容
void setRenderAreaContent(AppContext* ctx, int areaIndex, const char* background, const char* image, const char* text, SDL_Color textColor, SDL_Rect textRect, SDL_Rect imageRect) {
    if (background) {
        ctx->render_areas[areaIndex].textures[0] = IMG_LoadTexture(ctx->renderer, background);
        if (ctx->render_areas[areaIndex].textures[0] == NULL) {
            fprintf(stderr, "Failed to load texture %s: %s\n", background, IMG_GetError());
        }
    }
    if (image) {
        ctx->render_areas[areaIndex].textures[2] = IMG_LoadTexture(ctx->renderer, image);
        if (ctx->render_areas[areaIndex].textures[2] == NULL) {
            fprintf(stderr, "Failed to load texture %s: %s\n", image, IMG_GetError());
        }
    }
    if (text) {
        strcpy(ctx->render_areas[areaIndex].text, text);
        ctx->render_areas[areaIndex].textColor = textColor;
        ctx->render_areas[areaIndex].textRect = textRect;
    }
    ctx->render_areas[areaIndex].imageRect = imageRect;
}
