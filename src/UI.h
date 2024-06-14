#ifndef UI_H
#define UI_H

#include "SDL.h"
#include "script.h"
#include "play_script.h"
#include "save_and_load.h"

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

void onClickNewGame(AppContext* ctx) ;
void onClickResume(AppContext* ctx);
void onClickSave(AppContext* ctx) ;
void onClickMainMenu(AppContext* ctx);
void onClickLoad(AppContext* ctx);
void onClickCredit(AppContext* ctx);
void onClickExit(AppContext* ctx);
void onClickSetting(AppContext* ctx);

void initMenuButtons(AppContext* ctx, Button* buttons);

void onClickSaveSlot1(AppContext* ctx);
void onClickLoadSlot1(AppContext* ctx);

void onClickSaveSlot2(AppContext* ctx);
void onClickLoadSlot2(AppContext* ctx);

void onClickLoadSlot3(AppContext* ctx);
void onClickSaveSlot3(AppContext* ctx);

void onClickSaveAuto(AppContext* ctx);
void onClickLoadAuto(AppContext* ctx);

void onClickDoNothing(AppContext* ctx);

//主介面

void renderTitle(AppContext* ctx);
void MainMenu(AppContext* ctx);

#endif
