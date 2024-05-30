#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string.h>
#include <libavutil/imgutils.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include "SDL.h"

int stopVideoFlag = 0;
int isVideoPlaying = 0;

int main(int argc, char* argv[]) {
    AppContext ctx = {NULL, NULL, NULL, 1500, 900, NULL, 0, 0};  // 初始化應用程式上下文
    initSDL(&ctx);  // 初始化SDL
    
    // 創建四個渲染區域  
    createRenderArea(&ctx, 0, 0, ctx.window_width / 10, ctx.window_height);  // 第一個窗口
    createRenderArea(&ctx, ctx.window_width / 10 * 9, 0, ctx.window_width / 10, ctx.window_height); // 第二個窗口
    createRenderArea(&ctx, ctx.window_width / 10, 0, ctx.window_width / 10 * 8, ctx.window_height / 10 * 7); // 第三個窗口
    createRenderArea(&ctx, ctx.window_width / 10, ctx.window_height / 10 * 7, ctx.window_width / 10 * 8, ctx.window_height / 10 * 3); // 第四個窗口

    SDL_Color white = {255, 255, 255};  // 設置顏色為白色
    SDL_Color black = {0, 0, 0};  // 設置顏色為黑色

    // 設置每個渲染區域的內容
    // Area 1
    char* images1[] = {"a24.bmp"};
    char* texts1[] = {"helloworld"};
    SDL_Rect textRect1 = {ctx.window_width / 20, ctx.window_height / 2 - 25, 100, 50};
    SDL_Rect imageRect1 = {0, 0, ctx.window_width / 10, ctx.window_width / 10};
    setRenderAreaContent(&ctx, 0, "aa-24.bmp", images1, 1, texts1, &white, &textRect1, 1, NULL, NULL);
    ctx.render_areas[0].imageRects = malloc(sizeof(SDL_Rect));
    ctx.render_areas[0].imageRects[0] = imageRect1;

    // Area 2
    char* images2[] = {"test.png"};
    char* texts2[10];
    SDL_Color textColors2[10];  // 每個文字的顏色都設置為白色
    SDL_Rect textRects2[10];
    for (int i = 0; i < 10; i++) {
        texts2[i] = malloc(10 * sizeof(char));
        sprintf(texts2[i], "item%d", i + 1);
        textColors2[i] = white;  // 設置顏色為白色
        textRects2[i].x = 10;
        textRects2[i].y = 50 * i; // 調整每個文本的 y 坐標，使其從上到下排列
        textRects2[i].w = ctx.window_width / 10 - 20; // 調整文本寬度
        textRects2[i].h = 40; // 調整文本高度
    }
    SDL_Rect imageRect2 = {0, ctx.window_height - ctx.window_width / 10, ctx.window_width / 10, ctx.window_width / 10}; // 正方形圖片的位置和大小
    setRenderAreaContent(&ctx, 1, "a24.bmp", images2, 1, texts2, textColors2, textRects2, 10, NULL, NULL);
    ctx.render_areas[1].imageRects = malloc(sizeof(SDL_Rect));
    ctx.render_areas[1].imageRects[0] = imageRect2;

    // Area 3
    char* images3[] = {"a24.bmp", "test.png"};
    char* texts3[] = {NULL};
    SDL_Rect imageRects3[2] = {
        {0, 0, ctx.window_width / 20, ctx.window_height / 10 * 7},  // 左邊圖片
        {ctx.window_width / 10 * 8 - ctx.window_width / 20, 0, ctx.window_width / 20, ctx.window_height / 10 * 7}  // 右邊圖片
    };
    setRenderAreaContent(&ctx, 2, NULL, images3, 2, texts3, NULL, imageRects3, 0, NULL, NULL);

    // Area 4
    char* texts4[] = {"Input your decision"};
    SDL_Rect textRect4 = {10, 10, ctx.window_width / 10 * 8 - 20, 50}; // 調整文本框大小
    char* images4[] = {"aa-24.bmp"};
    SDL_Rect imageRect4 = {ctx.window_width / 10 * 8 - 100, ctx.window_height / 10 * 3 - 100, 100, 100};  // 正方形圖片的位置和大小
    setRenderAreaContent(&ctx, 3, "test.png", images4, 1, texts4, &white, &textRect4, 1, NULL, NULL);
    ctx.render_areas[3].imageRects = malloc(sizeof(SDL_Rect));
    ctx.render_areas[3].imageRects[0] = imageRect4;

    loadTextures(&ctx);  // 載入文字和圖像紋理

    int close = 0;
    SDL_Event e;
    char inputText[100] = "";

    // 主循環
    while (!close) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                close = 1;
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_RETURN) {
                    printf("Your input: %s\n", inputText);
                    strcpy(inputText, "");
                    strcpy(ctx.render_areas[3].texts[0], "Input your decision");  // 重置文字
                } else if (e.key.keysym.sym == SDLK_BACKSPACE && strlen(inputText) > 0) {
                    inputText[strlen(inputText) - 1] = '\0';
                } else if ((e.key.keysym.sym >= SDLK_0 && e.key.keysym.sym <= SDLK_9) || e.key.keysym.sym == SDLK_KP_MINUS) {
                    int len = strlen(inputText);
                    inputText[len] = e.key.keysym.sym;
                    inputText[len + 1] = '\0';
                }
                strcpy(ctx.render_areas[3].texts[0], inputText);  // 更新第四個窗口的文字
            }
        }

        if (isVideoPlaying) {
            // 播放视频
            playVideoFrame(&ctx, &ctx.render_areas[2], "test.mp4");

            // 每秒检查一次输入缓冲
            if (SDL_PollEvent(&e) && e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
                isVideoPlaying = 0;
                setRenderAreaContent(&ctx, 2, NULL, images3, 2, texts3, NULL, imageRects3, 0, NULL, NULL);
                loadTextures(&ctx);
            }
        } else {
            // 切換第三窗口的圖片
            if (ctx.switchCounter++ % 2 == 0) {
                ctx.render_areas[2].images[0] = NULL;
                ctx.render_areas[2].images[1] = IMG_LoadTexture(ctx.renderer, "test.png");
            } else {
                ctx.render_areas[2].images[0] = IMG_LoadTexture(ctx.renderer, "a24.bmp");
                ctx.render_areas[2].images[1] = IMG_LoadTexture(ctx.renderer, "test.png");
            }

            // 检查是否按下空白键以切换到视频播放
            if (SDL_PollEvent(&e) && e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
                isVideoPlaying = 1;
            }
        }

        SDL_RenderClear(ctx.renderer);
        renderAreas(&ctx);  // 渲染所有區域
        SDL_RenderPresent(ctx.renderer);
        SDL_Delay(33);  // 每秒切換第三窗口的圖片
    }

    cleanUp(&ctx);  // 清理資源

    for (int i =0;  i < 10; i++) {
        free(texts2[i]);
    }

    return 0;
}
