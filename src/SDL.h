
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    SDL_Rect rect;  // 渲染區域的位置和大小
    SDL_Texture* textures[3];  // 背景, 文字, 圖片
    int currentTextureIndex;
    char text[256];  // 儲存顯示的文字
    SDL_Color textColor;
    SDL_Rect textRect;  // 文字的位置和大小
    SDL_Rect imageRect;  // 圖片的位置和大小
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
void renderText(AppContext* ctx, RenderArea* area);
void renderBackground(AppContext* ctx, RenderArea* area);
void renderImage(AppContext* ctx, RenderArea* area);
void createRenderArea(AppContext* ctx, int x, int y, int w, int h);
void renderAreas(AppContext* ctx);
void setRenderAreaContent(AppContext* ctx, int areaIndex, const char* background, const char* image, const char* text, SDL_Color textColor, SDL_Rect textRect, SDL_Rect imageRect);
