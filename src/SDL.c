#include "SDL.h"

int stopVideoFlag = 0;
int isVideoPlaying = 0;
Uint8* audioBuffer = NULL;
Uint32 audioBufferSize = 0;
Uint32 audioBufferPos = 0;
static int64_t audio_clock = 0;

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

    ctx->font = TTF_OpenFont("third-party/font/Noto_Sans_TC/static/NotoSansTC-Black.ttf", 24);
    if (ctx->font == NULL) {
        fprintf(stderr, "Failed to load font: %s\n", TTF_GetError());
        cleanUp(ctx);
        exit(1);
    }
}

void loadTextures(AppContext* ctx) {
    for (int i = 0; i < ctx->num_render_areas; i++) {
        if (ctx->render_areas[i].text_count > 0) {
            ctx->render_areas[i].textTextures = malloc(ctx->render_areas[i].text_count * sizeof(SDL_Texture*));
            for (int j = 0; j < ctx->render_areas[i].text_count; j++) {
                SDL_Surface* surface = TTF_RenderUTF8_Blended(ctx->font, ctx->render_areas[i].texts[j], ctx->render_areas[i].textColors[j]);
                ctx->render_areas[i].textTextures[j] = SDL_CreateTextureFromSurface(ctx->renderer, surface);
                SDL_FreeSurface(surface);
                if (ctx->render_areas[i].textTextures[j] == NULL) {
                    fprintf(stderr, "Failed to create text texture: %s\n", SDL_GetError());
                }
            }
        }
    }
}

void renderBackground(AppContext* ctx, RenderArea* area) {
    if (area->is_video) {
        renderVideo(ctx, area);
    } else if (area->background) {
        // 設置目標矩形大小，使背景適應渲染區域
        SDL_Rect dstrect = area->rect;
        SDL_RenderCopy(ctx->renderer, area->background, NULL, &dstrect);
    } else {
        SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(ctx->renderer, &area->rect);
    }
}

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

    // 設置背景
    if (background) {
        if (access(background, F_OK) != 0) {
            fprintf(stderr, "File does not exist: %s\n", background);
            return;
        }

        area->background = IMG_LoadTexture(ctx->renderer, background);

        if (area->background == NULL) {
            fprintf(stderr, "Failed to load background texture %s: %s\n", background, IMG_GetError());
        }
    }

    // 設置圖片
    for (int i = 0; i < area->image_count; i++) {
        if (area->images[i]) {
            SDL_DestroyTexture(area->images[i]);
        }
    }

    if (area->images) {
        free(area->images);
        free(area->imageRects);
    }

    area->image_count = image_count;
    area->images      = malloc(image_count * sizeof(SDL_Texture*));
    area->imageRects  = malloc(image_count * sizeof(SDL_Rect));

    for (int i = 0; i < image_count; i++) {
        if (access(images[i], F_OK) != 0) {
            fprintf(stderr, "File does not exist: %s\n", images[i]);
            return;
        }

        area->images[i] = IMG_LoadTexture(ctx->renderer, images[i]);

        if (area->images[i] == NULL) {
            fprintf(stderr, "Failed to load image texture %s: %s\n", images[i], IMG_GetError());
        }
        else {
            printf("Successfully loaded image texture: %s\n", images[i]);
        }

        if (imageRect != NULL) {
            area->imageRects[i] = *imageRect;
        }
    }

    // 設置文字
    if (area->text_count > 0) {
        for (int i = 0; i < area->text_count; i++) {
            if (area->texts[i]) {
                free(area->texts[i]);
            }
            if (area->textTextures[i]) {
                SDL_DestroyTexture(area->textTextures[i]);
            }
        }
    }

    if (area->texts) {
        free(area->texts);
        free(area->textColors);
        free(area->textRects);
        free(area->textTextures);
    }

    area->text_count   = text_count;
    area->texts        = malloc(text_count * sizeof(char*));
    area->textColors   = malloc(text_count * sizeof(SDL_Color));
    area->textRects    = malloc(text_count * sizeof(SDL_Rect));
    area->textTextures = malloc(text_count * sizeof(SDL_Texture*));

    for (int i = 0; i < text_count; i++) {
        area->texts[i] = strdup(texts[i]);
        area->textColors[i] = textColors[i];
        area->textRects[i] = textRects[i];
    }

    // 設置影片
    if (video_path) {
        if (access(video_path, F_OK) != 0) {
            fprintf(stderr, "File does not exist: %s\n", video_path);
            return;
        }

        area->video_path    = strdup(video_path);
        area->is_video      = 1;
        area->video_texture = NULL;
    } else {
        area->is_video = 0;
    }
}

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
        //printf("Rendering image at (%d, %d) with dimensions %dx%d\n", dstrect.x, dstrect.y, dstrect.w, dstrect.h);
        SDL_RenderCopy(ctx->renderer, texture, NULL, &dstrect);
    }
}

