#include "UI.h"

// 按鈕點擊事件回調函數

// 全域flag
bool newGameFlag = false;
bool EndFlag = false;
bool GamePlayingFlag = false;
bool LoadFlag = false;
bool CreditFlag = false;

SDL_Color white = {255, 255, 255, 255}; // 定義全域變數

int process() {
    AppContext ctx = {NULL, NULL, NULL, 1920, 1080, NULL, 0, 0};
    initSDL(&ctx);

    // 創建新的渲染區域
    createRenderArea(&ctx, 0, 0, ctx.window_width, ctx.window_height); // 顯示主菜單的區域

    while (1) {
        if (EndFlag) {
            End();
        } else if (LoadFlag) {
            Load(&ctx);
        } else if (GamePlayingFlag) {
            GamePlaying(&ctx);
        } else if (CreditFlag) {
            Credit();
        } else {
            MainMenu(&ctx);
        }
        
        // 重置標誌位，避免重複執行
        newGameFlag = false;
    }

    cleanUp(&ctx);
    return 0;
}


void onClickNewGame(AppContext* ctx) {
    printf("Button 'New Game' clicked\n");
    newGameFlag = true;
    GamePlayingFlag = true;
}


// main.c

void onClickResume(AppContext* ctx) {
    printf("Button 'Resume' clicked\n");
    LoadFlag = false;  // 取消 Load 狀態
    GamePlayingFlag = true; // 設置 GamePlaying 狀態
}

void onClickSave(AppContext* ctx) {
    printf("Button 'Save' clicked\n");
    LoadFlag = true;  // 設置 Load 狀態
    GamePlayingFlag = false; // 取消 GamePlaying 狀態
}

void onClickMainMenu(AppContext* ctx) {
    printf("Button 'Main Menu' clicked\n");
    LoadFlag = false;  // 取消 Load 狀態
    GamePlayingFlag = false; // 取消 GamePlaying 狀態
    CreditFlag = false; // 取消 Credit 狀態
}

// Settings 界面
void Settings(AppContext* ctx) {
    Button buttons[3];
    createButton(ctx, &buttons[0], ctx->window_width / 2 - 100, ctx->window_height / 2 - 75, 200, 50, "Resume", onClickResume);
    createButton(ctx, &buttons[1], ctx->window_width / 2 - 100, ctx->window_height / 2, 200, 50, "Save", onClickSave);
    createButton(ctx, &buttons[2], ctx->window_width / 2 - 100, ctx->window_height / 2 + 75, 200, 50, "Main Menu", onClickMainMenu);

    bool quit = false;
    SDL_Event e;

    // 設置設定背景
    setRenderAreaContent(ctx, 0, "background/start.png", NULL, 0, NULL, NULL, NULL, 0, NULL, NULL);

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                EndFlag = true;
                quit = true;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x = e.button.x;
                int y = e.button.y;
                for (int i = 0; i < 3; i++) {
                    if (isButtonClicked(&buttons[i], x, y)) {
                        buttons[i].onClick(ctx);
                        quit = true;
                        break;
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 255);
        SDL_RenderClear(ctx->renderer);

        // 渲染設定背景
        renderBackground(ctx, &ctx->render_areas[0]);

        for (int i = 0; i < 3; i++) {
            renderButton(ctx, &buttons[i]);
        }

        SDL_RenderPresent(ctx->renderer);
    }
}

void onClickSetting(AppContext* ctx) {
    printf("Button 'Setting' clicked\n");
    GamePlayingFlag = false; // 退出 GamePlaying 狀態
    printf("Entering Settings\n"); // 添加診斷輸出
    Settings(ctx); // 進入設定狀態
}



void onClickLoad(AppContext* ctx) {
    printf("Button 'Load' clicked\n");
    LoadFlag = true;
}

void onClickCredit(AppContext* ctx) {
    printf("Button 'Credit' clicked\n");
    CreditFlag = true;
}

