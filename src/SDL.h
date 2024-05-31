#ifndef SDL_H
#define SDL_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <unistd.h>  // 用於 getcwd
#include <limits.h>  // 用於 PATH_MAX
#include <linux/limits.h>


// 定義基準路徑
#define BASE_PATH "/home/goose/cp2-project/example-game/assests"
#define MAIN_PATH /home/goose/cp2-project/example-game/assests

extern int stopVideoFlag;
extern int isVideoPlaying;
// 結構定義==================================================================================================
typedef struct {
    SDL_Rect rect;
    SDL_Texture* background;
    SDL_Texture** images;
    SDL_Rect* imageRects;  // 新增字段
    int image_count;
    char** texts;
    SDL_Color* textColors;
    SDL_Rect* textRects;
    SDL_Texture** textTextures;
    int text_count;
    // 新增影片相關的字段
    char* video_path;
    SDL_Texture* video_texture;
    SDL_Rect videoRect;  // 影片位置和大小
    int is_video;
} RenderArea;

// 其他結構和函數宣告
typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    int window_width;
    int window_height;
    RenderArea* render_areas;
    int num_render_areas;
    int switchCounter;
    int isPlayingVideo;
    

    char base_path[PATH_MAX];  // 新增字段，用於保存基準路徑
} AppContext;

typedef struct {
    SDL_Texture *texture;
    SDL_Rect rect;
} Button;


// 函式宣告=================================================

/**
 * 初始化SDL
 * @param ctx 應用程式上下文
 */
void initSDL(AppContext* ctx);

/**
 * 載入文字和圖像紋理
 * @param ctx 應用程式上下文
 */
void loadTextures(AppContext* ctx);

/**
 * 渲染背景
 * @param ctx 應用程式上下文
 * @param area 渲染區域
 */
void renderBackground(AppContext* ctx, RenderArea* area);

/**
 * 創建渲染區域
 * @param ctx 應用程式上下文
 * @param x 渲染區域的X座標
 * @param y 渲染區域的Y座標
 * @param w 渲染區域的寬度
 * @param h 渲染區域的高度
 */
void createRenderArea(AppContext* ctx, int x, int y, int w, int h);

/**
 * 設置渲染區域內容
 * @param ctx 應用程式上下文
 * @param areaIndex 渲染區域索引
 * @param background 背景圖像
 * @param images 圖片數組
 * @param image_count 圖片數量
 * @param texts 文字數組
 * @param textColors 文字顏色數組
 * @param textRects 文字位置和大小數組
 * @param text_count 文字數量
 * @param video_path 影片路徑
 */
void setRenderAreaContent(AppContext* ctx, int areaIndex, const char* background, char** images, int image_count, char** texts, SDL_Color* textColors, SDL_Rect* textRects, int text_count, const char* video_path, SDL_Rect* videoRect);
/**
 * 渲染所有區域
 * @param ctx 應用程式上下文
 */
void renderAreas(AppContext* ctx);

/**
 * 渲染影片
 * @param ctx 應用程式上下文
 * @param area 渲染區域
 */
void renderVideo(AppContext* ctx, RenderArea* area);

/**
 * 渲染圖片
 * @param ctx 應用程式上下文
 * @param area 渲染區域
 */
void renderImage(AppContext* ctx, RenderArea* area);

/**
 * 渲染文字
 * @param ctx 應用程式上下文
 * @param area 渲染區域
 */
void renderText(AppContext* ctx, RenderArea* area);

/**
 * 清理資源
 * @param ctx 應用程式上下文
 */
void cleanUp(AppContext* ctx);


/**
 * 更新影片幀
 * @param ctx 應用程式上下文
 * @param areaIndex 渲染區域索引
 */
void updateVideoFrame(AppContext* ctx, int areaIndex);
void playVideo(AppContext* ctx, const char* video_path, RenderArea* area);
void stopVideo(AppContext* ctx);
void initFFmpeg();
void* videoPlaybackThread(void* arg);

void playVideoFrame(AppContext* ctx, RenderArea* area, const char* videoPath);
#endif
