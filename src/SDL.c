

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



//函數定義==================================================================================================




// 初始化SDL
// 參數: AppContext* ctx  
// 輸出: 無
// 功能: 初始化SDL和相關的庫，創建窗口和渲染器，並加載字體
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

    // 設置混合模式
    SDL_SetRenderDrawBlendMode(ctx->renderer, SDL_BLENDMODE_BLEND);

    ctx->font = TTF_OpenFont("/home/goose/cp2-project/third_party/font/Noto_Sans_TC/static/NotoSansTC-Black.ttf", 24);
    if (ctx->font == NULL) {
        fprintf(stderr, "Failed to load font: %s\n", TTF_GetError());
        cleanUp(ctx);  // 確保我們正確地關閉SDL
        exit(1);
    }
}


// 載入文字和圖像紋理
// 參數: AppContext* ctx  
// 輸出: 無
// 功能: 將文字渲染為紋理，並將其加載到每個渲染區域
void loadTextures(AppContext* ctx) {
    for (int i = 0; i < ctx->num_render_areas; i++) {
        if (ctx->render_areas[i].text_count > 0) {
            ctx->render_areas[i].textTextures = malloc(ctx->render_areas[i].text_count * sizeof(SDL_Texture*));
            for (int j = 0; j < ctx->render_areas[i].text_count; j++) {
                SDL_Surface* surface = TTF_RenderText_Solid(ctx->font, ctx->render_areas[i].texts[j], ctx->render_areas[i].textColors[j]);
                ctx->render_areas[i].textTextures[j] = SDL_CreateTextureFromSurface(ctx->renderer, surface);
                SDL_FreeSurface(surface);
                if (ctx->render_areas[i].textTextures[j] == NULL) {
                    fprintf(stderr, "Failed to create text texture: %s\n", SDL_GetError());
                }
            }
        }
    }
}

// 渲染背景
// 參數: AppContext* ctx  , RenderArea* area - 渲染區域
// 輸出: 無
// 功能: 渲染背景圖像或填充顏色
void renderBackground(AppContext* ctx, RenderArea* area) {
    if (area->background) {
        SDL_RenderCopy(ctx->renderer, area->background, NULL, &area->rect);
    } else {
        SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(ctx->renderer, &area->rect);
    }
}

// 創建渲染區域
// 參數: AppContext* ctx  , int x - 渲染區域的X座標, int y - 渲染區域的Y座標, int w - 渲染區域的寬度, int h - 渲染區域的高度
// 輸出: 無
// 功能: 創建並初始化一個新的渲染區域
void createRenderArea(AppContext* ctx, int x, int y, int w, int h) {  
    printf("Creating render area at (%d, %d) with dimensions %dx%d\n", x, y, w, h);
    ctx->num_render_areas++;
    ctx->render_areas = realloc(ctx->render_areas, ctx->num_render_areas * sizeof(RenderArea));
    ctx->render_areas[ctx->num_render_areas - 1].rect = (SDL_Rect){x, y, w, h};
    ctx->render_areas[ctx->num_render_areas - 1].background = NULL;
    ctx->render_areas[ctx->num_render_areas - 1].images = NULL;
    ctx->render_areas[ctx->num_render_areas - 1].image_count = 0;
    ctx->render_areas[ctx->num_render_areas - 1].texts = NULL;
    ctx->render_areas[ctx->num_render_areas - 1].textColors = NULL;
    ctx->render_areas[ctx->num_render_areas - 1].textRects = NULL;
    ctx->render_areas[ctx->num_render_areas - 1].textTextures = NULL;
    ctx->render_areas[ctx->num_render_areas - 1].text_count = 0;
}

// 渲染所有區域
// 參數: AppContext* ctx  
// 輸出: 無
// 功能: 渲染應用程式中的所有渲染區域
void renderAreas(AppContext* ctx) {
    for (int i = 0; i < ctx->num_render_areas; i++) {
        renderBackground(ctx, &ctx->render_areas[i]);
        renderImage(ctx, &ctx->render_areas[i]);
        renderText(ctx, &ctx->render_areas[i]);
    }
}

// 渲染圖片
// 參數: AppContext* ctx  , RenderArea* area - 渲染區域
// 輸出: 無
// 功能: 渲染區域中的所有圖片
// 渲染圖片
void renderImage(AppContext* ctx, RenderArea* area) {
    for (int i = 0; i < area->image_count; i++) {
        SDL_Texture* texture = area->images[i];
        if (texture == NULL) {
            fprintf(stderr, "Failed to load image texture: %s\n", SDL_GetError());
            continue;
        }
        SDL_Rect dstrect = area->imageRects[i];
        SDL_RenderCopy(ctx->renderer, texture, NULL, &dstrect);
    }
}


