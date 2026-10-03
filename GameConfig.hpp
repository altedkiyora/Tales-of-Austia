// ------------------------------------------------------------------------
// GameConfig.hpp  -  Tales of Austia
// Window size + the big screen state machine (flat enum, one live hand).
// Keep this module paper-thin: just "which screens exist + which is on".
// Only ever included from iMain.cpp (single translation unit).
// ------------------------------------------------------------------------
#pragma once

// -------------------------------------------------------------
// Tales of Austia — Starting Screen (iGraphics v4.0)
// Uses iGraphics DEFAULT library font only (iText + GLUT bitmaps)
// No custom font PNGs, no raw GL calls
// Reference: docs/iGraphics-AUST-Reference.md §5 + Developer Guide §3
// -------------------------------------------------------------

const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 800;

// ---- Game states (flat enum FSM — Guide §5.1) ----
enum GameState { GAME_STATE_MENU = 0, GAME_STATE_ABOUT = 1, GAME_STATE_PLAY = 2, GAME_STATE_SCENE2 = 3, GAME_STATE_SCENE3 = 4, GAME_STATE_SCENE4 = 5, GAME_STATE_TRANSITION = 6, GAME_STATE_SCENE5 = 7, GAME_STATE_INTERSTITIAL = 8, GAME_STATE_SCENE6 = 9, GAME_STATE_SCENE7 = 10, GAME_STATE_SCENE8 = 11, GAME_STATE_SCENE9 = 12, GAME_STATE_SPLASH = 13, GAME_STATE_BATTLE = 14, GAME_STATE_VICTORY = 15, GAME_STATE_SCENE10 = 16, GAME_STATE_CLASSROOM1 = 17, GAME_STATE_CLASSROOM2 = 18, GAME_STATE_LIBRARY = 19, GAME_STATE_LIBRARY2 = 20, GAME_STATE_BIGGATE = 21, GAME_STATE_BOSSROOM = 22, GAME_STATE_BOOKQUIZ = 23, GAME_STATE_LASTBRAWL = 24, GAME_STATE_ENDING = 25, GAME_STATE_PLANEGAME = 26 };
int currentGameState = GAME_STATE_MENU;

