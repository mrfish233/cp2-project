#include "UI.h"

static bool newGameFlag     = false;
static bool EndFlag         = false;
static bool GamePlayingFlag = false;
static bool LoadFlag        = false;
static bool CreditFlag      = false;

<<<<<<< Updated upstream
static bool fromMainMenuFlag = false;
static bool fromSettingFlag  = false;

static const SDL_Color g_white = {255, 255, 255, 255};
static const SDL_Color g_black = {0, 0, 0, 255};
static const SDL_Color g_grey  = {128, 128, 128, 255};
=======
// 全域flag
bool newGameFlag = false;
bool EndFlag = false;
bool GamePlayingFlag = false;
bool LoadFlag = false;
bool CreditFlag = false;
bool fromMainMenuFlag = false;
bool fromSettingFlag = false;
SDL_Color white = {255, 255, 255, 255}; // 定義全域變數
SDL_Color black = {0, 0, 0, 255};
>>>>>>> Stashed changes

static Script  g_script  = {0};
static Display g_display = {0};

Mix_Chunk *popSfx  = NULL;
Mix_Chunk *pageSfx = NULL;

int startEngine(char *dir) {
    AppContext ctx = { NULL, NULL, NULL, 1600, 900, NULL, 0, 0, 0 };
    // AppContext ctx = { NULL, NULL, NULL, 1024, 576, NULL, 0, 0, 0 };
    initSDL(&ctx);

    initAudio();
    loadSound("resources/sounds/button.mp3", &popSfx);
    loadSound("resources/sounds/page.mp3", &pageSfx);

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

void onClickResume(AppContext* ctx) {
    printf("Button 'Resume' clicked\n");
    LoadFlag = false;  // 取消 Load 狀態
    GamePlayingFlag = true; // 設置 GamePlaying 狀態

<<<<<<< Updated upstream
    if (initDisplay(&g_display) != 0) {
        printf("Failed to reset display\n");
        EndFlag = true;
    }

    g_display.continue_flag = 1;
}
=======
>>>>>>> Stashed changes

void onClickMainMenu(AppContext* ctx) {
    printf("Button 'Main Menu' clicked\n");
    LoadFlag = false;  // 取消 Load 狀態
    GamePlayingFlag = false; // 取消 GamePlaying 狀態
    CreditFlag = false; // 取消 Credit 狀態
}

void Settings(AppContext* ctx) {
    Button buttons[3] = {0};

    char buttonTexts[3][STR_SIZE] = {"Resume", "Save / Load", "Main Menu"};
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
                        playSound(popSfx);

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

void initMenuButtons(AppContext* ctx, Button* buttons) {
    char buttonTexts[4][STR_SIZE] = {"New Game", "Load", "Credit", "Exit"};

    for (int i = 0; i < 4; i++) {
        buttons[i].rect    = (SDL_Rect) {ctx->window_width / 2 - 100, ctx->window_height / 2 - 100 + 75 * i, 200, 50};
        buttons[i].onClick = NULL;
        strncpy(buttons[i].text, buttonTexts[i], STR_SIZE);

<<<<<<< Updated upstream
        createButton(ctx, &buttons[i]);
    }
}

void renderTitle(AppContext* ctx) {
    TTF_Font *title_font = TTF_OpenFont("resources/font/Noto_Sans_TC/static/NotoSansTC-Black.ttf", 96);
=======
void onClickCredit(AppContext* ctx) {
    printf("Button 'Credit' clicked\n");
    CreditFlag = true;
}
>>>>>>> Stashed changes

    SDL_Surface* title_surface = TTF_RenderUTF8_Blended(title_font, g_script.name, g_white);
    SDL_Texture* title_texture = SDL_CreateTextureFromSurface(ctx->renderer, title_surface);
    SDL_Rect title_rect = {ctx->window_width / 2 - title_surface->w / 2, 100, title_surface->w, title_surface->h};
    SDL_Rect title_bg_rect = {ctx->window_width / 2 - title_surface->w / 2 - 10, 100, title_surface->w + 20, title_surface->h + 10};

    SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 120);
    SDL_RenderFillRect(ctx->renderer, &title_bg_rect);

    SDL_RenderCopy(ctx->renderer, title_texture, NULL, &title_rect);
    SDL_FreeSurface(title_surface);
    SDL_DestroyTexture(title_texture);

    TTF_CloseFont(title_font);
}

void MainMenu(AppContext* ctx) {
    Button buttons[4] = {0};
    initMenuButtons(ctx, buttons);

    enum {
        BUTTON_NEW_GAME = 0,
        BUTTON_LOAD,
        BUTTON_CREDIT,
        BUTTON_EXIT
    };

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

                if (isButtonClicked(&buttons[BUTTON_NEW_GAME], x, y)) {
                    playSound(popSfx);

                    if (resetGame(&g_script, &g_display) != 0) {
                        printf("Failed to reset game\n");
                        EndFlag = true;
                        break;
                    }

                    newGameFlag = true;
                    GamePlayingFlag = true;
                    quit = true;
                } else if (isButtonClicked(&buttons[BUTTON_LOAD], x, y)) {
                    playSound(popSfx);

                    LoadFlag = true;
                    fromMainMenuFlag = true;
                    quit = true;
                } else if (isButtonClicked(&buttons[BUTTON_CREDIT], x, y)) {
                    playSound(popSfx);

                    CreditFlag = true;
                    quit = true;
                } else if (isButtonClicked(&buttons[BUTTON_EXIT], x, y)) {
                    playSound(popSfx);

                    EndFlag = true;
                    quit = true;
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

<<<<<<< Updated upstream
void BackToMainMenu(AppContext* ctx) {
    printf("Button 'Back' clicked\n");
    LoadFlag = false;  // 返回主菜單，將LoadFlag設置為false
}
=======
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





>>>>>>> Stashed changes

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

<<<<<<< Updated upstream
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

    char saveButtonText[SAVE_SIZE][STR_SIZE] = {"Save 1", "Save 2", "Save 3", "Save Auto"};
    char loadButtonText[SAVE_SIZE][STR_SIZE] = {"Load 1", "Load 2", "Load 3", "Load Auto"};

    char texts[SAVE_SIZE][STR_SIZE] = {0};
    char saveEvent[SAVE_SIZE][STR_SIZE] = {0};
    char saveDatetime[SAVE_SIZE][STR_SIZE] = {0};
    int hasSaveFile[SAVE_SIZE] = {0};

    if (findSaveEvent(&g_script, saveEvent, saveDatetime, hasSaveFile)) {
        printf("Failed to find save event\n");
        return;
    }

    SDL_Surface* text_surface = NULL;
    SDL_Texture* text_textures[SAVE_SIZE] = {0};
    SDL_Rect text_rects[SAVE_SIZE] = {0};

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
        } else {
            strncat(texts[i], "Empty", 7);

            loadButtons[i].textColor = g_grey;
        }

        if (fromMainMenuFlag) {
            saveButtons[i].textColor = g_grey;
        } else {
            saveButtons[i].textColor = g_black;
        }

        saveButtons[i].onClick = NULL;
        loadButtons[i].onClick = NULL;

        createButton(ctx, &saveButtons[i]);
        createButton(ctx, &loadButtons[i]);

        text_surface     = TTF_RenderUTF8_Blended(ctx->font, texts[i], g_white);
        text_textures[i] = SDL_CreateTextureFromSurface(ctx->renderer, text_surface);
        text_rects[i]    = (SDL_Rect) {
            430, i * section_height + section_height / 2 - 25, text_surface->w, text_surface->h
        };
        SDL_FreeSurface(text_surface);
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
                    playSound(popSfx);

                    backButton.onClick(ctx);
                    quit = true;
                } else {
                    for (int i = 0; i < 4; i++) {
                        if (isButtonClicked(&saveButtons[i], x, y)) {
                            if (fromMainMenuFlag) {
                                continue;
                            }

                            playSound(popSfx);

                            if (saveScript(&g_script, i) != 0) {
                                printf("Failed to save script\n");
                                quit = true;
                                EndFlag = true;
                                break;
                            }

                            memset(texts[i], 0, STR_SIZE);
                            strncat(texts[i], saveEvent[i], STR_SIZE);
                            strncat(texts[i], " ", 2);
                            strncat(texts[i], saveDatetime[i], STR_SIZE);

                            if (findSaveEvent(&g_script, saveEvent, saveDatetime, hasSaveFile)) {
                                printf("Failed to find save event\n");
                                quit = true;
                                break;
                            }

                            memset(texts[i], 0, STR_SIZE);
                            strncat(texts[i], saveEvent[i], STR_SIZE);
                            strncat(texts[i], " ", 2);
                            strncat(texts[i], saveDatetime[i], STR_SIZE);

                            text_surface = TTF_RenderUTF8_Blended(ctx->font, texts[i], g_white);
                            text_textures[i] = SDL_CreateTextureFromSurface(ctx->renderer, text_surface);
                            text_rects[i] = (SDL_Rect) {430, i * section_height + section_height / 2 - 25, text_surface->w, text_surface->h};
                            SDL_FreeSurface(text_surface);
                        }
                        if (isButtonClicked(&loadButtons[i], x, y)) {
                            if (!hasSaveFile[i]) {
                                continue;
                            }

                            playSound(popSfx);

                            if (loadScript(&g_script, i) != 0 || initDisplay(&g_display)  != 0) {
                                printf("Failed to load script or reset display\n");
                                quit = true;
                                EndFlag = true;
                                break;
                            }

                            g_display.continue_flag = 1;

                            LoadFlag = false;
                            fromMainMenuFlag = false;
                            GamePlayingFlag = true;
                            quit = true;
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
            SDL_RenderCopy(ctx->renderer, text_textures[i], NULL, &text_rects[i]);
        }

        SDL_RenderPresent(ctx->renderer);
    }
}
=======
>>>>>>> Stashed changes

void renderMessages(AppContext* ctx, char** messages, int messageCount) {
    if (messageCount == 0) {
        return;
    }

    int32_t maxHeight = 0;
    int32_t maxWidth  = 0;

    SDL_Surface *messageSurface = NULL;
    SDL_Texture *messageTextures[5] = {0};

    SDL_Rect messageRects[5] = {0};

    for (int i = 0; i < messageCount; i++) {
        messageSurface = TTF_RenderUTF8_Blended(ctx->font, messages[i], g_white);
        messageTextures[i] = SDL_CreateTextureFromSurface(ctx->renderer, messageSurface);

        int32_t textWidth  = messageSurface->w;
        int32_t textHeight = messageSurface->h;
        SDL_FreeSurface(messageSurface);

        messageRects[i] = (SDL_Rect) {
            (ctx->window_width - textWidth) / 2,
            (ctx->window_height - textHeight) / 4 + (i * (textHeight + 10)),
            textWidth,
            textHeight
        };

        maxWidth  = textWidth  > maxWidth  ? textWidth : maxWidth;
        maxHeight = textHeight > maxHeight ? textHeight : maxHeight;
    }

    SDL_Rect messageBgRect = {
        (ctx->window_width  - maxWidth) / 2 - 10,
        (ctx->window_height - maxHeight) / 4 - 10,
        maxWidth + 20,
        (maxHeight + 5) * messageCount + 20
    };

    SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 120);
    SDL_RenderFillRect(ctx->renderer, &messageBgRect);

    for (int32_t i = 0; i < messageCount; i++) {
        SDL_RenderCopy(ctx->renderer, messageTextures[i], NULL, &messageRects[i]);
        SDL_DestroyTexture(messageTextures[i]);
    }
}


