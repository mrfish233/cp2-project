
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string.h>
// SDL.h

#ifndef SDL_H
#define SDL_H

typedef struct {
    SDL_Rect rect;  // 渲染區域的位置和大小
    SDL_Texture* background;  // 背景紋理
    SDL_Texture** images;  // 圖片紋理陣列
    int image_count;  // 圖片數量
    char** texts;  // 文字陣列
    SDL_Texture** textTextures;  // 文字紋理陣列
    SDL_Color* textColors;  // 文字顏色陣列
    SDL_Rect* textRects;  // 文字位置和大小陣列
    int text_count;  // 文字數量
} RenderArea;

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    int window_width;
    int window_height;
    RenderArea* render_areas;
    int num_render_areas;
    int switchCounter;  // 用於第三區域圖片切換的計數器
} AppContext;

void cleanUp(AppContext* ctx);
void initSDL(AppContext* ctx);
void loadTextures(AppContext* ctx);

void renderImage(AppContext* ctx, RenderArea* area);
void renderText(AppContext* ctx, RenderArea* area);
void setRenderAreaContent(AppContext* ctx, int areaIndex, const char* background, char** images, int image_count, char** texts, SDL_Color* textColors, SDL_Rect* textRects, int text_count);
void createRenderArea(AppContext* ctx, int x, int y, int w, int h);
void renderAreas(AppContext* ctx);

#endif // SDL_H
