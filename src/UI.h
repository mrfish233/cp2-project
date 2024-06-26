#ifndef UI_H
#define UI_H

#include "SDL.h"
#include "script.h"
#include "play_script.h"
#include "save_and_load.h"

typedef enum _GamePlayArea {
    AREA_BACKGROUND = 0,
    AREA_GAME_BG,
    AREA_TACHIE,
    AREA_TEXT,
    AREA_ITEM,
    AREA_STATUS
} GamePlayArea;

int startEngine(char *dir);

// 介面組
void GamePlaying(AppContext* ctx);
void End();
void Load(AppContext* ctx);
void MainMenu(AppContext* ctx);
void Credit();
void BackToMainMenu(AppContext* ctx);
void Settings(AppContext* ctx);
void BackToPreviousMenu(AppContext* ctx);
//Button組

void onClickResume(AppContext* ctx);
void onClickSave(AppContext* ctx) ;
void onClickMainMenu(AppContext* ctx);
void onClickSetting(AppContext* ctx);

void initMenuButtons(AppContext* ctx, Button* buttons);

//主介面

void renderTitle(AppContext* ctx);
void MainMenu(AppContext* ctx);

#endif
