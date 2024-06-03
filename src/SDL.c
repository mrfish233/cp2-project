

// main.c
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
//結構定義==================================================================================================
//SDL.h中定義

//函式宣告==================================================================================================
//SDL.h中定義
//函數定義==================================================================================================

int stopVideoFlag = 0;
int isVideoPlaying = 0;

//函數定義==================================================================================================


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

    SDL_SetRenderDrawBlendMode(ctx->renderer, SDL_BLENDMODE_BLEND);

    ctx->font = TTF_OpenFont("/home/goose/cp2-project/third_party/font/Noto_Sans_TC/static/NotoSansTC-Black.ttf", 24);
    if (ctx->font == NULL) {
        fprintf(stderr, "Failed to load font: %s\n", TTF_GetError());
        cleanUp(ctx);
        exit(1);
    }

    // 直接設置基準路徑
    strncpy(ctx->base_path, BASE_PATH, sizeof(ctx->base_path) - 1);
    ctx->base_path[sizeof(ctx->base_path) - 1] = '\0';
}



// 載入文字和圖像紋理
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
void renderBackground(AppContext* ctx, RenderArea* area) {
    if (area->is_video) {
        renderVideo(ctx, area);
    } else if (area->background) {
        SDL_RenderCopy(ctx->renderer, area->background, NULL, &area->rect);
    } else {
        SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(ctx->renderer, &area->rect);
    }
}

// 創建渲染區域

// 創建渲染區域
void createRenderArea(AppContext* ctx, int x, int y, int w, int h) {
    printf("Creating render area at (%d, %d) with dimensions %dx%d\n", x, y, w, h);
    ctx->num_render_areas++;
    ctx->render_areas = realloc(ctx->render_areas, ctx->num_render_areas * sizeof(RenderArea));
    ctx->render_areas[ctx->num_render_areas - 1].rect = (SDL_Rect){x, y, w, h};
    ctx->render_areas[ctx->num_render_areas - 1].background = NULL;
    ctx->render_areas[ctx->num_render_areas - 1].images = NULL;
    ctx->render_areas[ctx->num_render_areas - 1].imageRects = NULL;  // 新增初始化
    ctx->render_areas[ctx->num_render_areas - 1].image_count = 0;
    ctx->render_areas[ctx->num_render_areas - 1].texts = NULL;
    ctx->render_areas[ctx->num_render_areas - 1].textColors = NULL;
    ctx->render_areas[ctx->num_render_areas - 1].textRects = NULL;
    ctx->render_areas[ctx->num_render_areas - 1].textTextures = NULL;
    ctx->render_areas[ctx->num_render_areas - 1].text_count = 0;
    ctx->render_areas[ctx->num_render_areas - 1].video_path = NULL;
    ctx->render_areas[ctx->num_render_areas - 1].video_texture = NULL;
    ctx->render_areas[ctx->num_render_areas - 1].is_video = 0;
}