void onClickExit(AppContext* ctx) {
    printf("Button 'Exit' clicked\n");
    EndFlag = true;
}


// 初始化按鈕
void initMenuButtons(AppContext* ctx, Button* buttons) {
    createButton(ctx, &buttons[0], ctx->window_width / 2 - 100, ctx->window_height / 2 - 100, 200, 50, "New Game", onClickNewGame);
    createButton(ctx, &buttons[1], ctx->window_width / 2 - 100, ctx->window_height / 2 - 25, 200, 50, "Load", onClickLoad);
    createButton(ctx, &buttons[2], ctx->window_width / 2 - 100, ctx->window_height / 2 + 50, 200, 50, "Credit", onClickCredit);
    createButton(ctx, &buttons[3], ctx->window_width / 2 - 100, ctx->window_height / 2 + 125, 200, 50, "Exit", onClickExit);
}

// 渲染標題
void renderTitle(AppContext* ctx) {
    SDL_Surface* surface = TTF_RenderUTF8_Blended(ctx->font, "失落的旋律", white);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(ctx->renderer, surface);
    SDL_Rect dstrect = {ctx->window_width / 2 - surface->w / 2, 50, surface->w, surface->h};
    SDL_RenderCopy(ctx->renderer, texture, NULL, &dstrect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void MainMenu(AppContext* ctx) {
    Button buttons[4];
    initMenuButtons(ctx, buttons);

    bool quit = false;
    SDL_Event e;

    // 設置主菜單背景
    setRenderAreaContent(ctx, 0, "background/start.png", NULL, 0, NULL, NULL, NULL, 0, NULL, NULL);

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                EndFlag = true;
                quit = true;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x = e.button.x;
                int y = e.button.y;
                for (int i = 0; i < 4; i++) {
                    if (isButtonClicked(&buttons[i], x, y)) {
                        buttons[i].onClick(ctx);
                        quit = true;
                        break;
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 255);
        SDL_RenderClear(ctx->renderer);

        // 渲染主菜單背景
        renderBackground(ctx, &ctx->render_areas[0]);

        renderTitle(ctx);
        for (int i = 0; i < 4; i++) {
            renderButton(ctx, &buttons[i]);
        }

        SDL_RenderPresent(ctx->renderer);
    }
}

void GamePlaying(AppContext* ctx) {
    // 創建四個渲染區域，每個區域大小為視窗的四分之一
    int section_width = ctx->window_width / 2;
    int section_height = ctx->window_height / 2;
    createRenderArea(ctx, 0, 0, section_width, section_height); // 左上區
    createRenderArea(ctx, section_width, 0, section_width, section_height); // 右上區
    createRenderArea(ctx, 0, section_height, section_width, section_height); // 左下區
    createRenderArea(ctx, section_width, section_height, section_width, section_height); // 右下區

    // 左上區設置按鈕
    Button settingButton;
    createButton(ctx, &settingButton, 10, 10, 100, 50, "Setting", onClickSetting);
    printf("Setting button created and onClickSetting bound.\n"); // 添加診斷輸出

    // 右上區設置文字
    char* itemTexts[] = {"item1", "item2", "item3", "item4", "item5", "status1", "status2", "status3", "status4", "status5"};
    SDL_Rect itemRects[10];
    for (int i = 0; i < 10; i++) {
        itemRects[i].x = section_width + 10; // 調整到右上區
        itemRects[i].y = 30 * i;
        itemRects[i].w = 100;
        itemRects[i].h = 20;
    }
    
    SDL_Color itemColors[10];
    for (int i = 0; i < 10; i++) {
        itemColors[i] = white;
    }
    setRenderAreaContent(ctx, 1, "background/beach.png", NULL, 0, itemTexts, itemColors, itemRects, 10, NULL, NULL);

    // 左下區設置圖片
    char* images[] = {"character/lulu/tachie/angry.png"};
    SDL_Rect imageRect = {10, section_height + section_height - 210, 200, 200}; // 調整到左下區
    setRenderAreaContent(ctx, 2, "background/beach.png", images, 1, NULL, NULL, NULL, 0, NULL, &imageRect);

    // 右下區設置可輸入的文字方塊
    SDL_Color black = {0, 0, 0, 255}; // 黑色
    SDL_Color inputTextColor = {255, 255, 255, 255}; // 白色
    char inputText[256] = {0};
    int textLength = 0;

    loadTextures(ctx);

    // 渲染循環
    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
                EndFlag = true;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x = e.button.x;
                int y = e.button.y;
                printf("Mouse click detected at (%d, %d)\n", x, y); // 添加診斷輸出
                if (isButtonClicked(&settingButton, x, y)) {
                    printf("Setting button clicked.\n"); // 添加診斷輸出
                    settingButton.onClick(ctx);
                    quit = true;
                }
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN) {
                // 清空輸入框內容
                memset(inputText, 0, sizeof(inputText));
                textLength = 0;
            } else if (e.type == SDL_TEXTINPUT) {
                // 處理文字輸入
                if (textLength < sizeof(inputText) - 1) {
                    strcat(inputText, e.text.text);
                    textLength += strlen(e.text.text);
                }
            }
        }

        SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 255);
        SDL_RenderClear(ctx->renderer);

        // 依次渲染各個區域，確保渲染順序
        renderBackground(ctx, &ctx->render_areas[0]);
        renderBackground(ctx, &ctx->render_areas[1]);
        renderBackground(ctx, &ctx->render_areas[2]);
        SDL_SetRenderDrawColor(ctx->renderer, black.r, black.g, black.b, black.a);
        SDL_RenderFillRect(ctx->renderer, &ctx->render_areas[3].rect);

        renderText(ctx, &ctx->render_areas[1]);
        renderImage(ctx, &ctx->render_areas[2]);

        // 渲染輸入框內的文字
        if (strlen(inputText) > 0) {
            SDL_Surface* textSurface = TTF_RenderUTF8_Blended(ctx->font, inputText, inputTextColor);
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(ctx->renderer, textSurface);
            SDL_Rect textRect = {section_width + 10, section_height + section_height / 2 - 15, textSurface->w, textSurface->h};
            SDL_RenderCopy(ctx->renderer, textTexture, NULL, &textRect);
            SDL_FreeSurface(textSurface);
            SDL_DestroyTexture(textTexture);
        }

        renderButton(ctx, &settingButton); // 渲染按鈕在最上層

        SDL_RenderPresent(ctx->renderer);
    }
}







