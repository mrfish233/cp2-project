#include "UI.h"

static bool newGameFlag     = false;
static bool EndFlag         = false;
static bool GamePlayingFlag = false;
static bool LoadFlag        = false;
static bool CreditFlag      = false;

static const SDL_Color g_white = {255, 255, 255, 255};
static const SDL_Color g_black = {0, 0, 0, 255};
static const SDL_Color g_grey  = {128, 128, 128, 255};

static Script  g_script  = {0};
static Display g_display = {0};

int startEngine(char *dir) {
    AppContext ctx = { NULL, NULL, NULL, 1600, 900, NULL, 0, 0, 0 };
    // AppContext ctx = { NULL, NULL, NULL, 1024, 576, NULL, 0, 0, 0 };
    initSDL(&ctx);

    if (initGame(&g_script, &g_display, dir) != 0) {
        cleanUp(&ctx);
        clearScript(&g_script);
        return 1;
    }

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
    clearScript(&g_script);
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

void onClickMainMenu(AppContext* ctx) {
    printf("Button 'Main Menu' clicked\n");
    LoadFlag = false;  // 取消 Load 狀態
    GamePlayingFlag = false; // 取消 GamePlaying 狀態
    CreditFlag = false; // 取消 Credit 狀態
}

void Settings(AppContext* ctx) {
    Button buttons[3] = {0};

    char buttonTexts[3][STR_SIZE] = {"Resume", "Save", "Main Menu"};
    void (*buttonCallbacks[3])(AppContext*) = {onClickResume, onClickSave, onClickMainMenu};

    for (int i = 0; i < 3; i++) {
        buttons[i].rect    = (SDL_Rect) {ctx->window_width / 2 - 100, ctx->window_height / 2 - 75 + 75 * i, 200, 50};
        buttons[i].onClick = buttonCallbacks[i];
        strncpy(buttons[i].text, buttonTexts[i], STR_SIZE);

        createButton(ctx, &buttons[i]);
    }

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

void onClickCredit(AppContext* ctx) {
    printf("Button 'Credit' clicked\n");
    CreditFlag = true;
}

void onClickExit(AppContext* ctx) {
    printf("Button 'Exit' clicked\n");
    EndFlag = true;
}

void initMenuButtons(AppContext* ctx, Button* buttons) {
    char buttonTexts[4][STR_SIZE] = {"New Game", "Load", "Credit", "Exit"};
    void (*buttonCallbacks[4])(AppContext*) = {onClickNewGame, onClickLoad, onClickCredit, onClickExit};

    for (int i = 0; i < 4; i++) {
        buttons[i].rect    = (SDL_Rect) {ctx->window_width / 2 - 100, ctx->window_height / 2 - 100 + 75 * i, 200, 50};
        buttons[i].onClick = buttonCallbacks[i];
        strncpy(buttons[i].text, buttonTexts[i], STR_SIZE);

        createButton(ctx, &buttons[i]);
    }
}

void renderTitle(AppContext* ctx) {
    SDL_Surface* surface = TTF_RenderUTF8_Blended(ctx->font, "失落的旋律", g_white);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(ctx->renderer, surface);
    SDL_Rect dstrect = {ctx->window_width / 2 - surface->w / 2, 50, surface->w, surface->h};
    SDL_RenderCopy(ctx->renderer, texture, NULL, &dstrect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void MainMenu(AppContext* ctx) {
    Button buttons[4] = {0};
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

void onClickDoNothing(AppContext* ctx) {
    printf("Button 'Do Nothing' clicked\n");
    // 不執行任何操作
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

bool fromMainMenuFlag = false;
bool fromSettingFlag  = false;

Button settingButton = {0};

void onClickLoad(AppContext* ctx) {
    printf("Button 'Load' clicked\n");
    LoadFlag = true;  // 設置 Load 狀態
    fromMainMenuFlag = true; // 標記從主菜單進入 Load 界面
}

void onClickSave(AppContext* ctx) {
    printf("Button 'Save' clicked\n");
    LoadFlag = true;  // 設置 Load 狀態
    GamePlayingFlag = false; // 取消 GamePlaying 狀態
    fromSettingFlag = true; // 標記從設定進入 Load 界面
}

void BackToPreviousMenu(AppContext* ctx) {
    printf("Button 'Back' clicked\n");
    LoadFlag = false; // 取消 Load 狀態
    if (fromSettingFlag) {
        fromSettingFlag = false; // 重置標誌
        Settings(ctx); // 返回設定界面
    } else if (fromMainMenuFlag) {
        fromMainMenuFlag = false; // 重置標誌
        MainMenu(ctx); // 返回主菜單界面
    } else {
        GamePlayingFlag = true; // 返回 GamePlaying 狀態
    }
}

void Load(AppContext* ctx) {
    // 創建四個渲染區域，每個區域高度為視窗高度的四分之一
    int section_height = ctx->window_height / 4;
    createRenderArea(ctx, 0, 0, ctx->window_width, section_height); // 第一部分
    createRenderArea(ctx, 0, section_height, ctx->window_width, section_height); // 第二部分
    createRenderArea(ctx, 0, 2 * section_height, ctx->window_width, section_height); // 第三部分
    createRenderArea(ctx, 0, 3 * section_height, ctx->window_width, section_height); // 第四部分

    // 創建返回按鈕，回調函數設置為 BackToPreviousMenu
    Button backButton = {
        .rect      = {ctx->window_width - 110, 10, 100, 50},
        .textColor = g_black,
        .onClick   = BackToPreviousMenu
    };

    strncpy(backButton.text, "Back", STR_SIZE);

    createButton(ctx, &backButton);

    // 設置按鈕和文本框
    Button saveButtons[SAVE_SIZE] = {0};
    Button loadButtons[SAVE_SIZE] = {0};
    SDL_Rect textRects[SAVE_SIZE];

    char saveButtonText[SAVE_SIZE][STR_SIZE] = {"Save 1", "Save 2", "Save 3", "Save Auto"};
    char loadButtonText[SAVE_SIZE][STR_SIZE] = {"Load 1", "Load 2", "Load 3", "Load Auto"};

    void (*saveButtonCallbacks[SAVE_SIZE])(AppContext*) = {
        onClickSaveSlot1, onClickSaveSlot2, onClickSaveSlot3, onClickSaveAuto
    };
    void (*loadButtonCallbacks[SAVE_SIZE])(AppContext*) = {
        onClickLoadSlot1, onClickLoadSlot2, onClickLoadSlot3, onClickLoadAuto
    };

    char texts[SAVE_SIZE][STR_SIZE] = {0};
    char saveEvent[SAVE_SIZE][STR_SIZE] = {0};
    char saveDatetime[SAVE_SIZE][STR_SIZE] = {0};
    int hasSaveFile[SAVE_SIZE] = {0};

    if (findSaveEvent(&g_script, saveEvent, saveDatetime, hasSaveFile)) {
        printf("Failed to find save event\n");
        return;
    }

    for (int i = 0; i < SAVE_SIZE; i++) {
        saveButtons[i].rect = (SDL_Rect) {10,  i * section_height + section_height / 2 - 25, 200, 50};
        loadButtons[i].rect = (SDL_Rect) {220, i * section_height + section_height / 2 - 25, 200, 50};

        strncpy(saveButtons[i].text, saveButtonText[i], STR_SIZE);
        strncpy(loadButtons[i].text, loadButtonText[i], STR_SIZE);

        if (hasSaveFile[i]) {
            strncat(texts[i], saveEvent[i], STR_SIZE);
            strncat(texts[i], " ", 2);
            strncat(texts[i], saveDatetime[i], STR_SIZE);

            loadButtons[i].textColor = g_black;
            loadButtons[i].onClick   = loadButtonCallbacks[i];
        } else {
            strncat(texts[i], "Empty", 7);

            loadButtons[i].textColor = g_grey;
            loadButtons[i].onClick   = onClickDoNothing;
        }

        if (fromMainMenuFlag) {
            saveButtons[i].textColor = g_grey;
            saveButtons[i].onClick   = onClickDoNothing;
        } else {
            saveButtons[i].textColor = g_black;
            saveButtons[i].onClick   = saveButtonCallbacks[i];
        }

        createButton(ctx, &saveButtons[i]);
        createButton(ctx, &loadButtons[i]);

        textRects[i] = (SDL_Rect) {430, i * section_height + section_height / 2 - 25, 15 * strlen(texts[i]), 40};
    }

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
            SDL_Surface* surface = TTF_RenderUTF8_Blended(ctx->font, texts[i], g_white);
            SDL_Texture* texture = SDL_CreateTextureFromSurface(ctx->renderer, surface);
            SDL_RenderCopy(ctx->renderer, texture, NULL, &textRects[i]);
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
        }

        SDL_RenderPresent(ctx->renderer);
    }
}

void renderMessages(AppContext* ctx, char** messages, int messageCount) {
    SDL_Color white = {255, 255, 255, 255};
    int textHeight = 0;

    for (int i = 0; i < messageCount; i++) {
        SDL_Surface* messageSurface = TTF_RenderUTF8_Blended(ctx->font, messages[i], white);
        SDL_Texture* messageTexture = SDL_CreateTextureFromSurface(ctx->renderer, messageSurface);

        int textWidth = messageSurface->w;
        textHeight = messageSurface->h;
        SDL_FreeSurface(messageSurface);

        SDL_Rect messageRect = {
            (ctx->window_width - textWidth) / 5*2,
            (ctx->window_height - textHeight) / 5 + (i * (textHeight + 10)),
            textWidth,
            textHeight
        };

        SDL_RenderCopy(ctx->renderer, messageTexture, NULL, &messageRect);
        SDL_DestroyTexture(messageTexture);
    }
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

    enum {
        AREA_BACKGROUND = 0,
        AREA_GAME_BG,
        AREA_TACHIE,
        AREA_TEXT,
        AREA_ITEM,
        AREA_STATUS
    };

    // 設置背景
    setRenderAreaContent(ctx, AREA_TEXT,   "resources/images/whiteline3.png", NULL, 0, NULL, NULL, NULL, 0, NULL, NULL);
    setRenderAreaContent(ctx, AREA_ITEM,   "resources/images/black.png",      NULL, 0, NULL, NULL, NULL, 0, NULL, NULL);
    setRenderAreaContent(ctx, AREA_STATUS, "resources/images/black.png",      NULL, 0, NULL, NULL, NULL, 0, NULL, NULL);

    // 區域1: 設置背景和按鈕
    settingButton.rect = (SDL_Rect) {10, 10, 100, 50};
    settingButton.onClick = onClickSetting;
    strncpy(settingButton.text, "Setting", STR_SIZE);

    createButton(ctx, &settingButton);

    // 區域2: 設置圖片
    char* images[] = {"example-game/assets/character/lulu/tachie/angry.png"};
    SDL_Rect imageRect = {0, section_height * 0.6 + 10, section_width * 0.2, section_height * 0.4}; // 調整到左下區
    setRenderAreaContent(ctx, 1, NULL, images, 1, NULL, NULL, NULL, 0, NULL, &imageRect);

    // 區域3: 設置文字框

    SDL_Surface* textSurface = NULL;
    SDL_Texture* textTexture = NULL;
    SDL_Rect textRect = {0};

    SDL_Surface* nameSurface = NULL;
    SDL_Texture* nameTexture = NULL;
    SDL_Rect nameRect = {0};

    Button optionButtons[5] = {0};

    bool showOptionButtons = false;

    // 區域4: 設置物品欄
    char* itemTexts[] = {"item1", "item2", "item3", "item4", "item5"};
    SDL_Texture* itemTextures[5];
    SDL_Rect itemRects[5];
    for (int i = 0; i < 5; i++) {
        SDL_Surface* itemSurface = TTF_RenderUTF8_Blended(ctx->font, itemTexts[i], g_white);
        itemTextures[i] = SDL_CreateTextureFromSurface(ctx->renderer, itemSurface);
        itemRects[i] = (SDL_Rect){section_width * 0.8 + 10, 30 * i, itemSurface->w, itemSurface->h};
        SDL_FreeSurface(itemSurface);
    }

    // 區域4: 設置按鈕
    Button itemNextPageButton     = {
        .rect    = {section_width * 0.8 + 70, section_height * 0.4 - 60, 50, 50},
        .onClick = onClickItemNextPage
    };
    Button itemPreviousPageButton = {
        .rect    = {section_width * 0.8 + 10, section_height * 0.4 - 60, 50, 50},
        .onClick = onClickItemPreviousPage
    };

    strncpy(itemNextPageButton.text, ">", STR_SIZE);
    strncpy(itemPreviousPageButton.text, "<", STR_SIZE);

    createButton(ctx, &itemPreviousPageButton);
    createButton(ctx, &itemNextPageButton);

    // 區域5: 設置狀態欄
    char* statusTexts[] = {"status1", "status2", "status3", "status4", "status5"};
    SDL_Texture* statusTextures[5];
    SDL_Rect statusRects[5];
    for (int i = 0; i < 5; i++) {
        SDL_Surface* statusSurface = TTF_RenderUTF8_Blended(ctx->font, statusTexts[i], g_white);
        statusTextures[i] = SDL_CreateTextureFromSurface(ctx->renderer, statusSurface);
        statusRects[i] = (SDL_Rect){section_width * 0.8 + 10, section_height * 0.4 + 30 * i, statusSurface->w, statusSurface->h};
        SDL_FreeSurface(statusSurface);
    }

    Button statusNextPageButton = {
        .rect    = {section_width * 0.8 + 70, section_height * 0.8 - 60, 50, 50},
        .onClick = onClickStatusNextPage
    };
    Button statusPreviousPageButton = {
        .rect    = {section_width * 0.8 + 10, section_height * 0.8 - 60, 50, 50},
        .onClick = onClickStatusPreviousPage
    };

    strncpy(statusNextPageButton.text, ">", STR_SIZE);
    strncpy(statusPreviousPageButton.text, "<", STR_SIZE);

    createButton(ctx, &statusPreviousPageButton);
    createButton(ctx, &statusNextPageButton);

    // 新增變數
    bool showMessageFlag = false;
    Uint32 messageStartTime = 0;
    Uint32 messageDuration = 2000; // 2秒
    char* messages[5] = {"獲得 橘子", "獲得 蘋果", "獲得 草莓", "獲得 檸檬", "獲得 西瓜"};
    int messageCount = 0;

    loadTextures(ctx);

    // Current event, used to check if the event has been changed
    char currentEventID[STR_SIZE] = {0};

    // 渲染循環
    bool quit   = false;
    bool update = true;
    SDL_Event e;
    while (!quit) {
        if (update) {
            if (updateDialogue(&g_script, &g_display)) {
                printf("Failed to update dialogue\n");
                quit = true;
                EndFlag = true;
                break;
            }

            // Check if the event has been changed

            if (strcmp(currentEventID, g_script.current_event_id) != 0) {
                if (saveScript(&g_script, SAVE_SLOT_AUTO) != 0) {
                    printf("Failed to save script\n");
                    quit = true;
                    EndFlag = true;
                    break;
                }
            }

            strncpy(currentEventID, g_script.current_event_id, STR_SIZE);

            // Update display background and character

            if (strlen(g_display.path_background) > 0) {
                setRenderAreaContent(ctx, AREA_GAME_BG, g_display.path_background, NULL, 0, NULL, NULL, NULL, 0, NULL, NULL);
            }
            else {
                setRenderAreaContent(ctx, AREA_GAME_BG, "resources/images/black.png", NULL, 0, NULL, NULL, NULL, 0, NULL, NULL);
            }

            if (strlen(g_display.path_tachie) > 0) {
                char *images[1];
                images[0] = (char*) malloc(STR_SIZE);
                strncpy(images[0], g_display.path_tachie, STR_SIZE);

                SDL_Rect imageRect = {0, section_height * 0.6 + 10, section_width * 0.2, section_height * 0.4};

                setRenderAreaContent(ctx, AREA_GAME_BG, NULL, images, 1, NULL, NULL, NULL, 0, NULL, &imageRect);

                free(images[0]);
            }
            else {
                char *images[] = {"resources/images/black.png"};
                setRenderAreaContent(ctx, AREA_GAME_BG, NULL, images, 1, NULL, NULL, NULL, 0, NULL, NULL);
            }

            // Name

            if (strlen(g_display.character) > 0) {
                nameSurface = TTF_RenderUTF8_Blended(ctx->font, g_display.character, g_white);
                nameTexture = SDL_CreateTextureFromSurface(ctx->renderer, nameSurface);
                nameRect = (SDL_Rect) {section_width * 0.2 + 10, section_height * 0.8 - 40, nameSurface->w, nameSurface->h};
                SDL_FreeSurface(nameSurface);
            }
            else {
                SDL_DestroyTexture(nameTexture);
            }

            // Dialogues

            textSurface = TTF_RenderUTF8_Blended(ctx->font, g_display.dialogue, g_white);
            textTexture = SDL_CreateTextureFromSurface(ctx->renderer, textSurface);
            textRect = (SDL_Rect) {section_width * 0.2 + 10, section_height * 0.8 + 10, textSurface->w, textSurface->h};
            SDL_FreeSurface(textSurface);

            // Options

            if (g_display.option_flag) {
                showOptionButtons = true;
            }

            update = false;
        }

        SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 255);
        SDL_RenderClear(ctx->renderer);

        // 渲染各個區域，確保背景在最下面
        for (int32_t i = 1; i <= 5; i++) {
            renderBackground(ctx, &ctx->render_areas[i]);
        }

        renderImage(ctx, &ctx->render_areas[1]); // 區域2的圖片
        renderButton(ctx, &itemNextPageButton); // 區域4的按鈕
        renderButton(ctx, &itemPreviousPageButton);
        renderButton(ctx, &statusNextPageButton); // 區域5的按鈕
        renderButton(ctx, &statusPreviousPageButton);
        renderButton(ctx, &settingButton); // 區域1的按鈕

        SDL_RenderCopy(ctx->renderer, textTexture, NULL, &textRect); // 渲染區域3的文字
        SDL_RenderCopy(ctx->renderer, nameTexture, NULL, &nameRect); // 渲染區域3的名字

        for (int i = 0; i < 5; i++) {
            SDL_RenderCopy(ctx->renderer, itemTextures[i], NULL, &itemRects[i]); // 渲染區域4的文字
            SDL_RenderCopy(ctx->renderer, statusTextures[i], NULL, &statusRects[i]); // 渲染區域5的文字
        }

        // 如果顯示選項按鈕
        if (showOptionButtons) {
            for (int i = 0; i < g_display.option_size; i++) {
                optionButtons[i].rect.x = ctx->window_width  / 4;
                optionButtons[i].rect.y = ctx->window_height / 4 + i * 60;
                optionButtons[i].rect.w = ctx->window_width  / 2;
                optionButtons[i].rect.h = 50;

                strncpy(optionButtons[i].text, g_display.options[i], STR_SIZE);

                if (g_display.options_selectable[i]) {
                    optionButtons[i].textColor = g_black;
                } else {
                    optionButtons[i].textColor = g_grey;
                }

                optionButtons[i].onClick = NULL;

                renderButton(ctx, &optionButtons[i]);
            }
        }

        // 如果顯示消息
        if (showMessageFlag) {
            if (SDL_GetTicks() - messageStartTime < messageDuration) {
                renderMessages(ctx, messages, messageCount);
            } else {
                showMessageFlag = false; // 超過顯示時長後隱藏消息
            }
        }

        SDL_RenderPresent(ctx->renderer);

        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
                EndFlag = true;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x = e.button.x;
                int y = e.button.y;
                if (isButtonClicked(&settingButton, x, y)) {
                    settingButton.onClick(ctx);
                    quit = true;
                }
                else if (isButtonClicked(&itemNextPageButton, x, y)) {
                    itemNextPageButton.onClick(ctx);
                }
                else if (isButtonClicked(&itemPreviousPageButton, x, y)) {
                    itemPreviousPageButton.onClick(ctx);
                }
                else if (isButtonClicked(&statusNextPageButton, x, y)) {
                    statusNextPageButton.onClick(ctx);
                }
                else if (isButtonClicked(&statusPreviousPageButton, x, y)) {
                    statusPreviousPageButton.onClick(ctx);
                }
                else if (showOptionButtons) {
                    for (int i = 0; i < g_display.option_size; i++) {
                        if (g_display.options_selectable[i] && isButtonClicked(&optionButtons[i], x, y)) {
                            g_display.option_select = i + 1;
                            showOptionButtons = false;
                            update = true;
                            break;
                        }
                    }
                }
                else {
                    // Continue dialogue
                    update = true;
                }
            }
            else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_LSHIFT) {
                showMessageFlag = true;
                messageStartTime = SDL_GetTicks(); // 記錄消息顯示的開始時間
                messageCount = 5; // 顯示5個消息
            }
        }
    }

    // 釋放資源
    SDL_DestroyTexture(textTexture);
    SDL_DestroyTexture(nameTexture);

    for (int i = 0; i < 5; i++) {
        SDL_DestroyTexture(itemTextures[i]);
        SDL_DestroyTexture(statusTextures[i]);
    }
}