void setRenderAreaContent(AppContext* ctx, int areaIndex, const char* background, char** images, int image_count, char** texts, SDL_Color* textColors, SDL_Rect* textRects, int text_count, const char* video_path, SDL_Rect* imageRect) {
    RenderArea* area = &ctx->render_areas[areaIndex];
    size_t base_path_len = strlen(BASE_PATH); // 直接使用 BASE_PATH

    // 設置背景
    if (background) {
        size_t background_len = strlen(background);
        char* fullPath = (char*)malloc(base_path_len + background_len + 2); // +2 為了加上 '/' 和 '\0'
        snprintf(fullPath, base_path_len + background_len + 2, "%s/%s", BASE_PATH, background); // 使用 BASE_PATH
        printf("Loading background texture from: %s\n", fullPath);  // 調試信息
        area->background = IMG_LoadTexture(ctx->renderer, fullPath);
        if (area->background == NULL) {
            fprintf(stderr, "Failed to load background texture %s: %s\n", fullPath, IMG_GetError());
        }
        free(fullPath);
    }

    // 設置圖片
    area->image_count = image_count;
    area->images = malloc(image_count * sizeof(SDL_Texture*));
    area->imageRects = malloc(image_count * sizeof(SDL_Rect));
    for (int i = 0; i < image_count; i++) {
        size_t image_len = strlen(images[i]);
        char* fullPath = (char*)malloc(base_path_len + image_len + 2); // +2 為了加上 '/' 和 '\0'
        snprintf(fullPath, base_path_len + image_len + 2, "%s/%s", BASE_PATH, images[i]); // 使用 BASE_PATH
        printf("Loading image texture from: %s\n", fullPath);  // 調試信息
        FILE* file = fopen(fullPath, "r");  // 嘗試手動打開文件
        if (file) {
            fclose(file);
            area->images[i] = IMG_LoadTexture(ctx->renderer, fullPath);
            if (area->images[i] == NULL) {
                fprintf(stderr, "Failed to load image texture %s: %s\n", fullPath, IMG_GetError());
            } else {
                printf("Successfully loaded image texture: %s\n", fullPath);
            }
        } else {
            fprintf(stderr, "File does not exist or cannot be opened: %s\n", fullPath);
        }
        if (imageRect != NULL) {
            area->imageRects[i] = *imageRect;  // 設置圖像位置和大小
        }
        free(fullPath);
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

    // 設置影片
    if (video_path) {
        size_t video_len = strlen(video_path);
        char* fullPath = (char*)malloc(base_path_len + video_len + 2); // +2 為了加上 '/' 和 '\0'
        snprintf(fullPath, base_path_len + video_len + 2, "%s/%s", BASE_PATH, video_path); // 使用 BASE_PATH
        area->video_path = strdup(fullPath);
        area->is_video = 1;
        // 初始化影片播放相關的資源
        area->video_texture = NULL;  // 影片紋理初始化為NULL
        free(fullPath);
    } else {
        area->is_video = 0;
    }
}






// 載入影片
int loadVideo(const char* filename, AVFormatContext** pFormatContext, AVCodecContext** pCodecContext, int* videoStreamIndex) {
    AVCodec* pCodec;
    AVCodecParameters* pCodecParameters;

    // 打開影片文件
    if (avformat_open_input(pFormatContext, filename, NULL, NULL) != 0) {
        fprintf(stderr, "Could not open video file: %s\n", filename);
        return -1;
    }

    // 獲取影片流信息
    if (avformat_find_stream_info(*pFormatContext, NULL) < 0) {
        fprintf(stderr, "Could not find stream information\n");
        return -1;
    }

    // 找到影片流
    *videoStreamIndex = -1;
    for (int i = 0; i < (*pFormatContext)->nb_streams; i++) {
        if ((*pFormatContext)->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            *videoStreamIndex = i;
            break;
        }
    }
    if (*videoStreamIndex == -1) {
        fprintf(stderr, "Could not find video stream\n");
        return -1;
    }

    // 獲取編碼參數
    pCodecParameters = (*pFormatContext)->streams[*videoStreamIndex]->codecpar;

    // 找到編碼器
    pCodec = avcodec_find_decoder(pCodecParameters->codec_id);
    if (pCodec == NULL) {
        fprintf(stderr, "Unsupported codec!\n");
        return -1;
    }

    // 配置編碼上下文
    *pCodecContext = avcodec_alloc_context3(pCodec);
    if (avcodec_parameters_to_context(*pCodecContext, pCodecParameters) < 0) {
        fprintf(stderr, "Could not copy codec parameters to context\n");
        return -1;
    }

    // 打開編碼器
    if (avcodec_open2(*pCodecContext, pCodec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        return -1;
    }

    return 0;
}

// 渲染所有區域
void renderAreas(AppContext* ctx) {
    for (int i = 0; i < ctx->num_render_areas; i++) {
        renderBackground(ctx, &ctx->render_areas[i]);
        if (ctx->render_areas[i].is_video) {
            renderVideo(ctx, &ctx->render_areas[i]);
        } else {
            renderImage(ctx, &ctx->render_areas[i]);
            renderText(ctx, &ctx->render_areas[i]);
        }
    }
}

// 渲染影片
void renderVideo(AppContext* ctx, RenderArea* area) {
    if (area->is_video) {
        // 設置目標矩形大小，使影片適應渲染區域
        SDL_Rect dstrect = area->rect;
        SDL_RenderCopy(ctx->renderer, area->video_texture, NULL, &dstrect);
    }
}


void renderImage(AppContext* ctx, RenderArea* area) {
    for (int i = 0; i < area->image_count; i++) {
        SDL_Texture* texture = area->images[i];
        if (texture == NULL) {
            fprintf(stderr, "Failed to load image texture: %s\n", SDL_GetError());
            continue;
        }
        SDL_Rect dstrect = area->imageRects[i];
        printf("Rendering image at (%d, %d) with dimensions %dx%d\n", dstrect.x, dstrect.y, dstrect.w, dstrect.h);
        SDL_RenderCopy(ctx->renderer, texture, NULL, &dstrect);
    }
}

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
        SDL_SetTextureColorMod(texture, area->textColors[i].r, area->textColors[i].g, area->textColors[i].b);
        SDL_RenderCopy(ctx->renderer, texture, NULL, &dstrect);
    }
}

// 清理資源
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
        if (ctx->render_areas[i].video_path) {
            free(ctx->render_areas[i].video_path);
        }
        if (ctx->render_areas[i].video_texture) {
            SDL_DestroyTexture(ctx->render_areas[i].video_texture);
        }
        free(ctx->render_areas[i].images);
        free(ctx->render_areas[i].imageRects);  // 新增清理
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



