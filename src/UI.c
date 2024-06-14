#include "UI.h"

// 全域flag
bool newGameFlag = false;
bool EndFlag = false;
bool GamePlayingFlag = false;
bool LoadFlag = false;
bool CreditFlag = false;

SDL_Color white = {255, 255, 255, 255}; // 定義全域變數
SDL_Color black = {0, 0, 0, 255};

int startEngine(char *dir) {
    AppContext ctx = { NULL, NULL, NULL, 1300, 700, NULL, 0, 0, 0 };
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

void Settings(AppContext* ctx) {
    Button buttons[3];
    createButton(ctx, &buttons[0], ctx->window_width / 2 - 100, ctx->window_height / 2 - 75, 200, 50, "Resume", onClickResume);
    createButton(ctx, &buttons[1], ctx->window_width / 2 - 100, ctx->window_height / 2, 200, 50, "Save", onClickSave);
    createButton(ctx, &buttons[2], ctx->window_width / 2 - 100, ctx->window_height / 2 + 75, 200, 50, "Main Menu", onClickMainMenu);

    bool quit = false;
    SDL_Event e;

    // 設置設定背景
    setRenderAreaContent(ctx, 0, "example-game/assets/background/start.png", NULL, 0, NULL, NULL, NULL, 0, NULL, NULL);

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

void initMenuButtons(AppContext* ctx, Button* buttons) {
    createButton(ctx, &buttons[0], ctx->window_width / 2 - 100, ctx->window_height / 2 - 100, 200, 50, "New Game", onClickNewGame);
    createButton(ctx, &buttons[1], ctx->window_width / 2 - 100, ctx->window_height / 2 - 25, 200, 50, "Load", onClickLoad);
    createButton(ctx, &buttons[2], ctx->window_width / 2 - 100, ctx->window_height / 2 + 50, 200, 50, "Credit", onClickCredit);
    createButton(ctx, &buttons[3], ctx->window_width / 2 - 100, ctx->window_height / 2 + 125, 200, 50, "Exit", onClickExit);
}

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
    setRenderAreaContent(ctx, 0, "example-game/assets/background/start.png", NULL, 0, NULL, NULL, NULL, 0, NULL, NULL);

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

void onClickItemNextPage(AppContext* ctx) {
    printf("Button 'Item Next Page' clicked\n");
    // Implement item next page logic
}

void onClickItemPreviousPage(AppContext* ctx) {
    printf("Button 'Item Previous Page' clicked\n");
    // Implement item previous page logic
}

void onClickStatusNextPage(AppContext* ctx) {
    printf("Button 'Status Next Page' clicked\n");
    // Implement status next page logic
}

void onClickStatusPreviousPage(AppContext* ctx) {
    printf("Button 'Status Previous Page' clicked\n");
    // Implement status previous page logic
}

void BackToMainMenu(AppContext* ctx) {
    printf("Button 'Back' clicked\n");
    LoadFlag = false;  // 返回主菜單，將LoadFlag設置為false
}

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

    createButton(ctx, &saveButtons[0], 10, section_height / 2 - 25, 200, 50, "Save 1", onClickSaveSlot1);
    createButton(ctx, &loadButtons[0], 220, section_height / 2 - 25, 200, 50, "Load 1", onClickLoadSlot1);
    textRects[0] = (SDL_Rect){430, section_height / 2 - 25, 100, 60};

    createButton(ctx, &saveButtons[1], 10, section_height + section_height / 2 - 25, 200, 50, "Save 2", onClickSaveSlot2);
    createButton(ctx, &loadButtons[1], 220, section_height + section_height / 2 - 25, 200, 50, "Load 2", onClickLoadSlot2);
    textRects[1] = (SDL_Rect){430, section_height + section_height / 2 - 25, 100, 60};

    createButton(ctx, &saveButtons[2], 10, 2 * section_height + section_height / 2 - 25, 200, 50, "Save 3", onClickSaveSlot3);
    createButton(ctx, &loadButtons[2], 220, 2 * section_height + section_height / 2 - 25, 200, 50, "Load 3", onClickLoadSlot3);
    textRects[2] = (SDL_Rect){430, 2 * section_height + section_height / 2 - 25, 100, 60};

    createButton(ctx, &saveButtons[3], 10, 3 * section_height + section_height / 2 - 25, 200, 50, "Auto Save", onClickSaveAuto);
    createButton(ctx, &loadButtons[3], 220, 3 * section_height + section_height / 2 - 25, 200, 50, "Load Auto", onClickLoadAuto);
    textRects[3] = (SDL_Rect){430, 3 * section_height + section_height / 2 - 25, 100, 60};

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

//實驗================================================================================================
typedef struct {
    Button button;
    bool selectable;
} OptionButton;

// 定義全域變數
Button settingButton;
OptionButton optionButtons[5];
bool showOptionButtons = false;

// 各選項按鈕的回調函數
void onClickOption1(AppContext* ctx) {
    printf("Button '追擊' clicked\n");
}

void onClickOption2(AppContext* ctx) {
    printf("Button '防禦' clicked\n");
}

void onClickOption3(AppContext* ctx) {
    printf("Button '逃跑' clicked\n");
}

void onClickOption4(AppContext* ctx) {
    printf("Button '攻擊' clicked\n");
}

void onClickOption5(AppContext* ctx) {
    printf("Button 'Option 5' clicked\n");
}

// 處理信號並顯示按鈕
void handleSignal(AppContext* ctx, int numButtons, bool buttonStates[]) {
    for (int i = 0; i < numButtons; i++) {
        const char* buttonText;
        void (*onClick)(AppContext*) = NULL;
        switch (i) {
            case 0:
                buttonText = "追擊";
                onClick = onClickOption1;
                break;
            case 1:
                buttonText = "防禦";
                onClick = onClickOption2;
                break;
            case 2:
                buttonText = "逃跑";
                onClick = onClickOption3;
                break;
            case 3:
                buttonText = "攻擊";
                onClick = onClickOption4;
                break;
            case 4:
                buttonText = "Option 5";
                onClick = onClickOption5;
                break;
        }
        createButton(ctx, &optionButtons[i].button, ctx->window_width / 2 - 100, 100 + i * 60, 200, 50, buttonText, buttonStates[i] ? onClick : NULL);
        optionButtons[i].selectable = buttonStates[i];
    }
    showOptionButtons = true;
}

void GamePlaying(AppContext* ctx) {
    // 創建五個渲染區域，根據您的設計進行佈局
    int section_width = ctx->window_width;
    int section_height = ctx->window_height;

    // 區域佈局
    createRenderArea(ctx, 0, 0, section_width * 0.8, section_height * 0.8); // 區域 1
    createRenderArea(ctx, 0, section_height * 0.8, section_width * 0.2, section_height * 0.2); // 區域 2
    createRenderArea(ctx, section_width * 0.2, section_height * 0.8, section_width * 0.8, section_height * 0.2); // 區域 3
    createRenderArea(ctx, section_width * 0.8, 0, section_width * 0.2, section_height * 0.4); // 區域 4
    createRenderArea(ctx, section_width * 0.8, section_height * 0.4, section_width * 0.2, section_height * 0.2); // 區域 5

    // 設置背景
    setRenderAreaContent(ctx, 0, "example-game/assets/background/start.png", NULL, 0, NULL, NULL, NULL, 0, NULL, NULL);
    setRenderAreaContent(ctx, 1, "example-game/assets/background/beach.png", NULL, 0, NULL, NULL, NULL, 0, NULL, NULL);
    setRenderAreaContent(ctx, 2, "example-game/assets/background/black.png", NULL, 0, NULL, NULL, NULL, 0, NULL, NULL);
    setRenderAreaContent(ctx, 3, "example-game/assets/background/bedroom.png", NULL, 0, NULL, NULL, NULL, 0, NULL, NULL);
    setRenderAreaContent(ctx, 4, "example-game/assets/background/church.png", NULL, 0, NULL, NULL, NULL, 0, NULL, NULL);

    // 區域1: 設置背景和按鈕
    createButton(ctx, &settingButton, 10, 10, 100, 50, "Setting", onClickSetting);

    // 區域2: 設置圖片
    char* images[] = {"example-game/assets/character/lulu/tachie/angry.png"};
    SDL_Rect imageRect = {0, section_height * 0.6 + 10, section_width* 0.2, section_height * 0.4}; // 調整到左下區
    setRenderAreaContent(ctx, 1, NULL, images, 1, NULL, NULL, NULL, 0, NULL, &imageRect);

    // 區域3: 設置文字框
    SDL_Surface* textSurface = TTF_RenderUTF8_Blended(ctx->font, "text put here", white);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(ctx->renderer, textSurface);
    SDL_Rect textRect = {section_width * 0.2 + 10, section_height * 0.8 + 10, textSurface->w, textSurface->h};
    SDL_FreeSurface(textSurface);

    // 區域4: 設置物品欄
    char* itemTexts[] = {"item1", "item2", "item3", "item4", "item5"};
    SDL_Texture* itemTextures[5];
    SDL_Rect itemRects[5];
    for (int i = 0; i < 5; i++) {
        SDL_Surface* itemSurface = TTF_RenderUTF8_Blended(ctx->font, itemTexts[i], white);
        itemTextures[i] = SDL_CreateTextureFromSurface(ctx->renderer, itemSurface);
        itemRects[i] = (SDL_Rect){section_width * 0.8 + 10, 30 * i, itemSurface->w, itemSurface->h};
        SDL_FreeSurface(itemSurface);
    }

    // 區域4: 設置按鈕
    Button itemNextPageButton, itemPreviousPageButton;
    createButton(ctx, &itemPreviousPageButton, section_width * 0.8 + 10, section_height * 0.4 - 60, 50, 50, "<", onClickItemPreviousPage);
    createButton(ctx, &itemNextPageButton, section_width * 0.8 + 70, section_height * 0.4 - 60, 50, 50, ">", onClickItemNextPage);

    // 區域5: 設置狀態欄
    char* statusTexts[] = {"status1", "status2", "status3", "status4", "status5"};
    SDL_Texture* statusTextures[5];
    SDL_Rect statusRects[5];
    for (int i = 0; i < 5; i++) {
        SDL_Surface* statusSurface = TTF_RenderUTF8_Blended(ctx->font, statusTexts[i], white);
        statusTextures[i] = SDL_CreateTextureFromSurface(ctx->renderer, statusSurface);
        statusRects[i] = (SDL_Rect){section_width * 0.8 + 10, section_height * 0.4 + 30 * i, statusSurface->w, statusSurface->h};
        SDL_FreeSurface(statusSurface);
    }

    Button statusNextPageButton, statusPreviousPageButton;
    createButton(ctx, &statusPreviousPageButton, section_width * 0.8 + 10, section_height * 0.8 - 60, 50, 50, "<", onClickStatusPreviousPage);
    createButton(ctx, &statusNextPageButton, section_width * 0.8 + 70, section_height * 0.8 - 60, 50, 50, ">", onClickStatusNextPage);

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
                if (isButtonClicked(&settingButton, x, y)) {
                    settingButton.onClick(ctx);
                    quit = true;
                } else if (isButtonClicked(&itemNextPageButton, x, y)) {
                    itemNextPageButton.onClick(ctx);
                } else if (isButtonClicked(&itemPreviousPageButton, x, y)) {
                    itemPreviousPageButton.onClick(ctx);
                } else if (isButtonClicked(&statusNextPageButton, x, y)) {
                    statusNextPageButton.onClick(ctx);
                } else if (isButtonClicked(&statusPreviousPageButton, x, y)) {
                    statusPreviousPageButton.onClick(ctx);
                } else if (showOptionButtons) {
                    for (int i = 0; i < 4; i++) {
                        if (optionButtons[i].selectable && isButtonClicked(&optionButtons[i].button, x, y)) {
                            optionButtons[i].button.onClick(ctx);
                        }
                    }
                }
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN) {
                bool buttonStates[4] = {true, true, false, true}; // 設定按鈕狀態
                handleSignal(ctx, 4, buttonStates); // 顯示4個按鈕
            }
        }

        SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 255);
        SDL_RenderClear(ctx->renderer);

        // 渲染各個區域，確保背景在最下面
        renderBackground(ctx, &ctx->render_areas[0]);
        renderBackground(ctx, &ctx->render_areas[1]);
        renderBackground(ctx, &ctx->render_areas[2]);
        renderBackground(ctx, &ctx->render_areas[3]);
        renderBackground(ctx, &ctx->render_areas[4]);

        renderImage(ctx, &ctx->render_areas[1]); // 區域2的圖片
        renderButton(ctx, &itemNextPageButton); // 區域4的按鈕
        renderButton(ctx, &itemPreviousPageButton);
        renderButton(ctx, &statusNextPageButton); // 區域5的按鈕
        renderButton(ctx, &statusPreviousPageButton);
        renderButton(ctx, &settingButton); // 區域1的按鈕
        SDL_RenderCopy(ctx->renderer, textTexture, NULL, &textRect); // 渲染區域3的文字
        for (int i = 0; i < 5; i++) {
            SDL_RenderCopy(ctx->renderer, itemTextures[i], NULL, &itemRects[i]); // 渲染區域4的文字
            SDL_RenderCopy(ctx->renderer, statusTextures[i], NULL, &statusRects[i]); // 渲染區域5的文字
        }

        // 如果顯示選項按鈕
        if (showOptionButtons) {
            for (int i = 0; i < 4; i++) {
                if (optionButtons[i].selectable) {
                    renderButton(ctx, &optionButtons[i].button);
                } else {
                    // 不可選按鈕，文字顯示為灰色
                    SDL_Surface* buttonSurface = TTF_RenderUTF8_Blended(ctx->font, optionButtons[i].button.text, (SDL_Color){128, 128, 128, 255});
                    SDL_Texture* buttonTexture = SDL_CreateTextureFromSurface(ctx->renderer, buttonSurface);
                    SDL_RenderCopy(ctx->renderer, buttonTexture, NULL, &optionButtons[i].button.rect);
                    SDL_FreeSurface(buttonSurface);
                    SDL_DestroyTexture(buttonTexture);
                }
            }
        }

        SDL_RenderPresent(ctx->renderer);
    }

    // 釋放資源
    SDL_DestroyTexture(textTexture);
    for (int i = 0; i < 5; i++) {
        SDL_DestroyTexture(itemTextures[i]);
        SDL_DestroyTexture(statusTextures[i]);
    }
}