void renderText(AppContext* ctx, RenderArea* area) {
    for (int i = 0; i < area->text_count; i++) {
        //printf("Rendering text %d\n", i);
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
    if (access(videoPath, F_OK) != 0) {
        fprintf(stderr, "File does not exist: %s\n", videoPath);
        return;
    }

    // 初始化FFmpeg相关变量
    AVFormatContext* pFormatCtx = avformat_alloc_context();
    if (avformat_open_input(&pFormatCtx, videoPath, NULL, NULL) != 0) {
        fprintf(stderr, "Could not open video file: %s\n", videoPath);
        return;
    }

    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        fprintf(stderr, "Could not find stream information\n");
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
        return;
    }

    AVCodecParameters* pCodecParameters = pFormatCtx->streams[videoStream]->codecpar;
    AVCodec* pCodec = avcodec_find_decoder(pCodecParameters->codec_id);
    if (pCodec == NULL) {
        fprintf(stderr, "Unsupported codec\n");
        return;
    }

    AVCodecContext* pCodecCtx = avcodec_alloc_context3(pCodec);
    if (avcodec_parameters_to_context(pCodecCtx, pCodecParameters) < 0) {
        fprintf(stderr, "Could not copy codec context\n");
        return;
    }

    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        return;
    }

    AVFrame* pFrame = av_frame_alloc();
    AVFrame* pFrameRGB = av_frame_alloc();
    if (pFrameRGB == NULL || pFrame == NULL) {
        fprintf(stderr, "Could not allocate frame\n");
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
}

void stopVideo(AppContext* ctx) {
    stopVideoFlag = 1;  // 设置停止标志位
}

void createButton(AppContext* ctx, Button* button, int x, int y, int w, int h, const char* text, void (*onClick)(AppContext* ctx)) {
    button->rect.x = x;
    button->rect.y = y;
    button->rect.w = w;
    button->rect.h = h;
    button->onClick = onClick;
    strncpy(button->text, text, sizeof(button->text) - 1);  // 確保保存文本

    SDL_Surface* surface = TTF_RenderUTF8_Blended(ctx->font, text, (SDL_Color){255, 255, 255, 255});
    button->texture = SDL_CreateTextureFromSurface(ctx->renderer, surface);
    SDL_FreeSurface(surface);
}

void renderButton(AppContext* ctx, Button* button) {
    // 清除按鈕區域
    SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(ctx->renderer, &button->rect);

    // 渲染按鈕背景
    SDL_SetRenderDrawColor(ctx->renderer, 255, 255, 255, 255); // 白色背景
    SDL_Rect bgRect = {button->rect.x + 2, button->rect.y + 2, button->rect.w - 4, button->rect.h - 4};
    SDL_RenderFillRect(ctx->renderer, &bgRect);

    // 渲染按鈕文字
    SDL_Surface* textSurface = TTF_RenderUTF8_Blended(ctx->font, button->text, (SDL_Color){0, 0, 0, 255});
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(ctx->renderer, textSurface);
    SDL_Rect textRect = {button->rect.x + (button->rect.w - textSurface->w) / 2, button->rect.y + (button->rect.h - textSurface->h) / 2, textSurface->w, textSurface->h};
    SDL_RenderCopy(ctx->renderer, textTexture, NULL, &textRect);

    // 清理資源
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

bool isButtonClicked(Button* button, int x, int y) {
    return (x >= button->rect.x && x <= button->rect.x + button->rect.w &&
            y >= button->rect.y && y <= button->rect.y + button->rect.h);
}

void initAudio() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        exit(1);
    }
}

void loadSound(const char* file, Mix_Chunk** chunk) {
    *chunk = Mix_LoadWAV(file);
    if (*chunk == NULL) {
        fprintf(stderr, "Failed to load sound effect! SDL_mixer Error: %s\n", Mix_GetError());
    }
}