void BackToMainMenu(AppContext* ctx) {
    printf("Button 'Back' clicked\n");
    LoadFlag = false;  // 返回主菜單，將LoadFlag設置為false
}

// main.c

void onClickSaveSlot1(AppContext* ctx) {
    printf("Button 'Save 1' clicked\n");
    // 添加保存邏輯
}

void onClickLoadSlot1(AppContext* ctx) {
    printf("Button 'Load 1' clicked\n");
    // 添加加載邏輯
}

void onClickSaveSlot2(AppContext* ctx) {
    printf("Button 'Save 2' clicked\n");
    // 添加保存邏輯
}

void onClickLoadSlot2(AppContext* ctx) {
    printf("Button 'Load 2' clicked\n");
    // 添加加載邏輯
}

void onClickSaveSlot3(AppContext* ctx) {
    printf("Button 'Save 3' clicked\n");
    // 添加保存邏輯
}

void onClickLoadSlot3(AppContext* ctx) {
    printf("Button 'Load 3' clicked\n");
    // 添加加載邏輯
}

void onClickSaveAuto(AppContext* ctx) {
    printf("Button 'Auto Save' clicked\n");
    // 添加自動保存邏輯
}

void onClickLoadAuto(AppContext* ctx) {
    printf("Button 'Load Auto' clicked\n");
    // 添加自動加載邏輯
}