void playVideoFrame(AppContext* ctx, RenderArea* area, const char* videoPath) {
    size_t base_path_len = strlen(ctx->base_path);
    size_t video_len = strlen(videoPath);
    char* fullPath = (char*)malloc(base_path_len + video_len + 2); // +2 為了加上 '/' 和 '\0'
    snprintf(fullPath, base_path_len + video_len + 2, "%s/%s", ctx->base_path, videoPath);

    // 初始化FFmpeg相关变量
    AVFormatContext* pFormatCtx = avformat_alloc_context();
    if (avformat_open_input(&pFormatCtx, fullPath, NULL, NULL) != 0) {
        fprintf(stderr, "Could not open video file: %s\n", fullPath);
        free(fullPath);
        return;
    }

    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        fprintf(stderr, "Could not find stream information\n");
        free(fullPath);
        return;
    }

    int videoStream = -1;
    for (int i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
            break;
        }
    }

    if (videoStream == -1) {
        fprintf(stderr, "Did not find a video stream\n");
        free(fullPath);
        return;
    }

    AVCodecParameters* pCodecParameters = pFormatCtx->streams[videoStream]->codecpar;
    AVCodec* pCodec = avcodec_find_decoder(pCodecParameters->codec_id);
    if (pCodec == NULL) {
        fprintf(stderr, "Unsupported codec\n");
        free(fullPath);
        return;
    }

    AVCodecContext* pCodecCtx = avcodec_alloc_context3(pCodec);
    if (avcodec_parameters_to_context(pCodecCtx, pCodecParameters) < 0) {
        fprintf(stderr, "Could not copy codec context\n");
        free(fullPath);
        return;
    }

    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        free(fullPath);
        return;
    }

    AVFrame* pFrame = av_frame_alloc();
    AVFrame* pFrameRGB = av_frame_alloc();
    if (pFrameRGB == NULL || pFrame == NULL) {
        fprintf(stderr, "Could not allocate frame\n");
        free(fullPath);
        return;
    }

    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height, 32);
    uint8_t* buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
    av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize, buffer, AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height, 32);

    struct SwsContext* sws_ctx = sws_getContext(
        pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
        pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_RGB24,
        SWS_BILINEAR, NULL, NULL, NULL
    );

    AVPacket packet;
    int frameFinished;

    while (av_read_frame(pFormatCtx, &packet) >= 0 && isVideoPlaying) {
        if (packet.stream_index == videoStream) {
            avcodec_send_packet(pCodecCtx, &packet);
            frameFinished = avcodec_receive_frame(pCodecCtx, pFrame);

            if (frameFinished == 0) {
                sws_scale(
                    sws_ctx, (uint8_t const* const*)pFrame->data,
                    pFrame->linesize, 0, pCodecCtx->height,
                    pFrameRGB->data, pFrameRGB->linesize
                );

                SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
                    pFrameRGB->data[0], pCodecCtx->width, pCodecCtx->height,
                    24, pFrameRGB->linesize[0],
                    0x000000FF, 0x0000FF00, 0x00FF0000, 0
                );
                SDL_Texture* texture = SDL_CreateTextureFromSurface(ctx->renderer, surface);

                SDL_FreeSurface(surface);
                SDL_RenderCopy(ctx->renderer, texture, NULL, &area->rect);
                SDL_DestroyTexture(texture);
                SDL_RenderPresent(ctx->renderer);
                SDL_Delay(33);  // 模拟30fps播放速度
            }
        }
        av_packet_unref(&packet);

        // 检查输入缓冲
        SDL_Event e;
        if (SDL_PollEvent(&e) && e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
            isVideoPlaying = 0;
        }
    }

    av_free(buffer);
    av_frame_free(&pFrameRGB);
    av_frame_free(&pFrame);
    avcodec_free_context(&pCodecCtx);
    avformat_close_input(&pFormatCtx);
    free(fullPath);
}



// 停止影片播放函數
void stopVideo(AppContext* ctx) {
    stopVideoFlag = 1;  // 设置停止标志位
}



void createButton(AppContext* ctx, Button* button, int x, int y, int w, int h, const char* text, void (*onClick)(AppContext* ctx)) {
    button->rect.x = x;
    button->rect.y = y;
    button->rect.w = w;
    button->rect.h = h;
    button->onClick = onClick;

    SDL_Surface* surface = TTF_RenderText_Solid(ctx->font, text, (SDL_Color){255, 255, 255});
    button->texture = SDL_CreateTextureFromSurface(ctx->renderer, surface);
    SDL_FreeSurface(surface);
}

void renderButton(AppContext* ctx, Button* button) {
    SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(ctx->renderer, &button->rect);
    SDL_RenderCopy(ctx->renderer, button->texture, NULL, &button->rect);
}

int isButtonClicked(Button* button, int x, int y) {
    return (x >= button->rect.x && x <= button->rect.x + button->rect.w &&
            y >= button->rect.y && y <= button->rect.y + button->rect.h);
}



