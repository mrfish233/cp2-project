#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <libavutil/imgutils.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>

#include "script.h"
#include "parse_script.h"
#include "play_script.h"
#include "save_and_load.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <script-directory>\n", argv[0]);
        return 1;
    }

    Script  script  = {0};
    Display display = {0};

    if (initGame(&script, &display, argv[1]) != 0) {
        return 1;
    }

    while (1) {
        if (updateDialogue(&script, &display) != 0) {
            break;
        }

        if (strlen(display.character) > 0) {
            printf("%s: ", display.character);
        }

        printf("%s\n", display.dialogue);

        if (display.update_flag) {
            for (int32_t i = 0; i < display.update_size; i++) {
                printf("%s\n", display.updates[i]);
            }
        }

        if (display.option_flag) {
            for (int32_t i = 0; i < display.option_size; i++) {
                printf("    %d. %s\n", i + 1, display.options[i]);
            }

            int32_t option = 0;
            printf("Select an option: ");
            scanf("%d", &option);

            while (option < 1 || option > display.option_size) {
                printf("Invalid option, please select again: ");
                scanf("%d", &option);
            }

            display.option_select = option;
        }

        if (display.end_flag) {
            break;
        }

        usleep(500000);
    }

    /**
     * TODO:
     * 1. Implement selecting options
     * 2. Implement checking triggers
     */

    if (saveScript(&script, AUTO) != 0) {
        return 1;
    }

    // #ifdef DEBUG
    // printScript(&script);
    // #endif

    clearScript(&script);

    return 0;
}

// #include <stdio.h>
// #include <string.h>

// #include "SDL.h"
// #include <SDL2/SDL.h>
// #include <SDL2/SDL_image.h>
// #include <SDL2/SDL_ttf.h>
// #include <SDL2/SDL_mixer.h>

// #include <libswresample/swresample.h>

// // 按鈕點擊事件的回調函數
// void onClickShowAngry(AppContext* ctx) {
//     printf("Button 'Angry' clicked\n");
//     char* images[] = {"character/airudy/tachie/angry.png"};
//     SDL_Rect imageRect = {0, 0, ctx->window_width, ctx->window_height};
//     setRenderAreaContent(ctx, 0, NULL, images, 1, NULL, NULL, NULL, 0, NULL, &imageRect);
//     loadTextures(ctx);
//     printf("Set content for 'Angry' image.\n");
// }

// void onClickShowNormal(AppContext* ctx) {
//     printf("Button 'Normal' clicked\n");
//     char* images[] = {"character/airudy/tachie/normal.png"};
//     SDL_Rect imageRect = {0, 0, ctx->window_width, ctx->window_height};
//     setRenderAreaContent(ctx, 0, NULL, images, 1, NULL, NULL, NULL, 0, NULL, &imageRect);
//     loadTextures(ctx);
//     printf("Set content for 'Normal' image.\n");
// }

// void onClickPlayBeachVideo(AppContext* ctx) {
//     printf("Button 'Play Beach Video' clicked\n");
//     char* images[] = {NULL};
//     SDL_Rect videoRect = {0, 0, ctx->window_width, ctx->window_height};
//     setRenderAreaContent(ctx, 0, NULL, images, 0, NULL, NULL, NULL, 0, "background/test.mp4", &videoRect);
//     loadTextures(ctx);
//     isVideoPlaying = 1;
// }

// void onClickSetBackgroundBeach(AppContext* ctx) {
//     printf("Button 'Beach' clicked\n");
//     setRenderAreaContent(ctx, 0, "background/beach.png", NULL, 0, NULL, NULL, NULL, 0, NULL, NULL);
//     loadTextures(ctx);
//     printf("Set background to 'Beach'.\n");
// }

// void onClickSetBackgroundChurch(AppContext* ctx) {
//     printf("Button 'Church' clicked\n");
//     setRenderAreaContent(ctx, 0, "background/church.png", NULL, 0, NULL, NULL, NULL, 0, NULL, NULL);
//     loadTextures(ctx);
//     printf("Set background to 'Church'.\n");
// }

// int main(int argc, char* argv[]) {
//     AppContext ctx = {NULL, NULL, NULL, 800, 600, NULL, 0, 0};  // 初始化應用程式上下文
//     initSDL(&ctx);  // 初始化SDL
//     initAudio();    // 初始化音效

//     // 載入音效
//     Mix_Chunk* effect = NULL;
//     loadSound("/home/goose/cp2-project/example-game/assests/sounds/effect.mp3", &effect);

//     // 創建渲染區域  
//     createRenderArea(&ctx, 0, 0, ctx.window_width, ctx.window_height);  // 單一渲染區域

//     // 創建按鈕
//     Button buttons[5];
//     createButton(&ctx, &buttons[0], ctx.window_width / 2 - 250, ctx.window_height - 100, 100, 50, "Angry", onClickShowAngry);
//     createButton(&ctx, &buttons[1], ctx.window_width / 2 - 150, ctx.window_height - 100, 100, 50, "Normal", onClickShowNormal);
//     createButton(&ctx, &buttons[2], ctx.window_width / 2 - 50, ctx.window_height - 100, 150, 50, "Play Beach Video", onClickPlayBeachVideo);
//     createButton(&ctx, &buttons[3], ctx.window_width / 2 + 100, ctx.window_height - 100, 100, 50, "Beach", onClickSetBackgroundBeach);
//     createButton(&ctx, &buttons[4], ctx.window_width / 2 + 200, ctx.window_height - 100, 100, 50, "Church", onClickSetBackgroundChurch);

//     int close = 0;
//     SDL_Event e;

//     // 主循環
//     while (!close) {
//         while (SDL_PollEvent(&e) != 0) {
//             if (e.type == SDL_QUIT) {
//                 close = 1;
//             } else if (e.type == SDL_MOUSEBUTTONDOWN) {
//                 int x, y;
//                 SDL_GetMouseState(&x, &y);
//                 for (int i = 0; i < 5; i++) {
//                     if (isButtonClicked(&buttons[i], x, y)) {
//                         buttons[i].onClick(&ctx);
//                     }
//                 }
//                 // 播放音效
//                 playSound(effect);
//             } else if (e.type == SDL_KEYDOWN && isVideoPlaying) {
//                 isVideoPlaying = 0;
//                 printf("Stopping video playback due to key press.\n");
//                 setRenderAreaContent(&ctx, 0, NULL, NULL, 0, NULL, NULL, NULL, 0, NULL, NULL);
//                 loadTextures(&ctx);
//                 // 播放音效
//                 playSound(effect);
//             } else if (e.type == SDL_KEYDOWN) {
//                 // 播放音效
//                 playSound(effect);
//             }
//         }

//         SDL_RenderClear(ctx.renderer);
//         if (isVideoPlaying) {
//             playVideoWithAudio(&ctx, "background/test.mp4", &ctx.render_areas[0]);
//         } else {
//             renderAreas(&ctx);  // 渲染所有區域
//         }
//         for (int i = 0; i < 5; i++) {
//             renderButton(&ctx, &buttons[i]);
//         }
//         SDL_RenderPresent(ctx.renderer);
//         SDL_Delay(33);  // 每秒切換第三窗口的圖片
//     }

//     cleanUp(&ctx);  // 清理資源

//     Mix_FreeChunk(effect);  // 釋放音效資源
//     Mix_CloseAudio();       // 關閉音效

//     return 0;
// }