void Load(AppContext* ctx) {
    // 創建返回主菜單的按鈕
    Button backButton;
    createButton(ctx, &backButton, ctx->window_width - 110, 10, 100, 50, "Back", BackToMainMenu);

    // 創建四個渲染區域，每個區域高度為視窗高度的四分之一
    int section_height = ctx->window_height / 4;
    createRenderArea(ctx, 0, 0, ctx->window_width, section_height); // 第一部分
    createRenderArea(ctx, 0, section_height, ctx->window_width, section_height); // 第二部分
    createRenderArea(ctx, 0, 2 * section_height, ctx->window_width, section_height); // 第三部分
    createRenderArea(ctx, 0, 3 * section_height, ctx->window_width, section_height); // 第四部分

    // 設置按鈕和文本框
    Button saveButtons[4];
    Button loadButtons[4];
    SDL_Rect textRects[4];
    char* texts[4] = {"進度:", "進度:", "進度:", "進度:"};

    createButton(ctx, &saveButtons[0], 10, section_height / 2 - 25, 100, 50, "Save 1", onClickSaveSlot1);
    createButton(ctx, &loadButtons[0], 120, section_height / 2 - 25, 100, 50, "Load 1", onClickLoadSlot1);
    textRects[0] = (SDL_Rect){230, section_height / 2 - 25, 100, 50};

    createButton(ctx, &saveButtons[1], 10, section_height + section_height / 2 - 25, 100, 50, "Save 2", onClickSaveSlot2);
    createButton(ctx, &loadButtons[1], 120, section_height + section_height / 2 - 25, 100, 50, "Load 2", onClickLoadSlot2);
    textRects[1] = (SDL_Rect){230, section_height + section_height / 2 - 25, 100, 50};

    createButton(ctx, &saveButtons[2], 10, 2 * section_height + section_height / 2 - 25, 100, 50, "Save 3", onClickSaveSlot3);
    createButton(ctx, &loadButtons[2], 120, 2 * section_height + section_height / 2 - 25, 100, 50, "Load 3", onClickLoadSlot3);
    textRects[2] = (SDL_Rect){230, 2 * section_height + section_height / 2 - 25, 100, 50};

    createButton(ctx, &saveButtons[3], 10, 3 * section_height + section_height / 2 - 25, 100, 50, "Auto Save", onClickSaveAuto);
    createButton(ctx, &loadButtons[3], 120, 3 * section_height + section_height / 2 - 25, 100, 50, "Load Auto", onClickLoadAuto);
    textRects[3] = (SDL_Rect){230, 3 * section_height + section_height / 2 - 25, 100, 50};

    loadTextures(ctx);

    // 渲染循環
    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
                EndFlag = true;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x = e.button.x;
                int y = e.button.y;
                if (isButtonClicked(&backButton, x, y)) {
                    backButton.onClick(ctx);
                    quit = true;
                } else {
                    for (int i = 0; i < 4; i++) {
                        if (isButtonClicked(&saveButtons[i], x, y)) {
                            saveButtons[i].onClick(ctx);
                        }
                        if (isButtonClicked(&loadButtons[i], x, y)) {
                            loadButtons[i].onClick(ctx);
                        }
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 255);
        SDL_RenderClear(ctx->renderer);

        renderAreas(ctx);
        renderButton(ctx, &backButton);
        for (int i = 0; i < 4; i++) {
            renderButton(ctx, &saveButtons[i]);
            renderButton(ctx, &loadButtons[i]);
            SDL_Surface* surface = TTF_RenderUTF8_Blended(ctx->font, texts[i], white);
            SDL_Texture* texture = SDL_CreateTextureFromSurface(ctx->renderer, surface);
            SDL_RenderCopy(ctx->renderer, texture, NULL, &textRects[i]);
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
        }

        SDL_RenderPresent(ctx->renderer);
    }
}


void End() {
    // 結束遊戲的代碼
    printf("Ending the game...\n");
    exit(0); // 退出程序
}

void Credit() {
    // 顯示製作群的代碼
    printf("Displaying credits...\n");
    CreditFlag = false; // 退出製作群狀態
}