// 渲染文字
// 參數: AppContext* ctx - 應用程式上下文, RenderArea* area - 渲染區域
// 輸出: 無
// 功能: 渲染區域中的所有文字
// 渲染文字
void renderText(AppContext* ctx, RenderArea* area) {
    for (int i = 0; i < area->text_count; i++) {
        printf("Rendering text %d\n", i);
        SDL_Texture* texture = area->textTextures[i];

        if (texture == NULL) {
            fprintf(stderr, "Failed to create text texture: %s\n", SDL_GetError());
            continue;
        }

        SDL_Rect dstrect = {
            .x = area->rect.x + area->textRects[i].x,
            .y = area->rect.y + area->textRects[i].y,
            .w = area->textRects[i].w,
            .h = area->textRects[i].h
        };

        // 設置文字顏色
        SDL_SetTextureColorMod(texture, area->textColors[i].r, area->textColors[i].g, area->textColors[i].b);

        SDL_RenderCopy(ctx->renderer, texture, NULL, &dstrect);
    }
}


// 設置渲染區域內容
// 參數: AppContext* ctx - 應用程式上下文, int areaIndex - 渲染區域索引, const char* background - 背景圖像, char** images - 圖片數組, int image_count - 圖片數量, char** texts - 文字數組, SDL_Color* textColors - 文字顏色數組, SDL_Rect* textRects - 文字位置和大小數組, int text_count - 文字數量
// 輸出: 無
// 功能: 設置渲染區域的背景、圖片和文字內容
void setRenderAreaContent(AppContext* ctx, int areaIndex, const char* background, char** images, int image_count, char** texts, SDL_Color* textColors, SDL_Rect* textRects, int text_count) {
    RenderArea* area = &ctx->render_areas[areaIndex];
    
    // 設置背景
    if (background) {
        area->background = IMG_LoadTexture(ctx->renderer, background);
        if (area->background == NULL) {
            fprintf(stderr, "Failed to load texture %s: %s\n", background, IMG_GetError());
        }
    }
    
    // 設置圖片
    area->image_count = image_count;
    area->images = malloc(image_count * sizeof(SDL_Texture*));
    area->imageRects = malloc(image_count * sizeof(SDL_Rect));
    for (int i = 0; i < image_count; i++) {
        area->images[i] = IMG_LoadTexture(ctx->renderer, images[i]);
        if (area->images[i] == NULL) {
            fprintf(stderr, "Failed to load texture %s: %s\n", images[i], IMG_GetError());
        }
        area->imageRects[i] = textRects[i];  // 設置圖像位置和大小
    }
    
    // 設置文字
    area->text_count = text_count;
    area->texts = malloc(text_count * sizeof(char*));
    area->textColors = malloc(text_count * sizeof(SDL_Color));
    area->textRects = malloc(text_count * sizeof(SDL_Rect));
    area->textTextures = malloc(text_count * sizeof(SDL_Texture*));  // 增加對應的textTextures初始化
    for (int i = 0; i < text_count; i++) {
        area->texts[i] = strdup(texts[i]);
        area->textColors[i] = textColors[i];
        area->textRects[i] = textRects[i];
    }
}

// 清理資源
// 參數: AppContext* ctx  
// 輸出: 無
// 功能: 釋放應用程式中所有分配的資源
void cleanUp(AppContext* ctx) {
    for (int i = 0; i < ctx->num_render_areas; i++) {
        for (int j = 0; j < ctx->render_areas[i].image_count; j++) {
            if (ctx->render_areas[i].images[j]) {
                SDL_DestroyTexture(ctx->render_areas[i].images[j]);
            }
        }
        for (int j = 0; j < ctx->render_areas[i].text_count; j++) {
            if (ctx->render_areas[i].texts[j]) {
                free(ctx->render_areas[i].texts[j]);
            }
            if (ctx->render_areas[i].textTextures[j]) {
                SDL_DestroyTexture(ctx->render_areas[i].textTextures[j]);
            }
        }
        free(ctx->render_areas[i].images);
        free(ctx->render_areas[i].texts);
        free(ctx->render_areas[i].textTextures);
        free(ctx->render_areas[i].textColors);
        free(ctx->render_areas[i].textRects);
        if (ctx->render_areas[i].background) {
            SDL_DestroyTexture(ctx->render_areas[i].background);
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