void playSound(Mix_Chunk* chunk) {
    Mix_PlayChannel(-1, chunk, 0);
}

void audioCallback(void* userdata, Uint8* stream, int len) {
    AudioData* audio = (AudioData*)userdata;

    if (audio->length == 0) {
        return;
    }

    len = (len > audio->length ? audio->length : len);
    SDL_memcpy(stream, audio->pos, len);
    audio->pos += len;
    audio->length -= len;
}

void playAudioFrame(AVCodecContext* codecCtx, AVFrame* frame) {
    static struct SwrContext* swr_ctx = NULL;
    static Uint8* audio_buf = NULL;
    static int audio_buf_size = 0;

    // 初始化 SwrContext
    if (!swr_ctx) {
        swr_ctx = swr_alloc_set_opts(
            NULL,
            av_get_default_channel_layout(codecCtx->channels),
            AV_SAMPLE_FMT_S16,
            codecCtx->sample_rate,
            av_get_default_channel_layout(codecCtx->channels),
            codecCtx->sample_fmt,
            codecCtx->sample_rate,
            0, NULL
        );
        if (!swr_ctx || swr_init(swr_ctx) < 0) {
            fprintf(stderr, "Could not allocate or initialize resample context\n");
            if (swr_ctx) swr_free(&swr_ctx);
            return;
        }
    }

    int data_size = av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
    if (data_size < 0) {
        fprintf(stderr, "Failed to calculate data size\n");
        return;
    }

    int num_samples = frame->nb_samples;
    int num_channels = codecCtx->channels;
    int required_buf_size = num_samples * num_channels * data_size;

    // 檢查緩衝區大小並分配內存
    if (!audio_buf || audio_buf_size < required_buf_size) {
        if (audio_buf) free(audio_buf);
        audio_buf = (Uint8*)malloc(required_buf_size);
        if (!audio_buf) {
            fprintf(stderr, "Could not allocate audio buffer\n");
            return;
        }
        audio_buf_size = required_buf_size;
    }

    const uint8_t** in = (const uint8_t**)frame->data;
    uint8_t* out[] = {audio_buf};
    int ret = swr_convert(swr_ctx, out, num_samples, in, num_samples);
    if (ret < 0) {
        fprintf(stderr, "Error while converting audio frame\n");
        return;
    }

    int audio_buf_len = ret * num_channels * data_size;
    Mix_Chunk* chunk = Mix_QuickLoad_RAW(audio_buf, audio_buf_len);
    if (!chunk) {
        fprintf(stderr, "Failed to load audio chunk: %s\n", Mix_GetError());
        return;
    }

    // 計算當前音訊時間戳
    audio_clock = frame->pts * av_q2d(codecCtx->time_base);

    Mix_PlayChannel(-1, chunk, 0);
}

