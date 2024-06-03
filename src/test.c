// test.c
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string.h>
#include "SDL.h"

// 按鈕點擊事件的回調函數
void onClickShowSad(AppContext* ctx) {
    printf("Button 'Sad' clicked\n");
    char* images[] = {"character/airudy/tachie/sad.png"};
    SDL_Rect imageRect = {0, 0, ctx->window_width, ctx->window_height};
    setRenderAreaContent(ctx, 0, NULL, images, 1, NULL, NULL, NULL, 0, NULL, &imageRect);
    loadTextures(ctx);
    printf("Set content for 'Sad' image.\n");
}

void onClickShowHappy(AppContext* ctx) {
    printf("Button 'Happy' clicked\n");
    char* images[] = {"character/airudy/tachie/happy.png"};
    SDL_Rect imageRect = {0, 0, ctx->window_width, ctx->window_height};
    setRenderAreaContent(ctx, 0, NULL, images, 1, NULL, NULL, NULL, 0, NULL, &imageRect);
    loadTextures(ctx);
    printf("Set content for 'Happy' image.\n");
}

void onClickPlayVideo(AppContext* ctx) {
    printf("Button 'Play Video' clicked\n");
    char* images[] = {NULL};
    SDL_Rect videoRect = {0, 0, ctx->window_width, ctx->window_height};
    setRenderAreaContent(ctx, 0, NULL, images, 0, NULL, NULL, NULL, 0, "background/test.mp4", &videoRect);
    loadTextures(ctx);
    isVideoPlaying = 1;
}

void onClickSetBackgroundChurch(AppContext* ctx) {
    printf("Button 'Church' clicked\n");
    setRenderAreaContent(ctx, 0, "background/church.png", NULL, 0, NULL, NULL, NULL, 0, NULL, NULL);
    loadTextures(ctx);
    printf("Set background to 'Church'.\n");
}

void onClickSetBackgroundBedroom(AppContext* ctx) {
    printf("Button 'Bedroom' clicked\n");
    setRenderAreaContent(ctx, 0, "background/bedroom.png", NULL, 0, NULL, NULL, NULL, 0, NULL, NULL);
    loadTextures(ctx);
    printf("Set background to 'Bedroom'.\n");
}

int main(int argc, char* argv[]) {
    AppContext ctx = {NULL, NULL, NULL, 800, 600, NULL, 0, 0};  // 初始化應用程式上下文
    initSDL(&ctx);  // 初始化SDL

    // 創建渲染區域  
    createRenderArea(&ctx, 0, 0, ctx.window_width, ctx.window_height);  // 單一渲染區域

    // 創建按鈕
    Button buttons[5];
    createButton(&ctx, &buttons[0], ctx.window_width / 2 - 250, ctx.window_height - 100, 100, 50, "Sad", onClickShowSad);
    createButton(&ctx, &buttons[1], ctx.window_width / 2 - 150, ctx.window_height - 100, 100, 50, "Happy", onClickShowHappy);
    createButton(&ctx, &buttons[2], ctx.window_width / 2 - 50, ctx.window_height - 100, 100, 50, "Play Video", onClickPlayVideo);
    createButton(&ctx, &buttons[3], ctx.window_width / 2 + 50, ctx.window_height - 100, 100, 50, "Church", onClickSetBackgroundChurch);
    createButton(&ctx, &buttons[4], ctx.window_width / 2 + 150, ctx.window_height - 100, 100, 50, "Bedroom", onClickSetBackgroundBedroom);

    int close = 0;
    SDL_Event e;

    // 主循環
    while (!close) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                close = 1;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                for (int i = 0; i < 5; i++) {
                    if (isButtonClicked(&buttons[i], x, y)) {
                        buttons[i].onClick(&ctx);
                    }
                }
            } else if (e.type == SDL_KEYDOWN && isVideoPlaying) {
                isVideoPlaying = 0;
                printf("Stopping video playback due to key press.\n");
                setRenderAreaContent(&ctx, 0, NULL, NULL, 0, NULL, NULL, NULL, 0, NULL, NULL);
                loadTextures(&ctx);
            }
        }

        SDL_RenderClear(ctx.renderer);
        if (isVideoPlaying) {
            playVideoFrame(&ctx, &ctx.render_areas[0], "background/test.mp4");
        } else {
            renderAreas(&ctx);  // 渲染所有區域
        }
        for (int i = 0; i < 5; i++) {
            renderButton(&ctx, &buttons[i]);
        }
        SDL_RenderPresent(ctx.renderer);
        SDL_Delay(33);  // 每秒切換第三窗口的圖片
    }

    cleanUp(&ctx);  // 清理資源

    return 0;
}