void GamePlaying(AppContext* ctx) {
    // 創建五個渲染區域，根據您的設計進行佈局
    int section_width = ctx->window_width;
    int section_height = ctx->window_height;

    clearAllRenderAreas(ctx);

    // 區域佈局
    createRenderArea(ctx, 0, 0, section_width, section_height); // 遊戲背景
    createRenderArea(ctx, 0, 0, section_width * 0.8, section_height * 0.8); // 區域 1
    createRenderArea(ctx, 0, section_height * 0.8, section_width * 0.2, section_height * 0.2); // 區域 2
    createRenderArea(ctx, section_width * 0.2, section_height * 0.8, section_width * 0.8, section_height * 0.2); // 區域 3
    createRenderArea(ctx, section_width * 0.8, 0, section_width * 0.2, section_height * 0.4); // 區域 4
    createRenderArea(ctx, section_width * 0.8, section_height * 0.4, section_width * 0.2, section_height * 0.2); // 區域 5

    // 設置背景
    setRenderAreaContent(ctx, AREA_TEXT,   "resources/images/whiteline3.png", NULL, 0, NULL, NULL, NULL, 0, NULL, NULL);
    setRenderAreaContent(ctx, AREA_ITEM,   "resources/images/black.png",      NULL, 0, NULL, NULL, NULL, 0, NULL, NULL);
    setRenderAreaContent(ctx, AREA_STATUS, "resources/images/black.png",      NULL, 0, NULL, NULL, NULL, 0, NULL, NULL);

    // 區域1: 設置背景和按鈕
    Button settingButton = {
        .rect    = {10, 10, 120, 50},
        .onClick = onClickSetting
    };

    strncpy(settingButton.text, "Settings", STR_SIZE);

    createButton(ctx, &settingButton);

    // 區域3: 設置文字框

    SDL_Surface* textSurface = NULL;
    SDL_Texture* textTexture = NULL;
    SDL_Rect textRect = {0};

    SDL_Surface* nameSurface = NULL;
    SDL_Texture* nameTexture = NULL;
    SDL_Rect nameRect = {0};

    // 區域3：設置選項按鈕
    Button optionButtons[5] = {0};

    // 區域4: 設置物品欄
    SDL_Texture* itemTextures[5] = {0};
    SDL_Rect itemRects[5] = {0};

    // 區域4: 設置按鈕
    Button itemNextPageButton     = {
        .rect    = {section_width * 0.8 + 90, section_height * 0.4 - 60, 40, 40},
        .onClick = NULL
    };
    Button itemPreviousPageButton = {
        .rect    = {section_width * 0.8 + 10, section_height * 0.4 - 60, 40, 40},
        .onClick = NULL
    };

    strncpy(itemNextPageButton.text, ">", STR_SIZE);
    strncpy(itemPreviousPageButton.text, "<", STR_SIZE);

    createButton(ctx, &itemPreviousPageButton);
    createButton(ctx, &itemNextPageButton);

    // 區域5: 設置狀態欄
    SDL_Texture* statusTextures[5] = {0};
    SDL_Rect statusRects[5] = {0};

    Button statusNextPageButton = {
        .rect    = {section_width * 0.8 + 90, section_height * 0.8 - 60, 40, 40},
        .onClick = NULL
    };
    Button statusPreviousPageButton = {
        .rect    = {section_width * 0.8 + 10, section_height * 0.8 - 60, 40, 40},
        .onClick = NULL
    };

    strncpy(statusNextPageButton.text, ">", STR_SIZE);
    strncpy(statusPreviousPageButton.text, "<", STR_SIZE);

    createButton(ctx, &statusPreviousPageButton);
    createButton(ctx, &statusNextPageButton);

    // 新增變數
    bool showMessageFlag    = false;
    Uint32 messageStartTime = 0;
    Uint32 messageDuration  = 2000;

    loadTextures(ctx);

    // Current event, used to check if the event has been changed
    char currentEventID[STR_SIZE] = {0};

    // 渲染循環
    bool quit   = false;
    bool update = true;
    SDL_Event e;

    while (!quit) {
        if (update) {
            if (updateScriptData(&g_script, &g_display)) {
                printf("Failed to update dialogue\n");
                quit = true;
                EndFlag = true;
                break;
            }

            // Play voice

            if (strlen(g_display.path_voice) > 0) {
                Mix_Chunk *voice_sfx = NULL;
                loadSound(g_display.path_voice, &voice_sfx);
                playSound(voice_sfx);
            }

            // Check if the event has been changed, if so, save the script

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
                }
            }

            // Updates

            if (g_display.update_flag) {
                showMessageFlag = true;
                messageStartTime = SDL_GetTicks();
            }

            // Items

            for (int i = 0; i < 5; i++) {
                if (i < g_display.inventory_size) {
                    SDL_Surface* itemSurface = TTF_RenderUTF8_Blended(ctx->font, g_display.inventory[i], g_white);
                    itemTextures[i] = SDL_CreateTextureFromSurface(ctx->renderer, itemSurface);
                    itemRects[i]    = (SDL_Rect) {
                        section_width * 0.8 + 10, 30 * i, itemSurface->w, itemSurface->h
                    };

                    SDL_FreeSurface(itemSurface);
                }
                else {
                    SDL_DestroyTexture(itemTextures[i]);
                }
            }

            // Status

            for (int i = 0; i < 5; i++) {
                char status_text[1024] = {0};

                if (i < g_display.status_size) {
                    snprintf(status_text, 1024, "%s: %d", g_display.status[i].status_name, g_display.status[i].value);
                    SDL_Surface* statusSurface = TTF_RenderUTF8_Blended(ctx->font, status_text, g_white);
                    statusTextures[i] = SDL_CreateTextureFromSurface(ctx->renderer, statusSurface);
                    statusRects[i] = (SDL_Rect) {
                        section_width * 0.8 + 10, section_height * 0.4 + 30 * i, statusSurface->w, statusSurface->h
                    };

                    SDL_FreeSurface(statusSurface);
                }
                else {
                    SDL_DestroyTexture(statusTextures[i]);
                }
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
        if (g_display.option_flag) {
            for (int i = 0; i < g_display.option_size; i++) {
                renderButton(ctx, &optionButtons[i]);
            }
        }

        // 如果顯示消息
        if (showMessageFlag) {
            if (SDL_GetTicks() - messageStartTime < messageDuration) {
                char **messages = (char**) malloc(sizeof(char*) * g_display.update_size);

                for (int i = 0; i < g_display.update_size; i++) {
                    messages[i] = (char*) malloc(STR_SIZE);
                    strncpy(messages[i], g_display.updates[i], STR_SIZE);
                }

                renderMessages(ctx, messages, g_display.update_size);

                for (int i = 0; i < g_display.update_size; i++) {
                    free(messages[i]);
                }

                free(messages);
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
                    playSound(popSfx);

                    settingButton.onClick(ctx);
                    quit = true;
                }
                else if (isButtonClicked(&itemNextPageButton, x, y) ||
                         isButtonClicked(&itemPreviousPageButton, x, y)) {
                    playSound(pageSfx);

                    if (isButtonClicked(&itemNextPageButton, x, y)) {
                        g_display.inventory_page++;
                    } else {
                        g_display.inventory_page--;
                    }

                    updateInventoryPage(&g_script, &g_display);

                    for (int i = 0; i < 5; i++) {
                        if (i < g_display.inventory_size) {
                            SDL_Surface* itemSurface = TTF_RenderUTF8_Blended(ctx->font, g_display.inventory[i], g_white);
                            itemTextures[i] = SDL_CreateTextureFromSurface(ctx->renderer, itemSurface);
                            itemRects[i]    = (SDL_Rect){section_width * 0.8 + 10, 30 * i, itemSurface->w, itemSurface->h};
                            SDL_FreeSurface(itemSurface);
                        }
                        else {
                            SDL_DestroyTexture(itemTextures[i]);
                        }
                    }
                }
                else if (isButtonClicked(&statusNextPageButton, x, y) ||
                         isButtonClicked(&statusPreviousPageButton, x, y)) {
                    playSound(pageSfx);

                    if (isButtonClicked(&statusNextPageButton, x, y)) {
                        g_display.status_page++;
                    } else {
                        g_display.status_page--;
                    }

                    updateStatusPage(&g_script, &g_display);

                    for (int i = 0; i < 5; i++) {
                        char status_text[1024] = {0};

                        if (i < g_display.status_size) {
                            snprintf(status_text, 1024, "%s: %d", g_display.status[i].status_name, g_display.status[i].value);
                            SDL_Surface* statusSurface = TTF_RenderUTF8_Blended(ctx->font, status_text, g_white);
                            statusTextures[i] = SDL_CreateTextureFromSurface(ctx->renderer, statusSurface);
                            statusRects[i] = (SDL_Rect) {
                                section_width * 0.8 + 10, section_height * 0.4 + 30 * i, statusSurface->w, statusSurface->h
                            };

                            SDL_FreeSurface(statusSurface);
                        }
                        else {
                            SDL_DestroyTexture(statusTextures[i]);
                        }
                    }
                }
                else if (g_display.option_flag) {
                    for (int i = 0; i < g_display.option_size; i++) {
                        if (g_display.options_selectable[i] && isButtonClicked(&optionButtons[i], x, y)) {
                            playSound(popSfx);

                            g_display.option_select = i + 1;
                            update = true;
                            break;
                        }
                    }
                }
                else {
                    // Continue dialogue if not end
                    if (g_display.end_flag) {
                        quit = true;
                        GamePlayingFlag = false;
                        break;
                    }
                    else {
                        update = true;
                    }
                }
            }
        }
    }

    // 釋放資源

    for (int i = 0; i < 5; i++) {
        SDL_DestroyTexture(itemTextures[i]);
        SDL_DestroyTexture(statusTextures[i]);
    }
}
<<<<<<< Updated upstream
=======

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
    // 創建返回按鈕，回調函數設置為 BackToPreviousMenu
    Button backButton;
    createButton(ctx, &backButton, ctx->window_width - 110, 10, 100, 50, "Back", BackToPreviousMenu);

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


>>>>>>> Stashed changes