void playVideoWithAudio(AppContext* ctx, const char* videoPath, RenderArea* area) {
    if (access(videoPath, F_OK) != 0) {
        fprintf(stderr, "File does not exist: %s\n", videoPath);
        return;
    }

    // 初始化FFmpeg相关变量
    AVFormatContext* pFormatCtx = avformat_alloc_context();
    if (avformat_open_input(&pFormatCtx, videoPath, NULL, NULL) != 0) {
        fprintf(stderr, "Could not open video file: %s\n", videoPath);
        return;
    }

    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        fprintf(stderr, "Could not find stream information\n");
        return;
    }

    int videoStream = -1;
    int audioStream = -1;
    for (int i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && videoStream == -1) {
            videoStream = i;
        } else if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && audioStream == -1) {
            audioStream = i;
        }
    }

    if (videoStream == -1) {
        fprintf(stderr, "Did not find a video stream\n");
        return;
    }
    if (audioStream == -1) {
        fprintf(stderr, "Did not find an audio stream\n");
        return;
    }

    AVCodecParameters* pVideoCodecParameters = pFormatCtx->streams[videoStream]->codecpar;
    AVCodec* pVideoCodec = avcodec_find_decoder(pVideoCodecParameters->codec_id);
    if (pVideoCodec == NULL) {
        fprintf(stderr, "Unsupported video codec\n");
        return;
    }

    AVCodecContext* pVideoCodecCtx = avcodec_alloc_context3(pVideoCodec);
    if (avcodec_parameters_to_context(pVideoCodecCtx, pVideoCodecParameters) < 0) {
        fprintf(stderr, "Could not copy video codec context\n");
        return;
    }

    if (avcodec_open2(pVideoCodecCtx, pVideoCodec, NULL) < 0) {
        fprintf(stderr, "Could not open video codec\n");
        return;
    }

    AVCodecParameters* pAudioCodecParameters = pFormatCtx->streams[audioStream]->codecpar;
    AVCodec* pAudioCodec = avcodec_find_decoder(pAudioCodecParameters->codec_id);
    if (pAudioCodec == NULL) {
        fprintf(stderr, "Unsupported audio codec\n");
        return;
    }

    AVCodecContext* pAudioCodecCtx = avcodec_alloc_context3(pAudioCodec);
    if (avcodec_parameters_to_context(pAudioCodecCtx, pAudioCodecParameters) < 0) {
        fprintf(stderr, "Could not copy audio codec context\n");
        return;
    }

    if (avcodec_open2(pAudioCodecCtx, pAudioCodec, NULL) < 0) {
        fprintf(stderr, "Could not open audio codec\n");
        return;
    }

    AVFrame* pFrame = av_frame_alloc();
    AVFrame* pFrameRGB = av_frame_alloc();
    if (pFrameRGB == NULL || pFrame == NULL) {
        fprintf(stderr, "Could not allocate frame\n");
        return;
    }

    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, pVideoCodecCtx->width, pVideoCodecCtx->height, 32);
    uint8_t* buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
    av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize, buffer, AV_PIX_FMT_RGB24, pVideoCodecCtx->width, pVideoCodecCtx->height, 32);

    struct SwsContext* sws_ctx = sws_getContext(
        pVideoCodecCtx->width, pVideoCodecCtx->height, pVideoCodecCtx->pix_fmt,
        pVideoCodecCtx->width, pVideoCodecCtx->height, AV_PIX_FMT_RGB24,
        SWS_BILINEAR, NULL, NULL, NULL
    );

    AVPacket packet;
    int frameFinished;

    while (av_read_frame(pFormatCtx, &packet) >= 0 && isVideoPlaying) {
        if (packet.stream_index == videoStream) {
            avcodec_send_packet(pVideoCodecCtx, &packet);
            frameFinished = avcodec_receive_frame(pVideoCodecCtx, pFrame);

            if (frameFinished == 0) {
                sws_scale(
                    sws_ctx, (uint8_t const* const*)pFrame->data,
                    pFrame->linesize, 0, pVideoCodecCtx->height,
                    pFrameRGB->data, pFrameRGB->linesize
                );

                SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
                    pFrameRGB->data[0], pVideoCodecCtx->width, pVideoCodecCtx->height,
                    24, pFrameRGB->linesize[0],
                    0x000000FF, 0x0000FF00, 0x00FF0000, 0
                );
                SDL_Texture* texture = SDL_CreateTextureFromSurface(ctx->renderer, surface);

                SDL_FreeSurface(surface);
                SDL_RenderCopy(ctx->renderer, texture, NULL, &area->rect);
                SDL_DestroyTexture(texture);
                SDL_RenderPresent(ctx->renderer);

                // 計算當前影像時間戳
                double video_clock = pFrame->pts * av_q2d(pVideoCodecCtx->time_base);

                // 同步音訊和影像
                if (audio_clock > video_clock ) {
                    int delay = (int)((audio_clock - video_clock + 1.5) * 1000);
                    SDL_Delay(delay);
                } else {
                    SDL_Delay(33);  // 每秒30幀，模擬播放速度
                }
            }
        } else if (packet.stream_index == audioStream) {
            avcodec_send_packet(pAudioCodecCtx, &packet);
            frameFinished = avcodec_receive_frame(pAudioCodecCtx, pFrame);

            if (frameFinished == 0) {
                playAudioFrame(pAudioCodecCtx, pFrame);
            }
        }
        av_packet_unref(&packet);

        // 檢查輸入緩衝區
        SDL_Event e;
        if (SDL_PollEvent(&e) && e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
            isVideoPlaying = 0;
        }
    }

    av_free(buffer);
    av_frame_free(&pFrameRGB);
    av_frame_free(&pFrame);
    avcodec_free_context(&pVideoCodecCtx);
    avcodec_free_context(&pAudioCodecCtx);
    avformat_close_input(&pFormatCtx);
}
