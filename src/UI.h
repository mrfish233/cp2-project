#ifndef UI_H
#define UI_H

#include "SDL.h"





int process();







// 介面組
void GamePlaying(AppContext* ctx);
void End();
void Load(AppContext* ctx);
void MainMenu(AppContext* ctx);
void Credit();
void BackToMainMenu(AppContext* ctx);
void Settings(AppContext* ctx);

//Button組
void createButton1(AppContext* ctx, Button* button, int x, int y, int w, int h, const char* text, void (*onClick)(AppContext* ctx)) ;


void onClickNewGame(AppContext* ctx) ;
void onClickResume(AppContext* ctx);
void onClickSave(AppContext* ctx) ;
void onClickMainMenu(AppContext* ctx);
void onClickLoad(AppContext* ctx);
void onClickCredit(AppContext* ctx);
void onClickExit(AppContext* ctx);
void onClickSetting(AppContext* ctx);
void initMenuButtons(AppContext* ctx, Button* buttons);
void initMenuButtons(AppContext* ctx, Button* buttons);
void onClickSaveSlot1(AppContext* ctx);
void onClickLoadSlot1(AppContext* ctx) ;
void onClickSaveSlot2(AppContext* ctx);
void onClickLoadSlot2(AppContext* ctx);
void onClickSaveAuto(AppContext* ctx);
void onClickLoadAuto(AppContext* ctx);
void onClickLoadSlot3(AppContext* ctx);
void onClickSaveSlot3(AppContext* ctx);
//主介面
void renderTitle(AppContext* ctx);
void MainMenu(AppContext* ctx);




#endif