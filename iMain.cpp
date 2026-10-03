#include "iGraphics.h"
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <cstring>
#include <cctype>

// ============================================================
// Tales of Austia  -  iGraphics v4.0, VS2013 (v120 toolset)
// Single translation unit. Code lives in the module headers
// listed below (in dependency order); this file keeps the
// iGraphics callbacks, the audio glue, the mouse/state machine
// (iMouse, fixedUpdate) and main().
// ============================================================

// -- GameConfig.hpp
#include "GameConfig.hpp"
// -- AssetRegistry.hpp
#include "AssetRegistry.hpp"
// -- GameState.hpp
#include "GameState.hpp"
// -- Brawl.hpp
#include "Brawl.hpp"
// -- Battle.hpp
#include "Battle.hpp"
// -- PlaneGame.hpp
#include "PlaneGame.hpp"
// -- LevelData.hpp
#include "LevelData.hpp"
// -- Collision.hpp
#include "Collision.hpp"
// -- Ui.hpp
#include "Ui.hpp"
// -- SceneFlow.hpp
#include "SceneFlow.hpp"
// -- Scenes.hpp
#include "Scenes.hpp"

// -------------------------------------------------------------
// iGraphics callbacks (Reference §4)
// -------------------------------------------------------------

void iDraw() {
    iClear();
    if (currentGameState == GAME_STATE_MENU) drawSceneMenu();
    else if (currentGameState == GAME_STATE_ABOUT) drawSceneAbout();
    else if (currentGameState == GAME_STATE_PLAY) drawScenePlay();
    else if (currentGameState == GAME_STATE_SCENE2) drawSceneScene2();
    else if (currentGameState == GAME_STATE_SCENE3) drawSceneScene3();
    else if (currentGameState == GAME_STATE_TRANSITION) drawSceneTransition();
    else if (currentGameState == GAME_STATE_SCENE4) drawSceneScene4();
    else if (currentGameState == GAME_STATE_SCENE5) drawSceneScene5();
    else if (currentGameState == GAME_STATE_SCENE6) drawSceneScene6();
    else if (currentGameState == GAME_STATE_SCENE7) drawSceneScene7();
    else if (currentGameState == GAME_STATE_SCENE8) drawSceneScene8();
    else if (currentGameState == GAME_STATE_SCENE9) drawSceneScene9();
    else if (currentGameState == GAME_STATE_SCENE10) drawSceneScene10();
    else if (currentGameState == GAME_STATE_CLASSROOM1) drawSceneClassroom1();
    else if (currentGameState == GAME_STATE_CLASSROOM2) drawSceneClassroom2();
    else if (currentGameState == GAME_STATE_PLANEGAME) drawScenePlaneGame();
    else if (currentGameState == GAME_STATE_LIBRARY) drawSceneLibrary();
    else if (currentGameState == GAME_STATE_LIBRARY2) drawSceneLibrary2();
    else if (currentGameState == GAME_STATE_BIGGATE) drawSceneBigGate();
    else if (currentGameState == GAME_STATE_BOSSROOM) drawSceneBossRoom();
    else if (currentGameState == GAME_STATE_SPLASH) drawSceneSplash();
    else if (currentGameState == GAME_STATE_BATTLE) drawSceneBattle();
    else if (currentGameState == GAME_STATE_LASTBRAWL) drawSceneLastBrawl(); // LASTBRAWL: remove to revert
    else if (currentGameState == GAME_STATE_ENDING) drawSceneEnding(); // ENDING: remove to revert
    else if (currentGameState == GAME_STATE_VICTORY) drawSceneVictory();
    else if (currentGameState == GAME_STATE_BOOKQUIZ) drawSceneBookQuiz();
    else if (currentGameState == GAME_STATE_INTERSTITIAL) drawSceneInterstitial();
}

void iMouseMove(int newMouseX, int newMouseY) {
    currentMouseX = newMouseX; currentMouseY = newMouseY;
    updateButtonHoverStates();
}

void iPassiveMouseMove(int newMouseX, int newMouseY) {
    currentMouseX = newMouseX; currentMouseY = newMouseY;
    updateButtonHoverStates();
}

void playClickSound() {
    mciSendString("play clicksfx from 0", NULL, 0, NULL);
}

// Gated music starters: no-ops while the MUSIC toggle is off.
void startMusicLoop(const char* alias) {
    if (!musicEnabled) return;
    char loopCommand[64];
    sprintf_s(loopCommand, "play %s repeat", alias);
    mciSendString(loopCommand, NULL, 0, NULL);
}
void restartMusic(const char* alias) {
    if (!musicEnabled) return;
    char restartCommand[64];
    sprintf_s(restartCommand, "play %s from 0", alias);
    mciSendString(restartCommand, NULL, 0, NULL);
}
void stopAllMusic() {
    mciSendString("stop menumusic", NULL, 0, NULL);
    mciSendString("stop aftercheckpoint", NULL, 0, NULL);
    mciSendString("stop rbd", NULL, 0, NULL);
}
// Stage music: aftercheckpoint loops from the end of Online till run end,
// menumusic everywhere before that (Guide §3.1 audio: one alias, repeat).
void startStageMusic() {
    if (checkpointMusic) startMusicLoop("aftercheckpoint");
    else startMusicLoop("menumusic");
}
void toggleMusic() {
    musicEnabled = !musicEnabled;
    musicButton.text = musicEnabled ? "MUSIC: ON" : "MUSIC: OFF";
    if (!musicEnabled) { stopAllMusic(); return; }
    // resume the track that belongs to the run: checkpoint past Online, rbd mid-transition
    if (checkpointMusic && currentGameState != GAME_STATE_INTERSTITIAL) startMusicLoop("aftercheckpoint");
    else if (currentGameState == GAME_STATE_INTERSTITIAL) restartMusic("rbd");
    else startMusicLoop("menumusic");
}

// Open with console error report — MCI fails silently otherwise (err 275 = file not found)
// Returns 0 on success, so callers can fall back to another file with the same alias.
MCIERROR openAudioAlias(const char* filePath, const char* aliasName) {
    char openCommand[512], errorText[256];
    sprintf_s(openCommand, "open \"%s\" alias %s", filePath, aliasName);
    MCIERROR openError = mciSendString(openCommand, NULL, 0, NULL);
    if (openError) {
        mciGetErrorString(openError, errorText, 256);
        printf("AUDIO FAIL: [%s] as [%s] err=%u [%s]\n", filePath, aliasName, openError, errorText);
    } else {
        printf("AUDIO OK: [%s] as [%s]\n", filePath, aliasName);
    }
    return openError;
}

// Single place for MENU<->PLAY state switches.
void switchToPlayScene() {
    if (currentGameState == GAME_STATE_PLAY) return;
    currentGameState = GAME_STATE_PLAY;
    crDefeated = 0; // new run: the door tells the epitaph again
    hasIdCard = 0; // new run: the gate stays shut until the book is found
    gateCooldownTicks = 0;
    dialogueTicksLeft = 0;
    seniorPenQueued = 0;
    checkpointMusic = 0; // new run: menu loop until the end of Online again
    exitCooldownTicks = EXIT_COOLDOWN_TICKS;
    playerSquareX = SCREEN_WIDTH / 2 - 24; playerSquareY = SCREEN_HEIGHT / 2 - 24;
}
void returnToMainMenu() {
    // make sure scene tracks never leak over the menu loop
    if (currentGameState == GAME_STATE_INTERSTITIAL) {
        mciSendString("stop rbd", NULL, 0, NULL);
        interstitialNextState = -1;
    }
    if (checkpointMusic || currentGameState == GAME_STATE_SCENE5) {
        // end of run: checkpoint loop stops, menu loop takes over
        checkpointMusic = 0;
        mciSendString("stop aftercheckpoint", NULL, 0, NULL);
        startMusicLoop("menumusic");
    } else if (currentGameState == GAME_STATE_INTERSTITIAL) {
        startMusicLoop("menumusic");
    }
    currentGameState = GAME_STATE_MENU;
}

void iMouse(int mouseButton, int mouseButtonState, int clickPositionX, int clickPositionY) {
    currentMouseX = clickPositionX; currentMouseY = clickPositionY;
    updateButtonHoverStates();
    // Press Red, release fires: DOWN arms red, UP over the same button acts.
    if (mouseButton == GLUT_LEFT_BUTTON && mouseButtonState == GLUT_DOWN) {
        clearAllPressedStates();
        if (currentGameState == GAME_STATE_MENU) {
            if (isPointInsideRect(clickPositionX, clickPositionY, playButton.left, playButton.bottom, playButton.width, playButton.height)) playButton.isPressed = true;
            else if (isPointInsideRect(clickPositionX, clickPositionY, aboutButton.left, aboutButton.bottom, aboutButton.width, aboutButton.height)) aboutButton.isPressed = true;
            else if (isPointInsideRect(clickPositionX, clickPositionY, musicButton.left, musicButton.bottom, musicButton.width, musicButton.height)) musicButton.isPressed = true;
            else if (isPointInsideRect(clickPositionX, clickPositionY, exitButton.left, exitButton.bottom, exitButton.width, exitButton.height)) exitButton.isPressed = true;
        } else {
            if (currentGameState == GAME_STATE_BOOKQUIZ) {
                if (isPointInsideRect(clickPositionX, clickPositionY, quizOptAButton.left, quizOptAButton.bottom, quizOptAButton.width, quizOptAButton.height)) quizOptAButton.isPressed = true;
                else if (isPointInsideRect(clickPositionX, clickPositionY, quizOptBButton.left, quizOptBButton.bottom, quizOptBButton.width, quizOptBButton.height)) quizOptBButton.isPressed = true;
            } else if (isPointInsideRect(clickPositionX, clickPositionY, backButton.left, backButton.bottom, backButton.width, backButton.height)) backButton.isPressed = true;
        }
    }
    if (mouseButton == GLUT_LEFT_BUTTON && mouseButtonState == GLUT_UP) {
        bool releasedOnPlay = playButton.isPressed && isPointInsideRect(clickPositionX, clickPositionY, playButton.left, playButton.bottom, playButton.width, playButton.height);
        bool releasedOnAbout = aboutButton.isPressed && isPointInsideRect(clickPositionX, clickPositionY, aboutButton.left, aboutButton.bottom, aboutButton.width, aboutButton.height);
        bool releasedOnMusic = musicButton.isPressed && isPointInsideRect(clickPositionX, clickPositionY, musicButton.left, musicButton.bottom, musicButton.width, musicButton.height);
        bool releasedOnExit = exitButton.isPressed && isPointInsideRect(clickPositionX, clickPositionY, exitButton.left, exitButton.bottom, exitButton.width, exitButton.height);
        bool releasedOnBack = backButton.isPressed && isPointInsideRect(clickPositionX, clickPositionY, backButton.left, backButton.bottom, backButton.width, backButton.height);
        bool releasedOnQuizA = quizOptAButton.isPressed && isPointInsideRect(clickPositionX, clickPositionY, quizOptAButton.left, quizOptAButton.bottom, quizOptAButton.width, quizOptAButton.height);
        bool releasedOnQuizB = quizOptBButton.isPressed && isPointInsideRect(clickPositionX, clickPositionY, quizOptBButton.left, quizOptBButton.bottom, quizOptBButton.width, quizOptBButton.height);
        clearAllPressedStates();
        if (currentGameState == GAME_STATE_MENU) {
            if (releasedOnPlay) { playClickSound(); switchToPlayScene(); }
            else if (releasedOnAbout) { playClickSound(); currentGameState = GAME_STATE_ABOUT; }
            else if (releasedOnMusic) { playClickSound(); toggleMusic(); }
            else if (releasedOnExit) exit(0); // no click sound: exit() cuts it off (iDelay banned)
        } else if (currentGameState == GAME_STATE_SCENE2) {
            // one-way destiny: BACK cannot go back to scene 1 (blocked with popup)
            if (releasedOnBack) { playClickSound(); showDialogue(NO_RETURN_MESSAGE); }
        } else if (currentGameState == GAME_STATE_SCENE3) {
            // one-way destiny: BACK cannot go back to scene 2 (blocked with popup)
            if (releasedOnBack) { playClickSound(); showDialogue(NO_RETURN_MESSAGE); }
        } else if (currentGameState == GAME_STATE_TRANSITION) {
            // one-way destiny: BACK cannot go back either (blocked with popup)
            if (releasedOnBack) { playClickSound(); showDialogue(NO_RETURN_MESSAGE); }
        } else if (currentGameState == GAME_STATE_SCENE4) {
            // one-way destiny: BACK cannot go back to scene 3 (blocked with popup)
            if (releasedOnBack) { playClickSound(); showDialogue(NO_RETURN_MESSAGE); }
        } else if (currentGameState == GAME_STATE_SCENE5) {
            // one-way destiny: BACK cannot go back to scene 4 (blocked with popup)
            if (releasedOnBack) { playClickSound(); showDialogue(NO_RETURN_MESSAGE); }
        } else if (currentGameState == GAME_STATE_SCENE6) {
            // one-way destiny: BACK cannot go back (blocked with popup, right-click still reaches the menu)
            if (releasedOnBack) { playClickSound(); showDialogue(NO_RETURN_MESSAGE); }
        } else if (currentGameState == GAME_STATE_SCENE7) {
            // one-way destiny: BACK cannot go back (blocked with popup, right-click still reaches the menu)
            if (releasedOnBack) { playClickSound(); showDialogue(NO_RETURN_MESSAGE); }
        } else if (currentGameState == GAME_STATE_SCENE8) {
            // one-way destiny: BACK cannot go back to the maze (blocked with popup)
            if (releasedOnBack) { playClickSound(); showDialogue(NO_RETURN_MESSAGE); }
        } else if (currentGameState == GAME_STATE_SCENE9) {
            // one-way destiny: BACK cannot go back (blocked with popup, right-click still reaches the menu)
            if (releasedOnBack) { playClickSound(); showDialogue(NO_RETURN_MESSAGE); }
        } else if (currentGameState == GAME_STATE_SCENE10) {
            // one-way destiny: BACK cannot go back (blocked with popup, right-click still reaches the menu)
            if (releasedOnBack) { playClickSound(); showDialogue(NO_RETURN_MESSAGE); }
        } else if (currentGameState == GAME_STATE_CLASSROOM1) {
            // one-way destiny: BACK cannot go back to scene 10 (blocked with popup)
            if (releasedOnBack) { playClickSound(); showDialogue(NO_RETURN_MESSAGE); }
        } else if (currentGameState == GAME_STATE_CLASSROOM2) {
            // one-way destiny: BACK cannot go back (same classroom, blocked with popup)
            if (releasedOnBack) { playClickSound(); showDialogue(NO_RETURN_MESSAGE); }
        } else if (currentGameState == GAME_STATE_PLANEGAME) {
            // minigame in progress: BACK cannot quit it (ESC gives up back to Rino)
            if (releasedOnBack) { playClickSound(); showDialogue(NO_RETURN_MESSAGE); }
        } else if (currentGameState == GAME_STATE_LIBRARY) {
            // one-way destiny: BACK cannot go back to classroom (blocked with popup)
            if (releasedOnBack) { playClickSound(); showDialogue(NO_RETURN_MESSAGE); }
        } else if (currentGameState == GAME_STATE_LIBRARY2) {
            // one-way destiny: BACK cannot go back (same library, blocked with popup)
            if (releasedOnBack) { playClickSound(); showDialogue(NO_RETURN_MESSAGE); }
        } else if (currentGameState == GAME_STATE_BIGGATE) {
            // one-way destiny: BACK cannot go back to the library (blocked with popup)
            if (releasedOnBack) { playClickSound(); showDialogue(NO_RETURN_MESSAGE); }
        } else if (currentGameState == GAME_STATE_BOSSROOM) {
            // one-way destiny: BACK cannot go back to the gate (blocked with popup)
            if (releasedOnBack) { playClickSound(); showDialogue(NO_RETURN_MESSAGE); }
        } else if (currentGameState == GAME_STATE_SPLASH) {
            // one-way destiny: BACK cannot go back to scene 9 (blocked with popup)
            if (releasedOnBack) { playClickSound(); showDialogue(NO_RETURN_MESSAGE); }
        } else if (currentGameState == GAME_STATE_BATTLE) {
            for (int battleOptI = 0; battleOptI < 4; battleOptI++) {
                int battleOptBottom = BATTLE_OPT_Y0 + battleOptI * (BATTLE_OPT_H + BATTLE_OPT_GAP);
                if (isPointInsideRect(clickPositionX, clickPositionY, BATTLE_OPT_X, battleOptBottom, BATTLE_OPT_W, BATTLE_OPT_H)) {
                    battleChooseMove(battleOptI);
                    break;
                }
            }
        } else if (currentGameState == GAME_STATE_VICTORY) {
            // one-way destiny: BACK cannot go back (blocked with popup, right-click still reaches the menu)
            if (releasedOnBack) { playClickSound(); showDialogue(NO_RETURN_MESSAGE); }
        } else if (currentGameState == GAME_STATE_BOOKQUIZ) {
            // quiz option boxes: press red, release fires the answer
            if (releasedOnQuizA) answerBookQuiz('A');
            else if (releasedOnQuizB) answerBookQuiz('B');
        } else if (currentGameState == GAME_STATE_INTERSTITIAL) {
            if (releasedOnBack) { playClickSound(); returnToMainMenu(); }
        } else {
            if (releasedOnBack) { playClickSound(); returnToMainMenu(); }
        }
    }
    if (mouseButton == GLUT_RIGHT_BUTTON && mouseButtonState == GLUT_DOWN) {
        if (currentGameState != GAME_STATE_MENU) returnToMainMenu();
    }
}

void fixedUpdate() {
    animationTickCounter++;
    if (exitCooldownTicks > 0) exitCooldownTicks--;
    {
        // debug warp cheat: edge-triggered N jumps exactly one scene per press
        static int lastWarpKeyDown = 0;
        int warpKeyDown = isKeyPressed('n') || isKeyPressed('N');
        if (warpKeyDown && !lastWarpKeyDown) { playClickSound(); debugWarpNext(); }
        lastWarpKeyDown = warpKeyDown;
    }
    {
        // border-outline visibility: edge-triggered B toggles; collision stays active
        static int lastBorderKeyDown = 0;
        int borderKeyDown = isKeyPressed('b') || isKeyPressed('B');
        if (borderKeyDown && !lastBorderKeyDown) { playClickSound(); showCollisionBorders = !showCollisionBorders; }
        lastBorderKeyDown = borderKeyDown;
    }

    if (currentGameState == GAME_STATE_PLAY) {
        int playerMoveSpeed = 4; // px/tick — Guide §3.1: ticks not seconds
        playerIsMoving = 0;
        if ((isKeyPressed('w') || isKeyPressed('W')) || isSpecialKeyPressed(GLUT_KEY_UP))    { playerSquareY += playerMoveSpeed; playerFacing = 0; playerIsMoving = 1; }
        if ((isKeyPressed('s') || isKeyPressed('S')) || isSpecialKeyPressed(GLUT_KEY_DOWN))  { playerSquareY -= playerMoveSpeed; playerFacing = 1; playerIsMoving = 1; }
        if ((isKeyPressed('a') || isKeyPressed('A')) || isSpecialKeyPressed(GLUT_KEY_LEFT))  { playerSquareX -= playerMoveSpeed; playerFacing = 3; playerIsMoving = 1; }
        if ((isKeyPressed('d') || isKeyPressed('D')) || isSpecialKeyPressed(GLUT_KEY_RIGHT)) { playerSquareX += playerMoveSpeed; playerFacing = 2; playerIsMoving = 1; }
        // stair collision border: feet circle vs closed loop, then screen clamps
        double feetX = playerSquareX + 24.0, feetY = (double)playerSquareY;
        resolveCollisionBorder(&feetX, &feetY);
        playerSquareX = (int)(feetX - 24.0);
        playerSquareY = (int)feetY;
        if (playerSquareX < 6) playerSquareX = 6;
        if (playerSquareY < 60) playerSquareY = 60;
        if (playerSquareX > SCREEN_WIDTH - 54) playerSquareX = SCREEN_WIDTH - 54;
        if (playerSquareY > SCREEN_HEIGHT - 90) playerSquareY = SCREEN_HEIGHT - 90;

        if (isKeyPressed(' ')) {
            static int lastClickSoundTick = -100;
            if (animationTickCounter - lastClickSoundTick > 10) {
                mciSendString("play clicksfx from 0", NULL, 0, NULL);
                lastClickSoundTick = animationTickCounter;
            }
        }

        // ID card pickup: PRESS X near the book on the platform
        if (!hasIdCard && playerDistSqTo(ID_CARD_X, ID_CARD_Y) < ID_CARD_PICKUP_RADIUS * ID_CARD_PICKUP_RADIUS && isInteractKeyPressed()) {
            hasIdCard = 1;
            playClickSound();
            showDialogue("ID CARD ACQUIRED - THE GATE WILL OPEN");
        }

        // dialogue + gate cooldown timers (ticks, Guide §3.1)
        if (dialogueTicksLeft > 0) dialogueTicksLeft--;
        if (gateCooldownTicks > 0) gateCooldownTicks--;

        // gate: PRESS X while pushing against the top edge inside the archway band
        {
            int feetCenterX = playerSquareX + 24;
            if (feetCenterX >= GATE_X0 && feetCenterX <= GATE_X1 && playerSquareY >= GATE_TRIGGER_Y && isInteractKeyPressed() && gateCooldownTicks == 0 && exitCooldownTicks == 0) {
                gateCooldownTicks = GATE_COOLDOWN_TICKS;
                if (hasIdCard) {
                    playClickSound();
                    savedScene1X = playerSquareX; // restore on BACK (else region yanks the hero)
                    savedScene1Y = playerSquareY;
                    playerSquareX = SCREEN_WIDTH / 2 - 24; // scene-2 entrance (feet 600,350 inside region)
                    playerSquareY = 350;
                    playerFacing = 0;
                    playerIsMoving = 0;
                    startInterstitial("the void is watching ", GAME_STATE_SCENE2);
                } else {
                    playClickSound();
                    showDialogue("GUARD: SHOW ME ID CARD");
                }
            }
        }

        if (isKeyPressed(27)) returnToMainMenu();
    } else if (currentGameState == GAME_STATE_MENU) {
        if (isKeyPressed('1')) { playClickSound(); switchToPlayScene(); }
        if (isKeyPressed('2')) { playClickSound(); currentGameState = GAME_STATE_ABOUT; }
        if (isKeyPressed('3')) exit(0);
        {
            // edge-triggered so holding M toggles exactly once
            static int lastMusicKeyDown = 0;
            int musicKeyDown = isKeyPressed('m') || isKeyPressed('M');
            if (musicKeyDown && !lastMusicKeyDown) { playClickSound(); toggleMusic(); }
            lastMusicKeyDown = musicKeyDown;
        }
        if (isKeyPressed(13)) { playClickSound(); switchToPlayScene(); }
    } else if (currentGameState == GAME_STATE_ABOUT) {
        if (isKeyPressed(27)) currentGameState = GAME_STATE_MENU;
    } else if (currentGameState == GAME_STATE_SCENE2) {
        if (dialogueTicksLeft > 0) dialogueTicksLeft--;
        // senior chained beat: once senior line fades, the Pen unlock pops
        if (seniorPenQueued && dialogueTicksLeft <= 0) {
            seniorPenQueued = 0;
            showDialogue("The Pen of HIGHCG: Matador unlocked");
        }
        // one-way destiny: no going back to scene 1 (blocked with popup)
        if (exitCooldownTicks == 0 && isPlayerAtBackExit() && isInteractKeyPressed()) {
            playClickSound();
            showDialogue(NO_RETURN_MESSAGE);
            return;
        }
        // next floor exit (top-left): PRESS X here goes up
        if (exitCooldownTicks == 0 && isPlayerAtNextFloor() && isInteractKeyPressed()) {
            playClickSound();
            switchToScene3();
        } else if (isSeniorInTalkRange() && isInteractKeyPressed()) {
            // talk to the senior: PRESS X in range shows his line
            playClickSound();
            showDialogue("Kire 1.1 naki? Intro de. Accha thak shorok e ashis. Eta dhor");
            seniorPenQueued = 1; // chained Pen unlock after this line fades
        }
        // hero walks scene 2 inside its collision region, then screen clamps
        int scene2MoveSpeed = 4;
        playerIsMoving = 0;
        if ((isKeyPressed('w') || isKeyPressed('W')) || isSpecialKeyPressed(GLUT_KEY_UP))    { playerSquareY += scene2MoveSpeed; playerFacing = 0; playerIsMoving = 1; }
        if ((isKeyPressed('s') || isKeyPressed('S')) || isSpecialKeyPressed(GLUT_KEY_DOWN))  { playerSquareY -= scene2MoveSpeed; playerFacing = 1; playerIsMoving = 1; }
        if ((isKeyPressed('a') || isKeyPressed('A')) || isSpecialKeyPressed(GLUT_KEY_LEFT))  { playerSquareX -= scene2MoveSpeed; playerFacing = 3; playerIsMoving = 1; }
        if ((isKeyPressed('d') || isKeyPressed('D')) || isSpecialKeyPressed(GLUT_KEY_RIGHT)) { playerSquareX += scene2MoveSpeed; playerFacing = 2; playerIsMoving = 1; }
        {
            double scene2FeetX = playerSquareX + 24.0, scene2FeetY = (double)playerSquareY;
            resolveScene2Border(&scene2FeetX, &scene2FeetY);
            pushOutOfRect(&scene2FeetX, &scene2FeetY, SENIOR_BLOCK_X, SENIOR_BLOCK_Y, SENIOR_BLOCK_W, SENIOR_BLOCK_H);
            playerSquareX = (int)(scene2FeetX - 24.0);
            playerSquareY = (int)scene2FeetY;
        }
        if (playerSquareX < 6) playerSquareX = 6;
        if (playerSquareY < 60) playerSquareY = 60;
        if (playerSquareX > SCREEN_WIDTH - 54) playerSquareX = SCREEN_WIDTH - 54;
        if (playerSquareY > SCREEN_HEIGHT - 90) playerSquareY = SCREEN_HEIGHT - 90;
        // one-way destiny: ESC cannot go back to scene 1 (blocked with popup)
        if (isKeyPressed(27)) { playClickSound(); showDialogue(NO_RETURN_MESSAGE); }
    } else if (currentGameState == GAME_STATE_SCENE3) {
        if (dialogueTicksLeft > 0) dialogueTicksLeft--;
        // one-way destiny: no going back to scene 2 (blocked with popup)
        if (exitCooldownTicks == 0 && isPlayerAtBackExit() && isInteractKeyPressed()) {
            playClickSound();
            showDialogue(NO_RETURN_MESSAGE);
            return;
        }
        // CR talk on senior's old spot: 1st X talks, 2nd X (after dialogue done) casts -> scene 4
        if (isCrInTalkRange() && isInteractKeyPressed()) {
            if (dialogueTicksLeft <= 0 && crHasTalked) {
                playClickSound();
                crHasTalked = 0; // one-shot per visit
                castReturnState = GAME_STATE_SCENE4;
                // end of Online: checkpoint loop takes over till run end
                checkpointMusic = 1;
                mciSendString("stop menumusic", NULL, 0, NULL);
                startMusicLoop("aftercheckpoint");
                startCastAnimation();
                return;
            } else if (dialogueTicksLeft <= 0 && !crHasTalked) {
                playClickSound();
                showDialogue("CR: Online de !!!");
                crHasTalked = 1;
            }
        }
        // next floor walks like scene 2 for now (own loop can be added later)
        int scene3MoveSpeed = 4;
        playerIsMoving = 0;
        if ((isKeyPressed('w') || isKeyPressed('W')) || isSpecialKeyPressed(GLUT_KEY_UP))    { playerSquareY += scene3MoveSpeed; playerFacing = 0; playerIsMoving = 1; }
        if ((isKeyPressed('s') || isKeyPressed('S')) || isSpecialKeyPressed(GLUT_KEY_DOWN))  { playerSquareY -= scene3MoveSpeed; playerFacing = 1; playerIsMoving = 1; }
        if ((isKeyPressed('a') || isKeyPressed('A')) || isSpecialKeyPressed(GLUT_KEY_LEFT))  { playerSquareX -= scene3MoveSpeed; playerFacing = 3; playerIsMoving = 1; }
        if ((isKeyPressed('d') || isKeyPressed('D')) || isSpecialKeyPressed(GLUT_KEY_RIGHT)) { playerSquareX += scene3MoveSpeed; playerFacing = 2; playerIsMoving = 1; }
        {
            double scene3FeetX = playerSquareX + 24.0, scene3FeetY = (double)playerSquareY;
            resolveScene2Border(&scene3FeetX, &scene3FeetY);
            pushOutOfRect(&scene3FeetX, &scene3FeetY, CR_BLOCK_X, CR_BLOCK_Y, CR_BLOCK_W, CR_BLOCK_H);
            playerSquareX = (int)(scene3FeetX - 24.0);
            playerSquareY = (int)scene3FeetY;
        }
        if (playerSquareX < 6) playerSquareX = 6;
        if (playerSquareY < 60) playerSquareY = 60;
        if (playerSquareX > SCREEN_WIDTH - 54) playerSquareX = SCREEN_WIDTH - 54;
        if (playerSquareY > SCREEN_HEIGHT - 90) playerSquareY = SCREEN_HEIGHT - 90;
        // one-way destiny: ESC cannot go back to scene 2 (blocked with popup)
        if (isKeyPressed(27)) { playClickSound(); showDialogue(NO_RETURN_MESSAGE); }
    } else if (currentGameState == GAME_STATE_TRANSITION) {
        // hero-sized cast animation at hero spot, then auto-enter the target scene
        if (castAnimTicksLeft > 0) castAnimTicksLeft--;
        if (castAnimTicksLeft <= 0) {
            playClickSound();
            if (castReturnState == GAME_STATE_SCENE7) returnToScene7AfterFail();
            else switchToScene4();
        }
        // ESC cannot go back either (blocked with popup, fail-cast must play out)
        if (castReturnState == GAME_STATE_SCENE4 && isKeyPressed(27)) { playClickSound(); showDialogue(NO_RETURN_MESSAGE); }
    } else if (currentGameState == GAME_STATE_SCENE4) {
        if (dialogueTicksLeft > 0) dialogueTicksLeft--;
        // 15s timer -> scene 5 (empty map)
        if (scene4TimerTicks > 0) scene4TimerTicks--;
        if (scene4TimerTicks <= 0) {
            playClickSound();
            switchToScene5();
            return;
        }
        // ledge walk: feet locked to the Y 312..316 polygon strip (W/S only tilt facing)
        int scene4MoveSpeed = 4;
        playerIsMoving = 0;
        if ((isKeyPressed('w') || isKeyPressed('W')) || isSpecialKeyPressed(GLUT_KEY_UP))    { playerSquareY += scene4MoveSpeed; playerFacing = 0; playerIsMoving = 1; }
        if ((isKeyPressed('s') || isKeyPressed('S')) || isSpecialKeyPressed(GLUT_KEY_DOWN))  { playerSquareY -= scene4MoveSpeed; playerFacing = 1; playerIsMoving = 1; }
        if ((isKeyPressed('a') || isKeyPressed('A')) || isSpecialKeyPressed(GLUT_KEY_LEFT))  { playerSquareX -= scene4MoveSpeed; playerFacing = 3; playerIsMoving = 1; }
        if ((isKeyPressed('d') || isKeyPressed('D')) || isSpecialKeyPressed(GLUT_KEY_RIGHT)) { playerSquareX += scene4MoveSpeed; playerFacing = 2; playerIsMoving = 1; }
        {
            double scene4FeetX = playerSquareX + 24.0, scene4FeetY = (double)playerSquareY;
            resolveScene4Border(&scene4FeetX, &scene4FeetY);
            playerSquareX = (int)(scene4FeetX - 24.0);
            playerSquareY = (int)scene4FeetY;
        }
        if (playerSquareX < 6) playerSquareX = 6;
        if (playerSquareX > SCREEN_WIDTH - 54) playerSquareX = SCREEN_WIDTH - 54;
        if (playerSquareY < SCENE4_MIN_Y) playerSquareY = SCENE4_MIN_Y;
        if (playerSquareY > SCENE4_MAX_Y) playerSquareY = SCENE4_MAX_Y;
        // one-way destiny: no return to scene 3 (blocked with popup)
        // HolyNova shower: falling 01s spawn above at random X, burst on ledge hit
        if (holyNovaSpawnTimer > 0) holyNovaSpawnTimer--;
        if (holyNovaSpawnTimer <= 0) {
            holyNovaSpawnTimer = HOLY_NOVA_SPAWN_EVERY;
            spawnHolyNovaAt(holyNovaRandomX(), SCREEN_HEIGHT);
        }
        for (int i = 0; i < HOLY_NOVA_MAX; i++) if (holyNovas[i].active) {
            if (holyNovas[i].state == 0) {
                holyNovas[i].y -= HOLY_NOVA_FALL_SPEED;
                if (holyNovas[i].y <= SCENE4_MIN_Y) {
                    holyNovas[i].y = SCENE4_MIN_Y;
                    holyNovas[i].state = 1; // impact -> burst
                    holyNovas[i].animTick = 0;
                }
            } else {
                holyNovas[i].animTick++;
                if (holyNovas[i].animTick >= (HOLY_NOVA_FRAMES - 1) * HOLY_NOVA_BURST_TICKS_PER_FRAME)
                    holyNovas[i].active = 0; // burst done
            }
            // touch damage: each nova counts once, every 5 touches drops the HP bar a stage
            if (holyNovas[i].active && !holyNovas[i].hitCounted && novaTouchesHero(&holyNovas[i])) {
                holyNovas[i].hitCounted = 1;
                heroHitCount++;
                heroHpStage = heroHitCount / HITS_PER_HP_STAGE;
                if (heroHpStage >= HERO_HP_STAGES) heroHpStage = HERO_HP_STAGES - 1;
                playClickSound(); // audible hit confirmation
            }
        }
        // one-way destiny: ESC cannot go back to scene 3 (blocked with popup)
        if (isKeyPressed(27)) { playClickSound(); showDialogue(NO_RETURN_MESSAGE); }
    } else if (currentGameState == GAME_STATE_SCENE5) {
        if (dialogueTicksLeft > 0) dialogueTicksLeft--;
        // top line: X advances to scene 6 (one-way, no way back to scene 4)
        if (exitCooldownTicks == 0 && isPlayerAtNextFloor() && isInteractKeyPressed()) {
            playClickSound();
            switchToScene6();
            return;
        }
        // empty map roam: same collision loop as scene 2/3, no NPC blockers
        int scene5MoveSpeed = 4;
        playerIsMoving = 0;
        if ((isKeyPressed('w') || isKeyPressed('W')) || isSpecialKeyPressed(GLUT_KEY_UP))    { playerSquareY += scene5MoveSpeed; playerFacing = 0; playerIsMoving = 1; }
        if ((isKeyPressed('s') || isKeyPressed('S')) || isSpecialKeyPressed(GLUT_KEY_DOWN))  { playerSquareY -= scene5MoveSpeed; playerFacing = 1; playerIsMoving = 1; }
        if ((isKeyPressed('a') || isKeyPressed('A')) || isSpecialKeyPressed(GLUT_KEY_LEFT))  { playerSquareX -= scene5MoveSpeed; playerFacing = 3; playerIsMoving = 1; }
        if ((isKeyPressed('d') || isKeyPressed('D')) || isSpecialKeyPressed(GLUT_KEY_RIGHT)) { playerSquareX += scene5MoveSpeed; playerFacing = 2; playerIsMoving = 1; }
        {
            double scene5FeetX = playerSquareX + 24.0, scene5FeetY = (double)playerSquareY;
            resolveScene2Border(&scene5FeetX, &scene5FeetY);
            playerSquareX = (int)(scene5FeetX - 24.0);
            playerSquareY = (int)scene5FeetY;
        }
        if (playerSquareX < 6) playerSquareX = 6;
        if (playerSquareY < 60) playerSquareY = 60;
        if (playerSquareX > SCREEN_WIDTH - 54) playerSquareX = SCREEN_WIDTH - 54;
        if (playerSquareY > SCREEN_HEIGHT - 90) playerSquareY = SCREEN_HEIGHT - 90;
        // one-way destiny: ESC cannot go back to scene 4 (blocked with popup)
        if (isKeyPressed(27)) { playClickSound(); showDialogue(NO_RETURN_MESSAGE); }
    } else if (currentGameState == GAME_STATE_SCENE6) {
        if (dialogueTicksLeft > 0) dialogueTicksLeft--;
        // door peek: X at the bottom-doors zone (post-victory: slaughter-house line)
        if (exitCooldownTicks == 0 && isPlayerAtDoorZone() && isInteractKeyPressed()) {
            playClickSound();
            if (crDefeated) showDialogue("Here is the slaughter house of checkpoint presentation, be gone from here");
            else startDoorPeek();
            return;
        }
        // top of left arm: X advances up to scene 7 (maze)
        if (exitCooldownTicks == 0 && isPlayerAtScene6UpExit() && isInteractKeyPressed()) {
            playClickSound();
            switchToScene7();
            return;
        }
        // one-way roam inside the scene-6 loop, no exits, no way back (ESC blocked with popup)
        int scene6MoveSpeed = 4;
        playerIsMoving = 0;
        if ((isKeyPressed('w') || isKeyPressed('W')) || isSpecialKeyPressed(GLUT_KEY_UP))    { playerSquareY += scene6MoveSpeed; playerFacing = 0; playerIsMoving = 1; }
        if ((isKeyPressed('s') || isKeyPressed('S')) || isSpecialKeyPressed(GLUT_KEY_DOWN))  { playerSquareY -= scene6MoveSpeed; playerFacing = 1; playerIsMoving = 1; }
        if ((isKeyPressed('a') || isKeyPressed('A')) || isSpecialKeyPressed(GLUT_KEY_LEFT))  { playerSquareX -= scene6MoveSpeed; playerFacing = 3; playerIsMoving = 1; }
        if ((isKeyPressed('d') || isKeyPressed('D')) || isSpecialKeyPressed(GLUT_KEY_RIGHT)) { playerSquareX += scene6MoveSpeed; playerFacing = 2; playerIsMoving = 1; }
        {
            double scene6FeetX = playerSquareX + 24.0, scene6FeetY = (double)playerSquareY;
            resolveScene6Border(&scene6FeetX, &scene6FeetY);
            playerSquareX = (int)(scene6FeetX - 24.0);
            playerSquareY = (int)scene6FeetY;
        }
        if (playerSquareX < 6) playerSquareX = 6;
        if (playerSquareY < 60) playerSquareY = 60;
        if (playerSquareX > SCREEN_WIDTH - 54) playerSquareX = SCREEN_WIDTH - 54;
        if (playerSquareY > SCREEN_HEIGHT - 90) playerSquareY = SCREEN_HEIGHT - 90;
        // one-way destiny: ESC cannot go back (blocked with popup)
        if (isKeyPressed(27)) { playClickSound(); showDialogue(NO_RETURN_MESSAGE); }
    } else if (currentGameState == GAME_STATE_SCENE7) {
        if (dialogueTicksLeft > 0) dialogueTicksLeft--;
        // maze center: X advances to scene 8
        if (exitCooldownTicks == 0 && isPlayerAtMazeGoal() && isInteractKeyPressed()) {
            playClickSound();
            switchToScene8();
            return;
        }
        // maze roam inside the outer hedge loop, no way back (ESC blocked with popup)
        int scene7MoveSpeed = 4;
        playerIsMoving = 0;
        if ((isKeyPressed('w') || isKeyPressed('W')) || isSpecialKeyPressed(GLUT_KEY_UP))    { playerSquareY += scene7MoveSpeed; playerFacing = 0; playerIsMoving = 1; }
        if ((isKeyPressed('s') || isKeyPressed('S')) || isSpecialKeyPressed(GLUT_KEY_DOWN))  { playerSquareY -= scene7MoveSpeed; playerFacing = 1; playerIsMoving = 1; }
        if ((isKeyPressed('a') || isKeyPressed('A')) || isSpecialKeyPressed(GLUT_KEY_LEFT))  { playerSquareX -= scene7MoveSpeed; playerFacing = 3; playerIsMoving = 1; }
        if ((isKeyPressed('d') || isKeyPressed('D')) || isSpecialKeyPressed(GLUT_KEY_RIGHT)) { playerSquareX += scene7MoveSpeed; playerFacing = 2; playerIsMoving = 1; }
        {
            double scene7FeetX = playerSquareX + 24.0, scene7FeetY = (double)playerSquareY;
            resolveScene7Border(&scene7FeetX, &scene7FeetY);
            playerSquareX = (int)(scene7FeetX - 24.0);
            playerSquareY = (int)scene7FeetY;
        }
        if (playerSquareX < 6) playerSquareX = 6;
        if (playerSquareY < 60) playerSquareY = 60;
        if (playerSquareX > SCREEN_WIDTH - 54) playerSquareX = SCREEN_WIDTH - 54;
        if (playerSquareY > SCREEN_HEIGHT - 90) playerSquareY = SCREEN_HEIGHT - 90;
        // one-way destiny: ESC cannot go back (blocked with popup)
        if (isKeyPressed(27)) { playClickSound(); showDialogue(NO_RETURN_MESSAGE); }
    } else if (currentGameState == GAME_STATE_SCENE8) {
        if (dialogueTicksLeft > 0) dialogueTicksLeft--;
        // goal first: reaching the top-left zone in time wins ties with the timer
        if (!scene8GoalReached && isPlayerAtNextFloor()) {
            scene8GoalReached = 1;
            playClickSound();
            showDialogue("phew , managed to reach on time!");
        }
        if (!scene8GoalReached) {
            // 10s race clock; timeout = fail: cast, back to maze, late line, -1 HP
            if (scene8TimerTicks > 0) scene8TimerTicks--;
            if (scene8TimerTicks <= 0) {
                playClickSound();
                castReturnState = GAME_STATE_SCENE7;
                startCastAnimation();
                return;
            }
        } else if (exitCooldownTicks == 0 && isPlayerAtNextFloor() && isInteractKeyPressed()) {
            playClickSound();
            playerSquareX = 1026; // feet (1050,660) top-right inside the scene-2 loop
            playerSquareY = 660;
            playerFacing = 1;
            playerIsMoving = 0;
            dialogueTicksLeft = 0;
            startInterstitial3("The void appreciates you.", "Citation shield unlocked", "", GAME_STATE_SCENE9);
            return;
        }
        // race roam: same collision loop as scene 2/3, ESC blocked with popup
        int scene8MoveSpeed = 4;
        playerIsMoving = 0;
        if ((isKeyPressed('w') || isKeyPressed('W')) || isSpecialKeyPressed(GLUT_KEY_UP))    { playerSquareY += scene8MoveSpeed; playerFacing = 0; playerIsMoving = 1; }
        if ((isKeyPressed('s') || isKeyPressed('S')) || isSpecialKeyPressed(GLUT_KEY_DOWN))  { playerSquareY -= scene8MoveSpeed; playerFacing = 1; playerIsMoving = 1; }
        if ((isKeyPressed('a') || isKeyPressed('A')) || isSpecialKeyPressed(GLUT_KEY_LEFT))  { playerSquareX -= scene8MoveSpeed; playerFacing = 3; playerIsMoving = 1; }
        if ((isKeyPressed('d') || isKeyPressed('D')) || isSpecialKeyPressed(GLUT_KEY_RIGHT)) { playerSquareX += scene8MoveSpeed; playerFacing = 2; playerIsMoving = 1; }
        {
            double scene8FeetX = playerSquareX + 24.0, scene8FeetY = (double)playerSquareY;
            resolveScene2Border(&scene8FeetX, &scene8FeetY);
            playerSquareX = (int)(scene8FeetX - 24.0);
            playerSquareY = (int)scene8FeetY;
        }
        if (playerSquareX < 6) playerSquareX = 6;
        if (playerSquareY < 60) playerSquareY = 60;
        if (playerSquareX > SCREEN_WIDTH - 54) playerSquareX = SCREEN_WIDTH - 54;
        if (playerSquareY > SCREEN_HEIGHT - 90) playerSquareY = SCREEN_HEIGHT - 90;
        // one-way destiny: ESC cannot go back to the maze (blocked with popup)
        if (isKeyPressed(27)) { playClickSound(); showDialogue(NO_RETURN_MESSAGE); }
    } else if (currentGameState == GAME_STATE_SCENE9) {
        if (dialogueTicksLeft > 0) dialogueTicksLeft--;
        // old-CR talk: 1st X = threat, 2nd X = visible transform to new_cr, then splash
        if (crTransformTicks > 0) {
            crTransformTicks--;
            if (crTransformTicks <= 0) {
                playClickSound();
                startSplash();
                return;
            }
        } else if (isCrInTalkRange() && isInteractKeyPressed()) {
            if (dialogueTicksLeft <= 0 && crAttackTalked) {
                playClickSound();
                crTransformed = 1;
                crTransformTicks = CR_TRANSFORM_TICKS;
            } else if (dialogueTicksLeft <= 0 && !crAttackTalked) {
                playClickSound();
                showDialogue("Oh to mara khas nai ehono, ei ne el routina de MID");
                crAttackTalked = 1;
            }
        }
        // roam with the 30x40 attack blocker, one-way destiny (ESC blocked with popup)
        int scene9MoveSpeed = 4;
        playerIsMoving = 0;
        if ((isKeyPressed('w') || isKeyPressed('W')) || isSpecialKeyPressed(GLUT_KEY_UP))    { playerSquareY += scene9MoveSpeed; playerFacing = 0; playerIsMoving = 1; }
        if ((isKeyPressed('s') || isKeyPressed('S')) || isSpecialKeyPressed(GLUT_KEY_DOWN))  { playerSquareY -= scene9MoveSpeed; playerFacing = 1; playerIsMoving = 1; }
        if ((isKeyPressed('a') || isKeyPressed('A')) || isSpecialKeyPressed(GLUT_KEY_LEFT))  { playerSquareX -= scene9MoveSpeed; playerFacing = 3; playerIsMoving = 1; }
        if ((isKeyPressed('d') || isKeyPressed('D')) || isSpecialKeyPressed(GLUT_KEY_RIGHT)) { playerSquareX += scene9MoveSpeed; playerFacing = 2; playerIsMoving = 1; }
        {
            double scene9FeetX = playerSquareX + 24.0, scene9FeetY = (double)playerSquareY;
            resolveScene2Border(&scene9FeetX, &scene9FeetY);
            pushOutOfRect(&scene9FeetX, &scene9FeetY, ATTACK_BLOCK_X, ATTACK_BLOCK_Y, ATTACK_BLOCK_W, ATTACK_BLOCK_H);
            playerSquareX = (int)(scene9FeetX - 24.0);
            playerSquareY = (int)scene9FeetY;
        }
        if (playerSquareX < 6) playerSquareX = 6;
        if (playerSquareY < 60) playerSquareY = 60;
        if (playerSquareX > SCREEN_WIDTH - 54) playerSquareX = SCREEN_WIDTH - 54;
        if (playerSquareY > SCREEN_HEIGHT - 90) playerSquareY = SCREEN_HEIGHT - 90;
        // one-way destiny: ESC cannot go back (blocked with popup)
        if (isKeyPressed(27)) { playClickSound(); showDialogue(NO_RETURN_MESSAGE); }
    } else if (currentGameState == GAME_STATE_SCENE10) {
        if (dialogueTicksLeft > 0) dialogueTicksLeft--;
        // detached post-Aftermath roam: door peek + up exit to classroom (ESC blocked with popup)
        if (exitCooldownTicks == 0 && isPlayerAtDoorZone() && isInteractKeyPressed()) {
            playClickSound();
            if (crDefeated) showDialogue("Here is the slaughter house of checkpoint presentation, be gone from here");
            else startDoorPeek();
            return;
        }
        // top of left arm: X advances up to classroom 1 (next floor)
        if (exitCooldownTicks == 0 && isPlayerAtScene6UpExit() && isInteractKeyPressed()) {
            playClickSound();
            switchToClassroom1();
            return;
        }
        // one-way roam inside the scene-6 loop, no exits, no way back (ESC blocked with popup)
        int scene10MoveSpeed = 4;
        playerIsMoving = 0;
        if ((isKeyPressed('w') || isKeyPressed('W')) || isSpecialKeyPressed(GLUT_KEY_UP))    { playerSquareY += scene10MoveSpeed; playerFacing = 0; playerIsMoving = 1; }
        if ((isKeyPressed('s') || isKeyPressed('S')) || isSpecialKeyPressed(GLUT_KEY_DOWN))  { playerSquareY -= scene10MoveSpeed; playerFacing = 1; playerIsMoving = 1; }
        if ((isKeyPressed('a') || isKeyPressed('A')) || isSpecialKeyPressed(GLUT_KEY_LEFT))  { playerSquareX -= scene10MoveSpeed; playerFacing = 3; playerIsMoving = 1; }
        if ((isKeyPressed('d') || isKeyPressed('D')) || isSpecialKeyPressed(GLUT_KEY_RIGHT)) { playerSquareX += scene10MoveSpeed; playerFacing = 2; playerIsMoving = 1; }
        {
            double scene10FeetX = playerSquareX + 24.0, scene10FeetY = (double)playerSquareY;
            resolveScene6Border(&scene10FeetX, &scene10FeetY);
            playerSquareX = (int)(scene10FeetX - 24.0);
            playerSquareY = (int)scene10FeetY;
        }
        if (playerSquareX < 6) playerSquareX = 6;
        if (playerSquareY < 60) playerSquareY = 60;
        if (playerSquareX > SCREEN_WIDTH - 54) playerSquareX = SCREEN_WIDTH - 54;
        if (playerSquareY > SCREEN_HEIGHT - 90) playerSquareY = SCREEN_HEIGHT - 90;
        // one-way destiny: ESC cannot go back (blocked with popup)
        if (isKeyPressed(27)) { playClickSound(); showDialogue(NO_RETURN_MESSAGE); }
    } else if (currentGameState == GAME_STATE_CLASSROOM1) {
        if (dialogueTicksLeft > 0) dialogueTicksLeft--;
        // auto-advance: walk up to y=690 -> classroom2 (no X needed)
        // Mont (3_transparent) + Nopla (2_transparent): PRESS X in range talks
        if (isMontInTalkRange() && isInteractKeyPressed()) {
            if (dialogueTicksLeft <= 0) {
                playClickSound();
                showDialogue("Mont: Shahosh thakle shorok e ay");
            }
        } else if (isNoplaInTalkRange() && isInteractKeyPressed()) {
            if (dialogueTicksLeft <= 0) {
                playClickSound();
                showDialogue("Nopla: Mama abaro kamla dise, help kor");
            }
        }
        // classroom roam inside the scene-2 loop, no way back (ESC blocked with popup)
        int classroom1MoveSpeed = 4;
        playerIsMoving = 0;
        if ((isKeyPressed('w') || isKeyPressed('W')) || isSpecialKeyPressed(GLUT_KEY_UP))    { playerSquareY += classroom1MoveSpeed; playerFacing = 0; playerIsMoving = 1; }
        if ((isKeyPressed('s') || isKeyPressed('S')) || isSpecialKeyPressed(GLUT_KEY_DOWN))  { playerSquareY -= classroom1MoveSpeed; playerFacing = 1; playerIsMoving = 1; }
        if ((isKeyPressed('a') || isKeyPressed('A')) || isSpecialKeyPressed(GLUT_KEY_LEFT))  { playerSquareX -= classroom1MoveSpeed; playerFacing = 3; playerIsMoving = 1; }
        if ((isKeyPressed('d') || isKeyPressed('D')) || isSpecialKeyPressed(GLUT_KEY_RIGHT)) { playerSquareX += classroom1MoveSpeed; playerFacing = 2; playerIsMoving = 1; }
        {
            double classroom1FeetX = playerSquareX + 24.0, classroom1FeetY = (double)playerSquareY;
            resolveClassroom1Border(&classroom1FeetX, &classroom1FeetY);
            pushOutOfRect(&classroom1FeetX, &classroom1FeetY, MONT_BLOCK_X, MONT_BLOCK_Y, MONT_BLOCK_W, MONT_BLOCK_H);
            pushOutOfRect(&classroom1FeetX, &classroom1FeetY, NOPLA_BLOCK_X, NOPLA_BLOCK_Y, NOPLA_BLOCK_W, NOPLA_BLOCK_H);
            playerSquareX = (int)(classroom1FeetX - 24.0);
            playerSquareY = (int)classroom1FeetY;
        }
        if (playerSquareX < 6) playerSquareX = 6;
        if (playerSquareY < 60) playerSquareY = 60;
        if (playerSquareX > SCREEN_WIDTH - 54) playerSquareX = SCREEN_WIDTH - 54;
        if (playerSquareY > SCREEN_HEIGHT - 90) playerSquareY = SCREEN_HEIGHT - 90;
        // auto-advance: y >= 690 -> classroom2 (checked after clamps so it fires same tick)
        if (playerSquareY >= 690) {
            playClickSound();
            switchToClassroom2();
            return;
        }
        // one-way destiny: ESC cannot go back to scene 10 (blocked with popup)
        if (isKeyPressed(27)) { playClickSound(); showDialogue(NO_RETURN_MESSAGE); }
    } else if (currentGameState == GAME_STATE_CLASSROOM2) {
        if (dialogueTicksLeft > 0) dialogueTicksLeft--;
        // Rino beat: 1st X = board line, 2nd X = accept + fly his plane game
        if (isRinoInTalkRange() && isInteractKeyPressed()) {
            if (dialogueTicksLeft <= 0 && rinoTalked) {
                playClickSound();
                rinoTalked = 0; // one-shot per visit
                switchToPlaneGame();
                return;
            } else if (dialogueTicksLeft <= 0 && !rinoTalked) {
                playClickSound();
                showDialogue("Oh you're here, your game was the best in class. I tried to make one myself. Why don't you try?");
                rinoTalked = 1;
            }
        } else if (isClassCrInTalkRange() && isInteractKeyPressed()) {
            if (dialogueTicksLeft <= 0) {
                playClickSound();
                showDialogue("CR: Mama badamtola  te chol, bba er fest ase ");
            }
        }
        // classroom roam inside the scene-2 loop, no way back (ESC blocked with popup)
        int classroom2MoveSpeed = 4;
        playerIsMoving = 0;
        if ((isKeyPressed('w') || isKeyPressed('W')) || isSpecialKeyPressed(GLUT_KEY_UP))    { playerSquareY += classroom2MoveSpeed; playerFacing = 0; playerIsMoving = 1; }
        if ((isKeyPressed('s') || isKeyPressed('S')) || isSpecialKeyPressed(GLUT_KEY_DOWN))  { playerSquareY -= classroom2MoveSpeed; playerFacing = 1; playerIsMoving = 1; }
        if ((isKeyPressed('a') || isKeyPressed('A')) || isSpecialKeyPressed(GLUT_KEY_LEFT))  { playerSquareX -= classroom2MoveSpeed; playerFacing = 3; playerIsMoving = 1; }
        if ((isKeyPressed('d') || isKeyPressed('D')) || isSpecialKeyPressed(GLUT_KEY_RIGHT)) { playerSquareX += classroom2MoveSpeed; playerFacing = 2; playerIsMoving = 1; }
        {
            double classroom2FeetX = playerSquareX + 24.0, classroom2FeetY = (double)playerSquareY;
            resolveClassroom2Border(&classroom2FeetX, &classroom2FeetY);
            pushOutOfRect(&classroom2FeetX, &classroom2FeetY, CLASSCR_BLOCK_X, CLASSCR_BLOCK_Y, CLASSCR_BLOCK_W, CLASSCR_BLOCK_H);
            pushOutOfRect(&classroom2FeetX, &classroom2FeetY, RINO_BLOCK_X, RINO_BLOCK_Y, RINO_BLOCK_W, RINO_BLOCK_H);
            playerSquareX = (int)(classroom2FeetX - 24.0);
            playerSquareY = (int)classroom2FeetY;
        }
        if (playerSquareX < 6) playerSquareX = 6;
        if (playerSquareY < 60) playerSquareY = 60;
        if (playerSquareX > SCREEN_WIDTH - 54) playerSquareX = SCREEN_WIDTH - 54;
        if (playerSquareY > SCREEN_HEIGHT - 90) playerSquareY = SCREEN_HEIGHT - 90;
        // one-way destiny: ESC cannot go back (blocked with popup)
        if (isKeyPressed(27)) { playClickSound(); showDialogue(NO_RETURN_MESSAGE); }
    } else if (currentGameState == GAME_STATE_PLANEGAME) {
        // Rino's plane minigame: fully owned by PlaneGame.hpp (win -> Library, lose -> Rino).
        updatePlaneGame();
    } else if (currentGameState == GAME_STATE_LIBRARY) {
        if (dialogueTicksLeft > 0) dialogueTicksLeft--;
        // auto-advance: walk up to y=690 -> library2 (no X needed)
        // placeholder roam until library NPCs land, no way back (ESC blocked with popup)
        int libraryMoveSpeed = 4;
        playerIsMoving = 0;
        if ((isKeyPressed('w') || isKeyPressed('W')) || isSpecialKeyPressed(GLUT_KEY_UP))    { playerSquareY += libraryMoveSpeed; playerFacing = 0; playerIsMoving = 1; }
        if ((isKeyPressed('s') || isKeyPressed('S')) || isSpecialKeyPressed(GLUT_KEY_DOWN))  { playerSquareY -= libraryMoveSpeed; playerFacing = 1; playerIsMoving = 1; }
        if ((isKeyPressed('a') || isKeyPressed('A')) || isSpecialKeyPressed(GLUT_KEY_LEFT))  { playerSquareX -= libraryMoveSpeed; playerFacing = 3; playerIsMoving = 1; }
        if ((isKeyPressed('d') || isKeyPressed('D')) || isSpecialKeyPressed(GLUT_KEY_RIGHT)) { playerSquareX += libraryMoveSpeed; playerFacing = 2; playerIsMoving = 1; }
        {
            double libraryFeetX = playerSquareX + 24.0, libraryFeetY = (double)playerSquareY;
            resolveLibrary1Border(&libraryFeetX, &libraryFeetY);
            playerSquareX = (int)(libraryFeetX - 24.0);
            playerSquareY = (int)libraryFeetY;
        }
        if (playerSquareX < 6) playerSquareX = 6;
        if (playerSquareY < 60) playerSquareY = 60;
        if (playerSquareX > SCREEN_WIDTH - 54) playerSquareX = SCREEN_WIDTH - 54;
        if (playerSquareY > SCREEN_HEIGHT - 90) playerSquareY = SCREEN_HEIGHT - 90;
        // auto-advance: y >= 690 -> library2 (checked after clamps so it fires same tick)
        if (playerSquareY >= 690) {
            playClickSound();
            switchToLibrary2();
            return;
        }
        // one-way destiny: ESC cannot go back to classroom (blocked with popup)
        if (isKeyPressed(27)) { playClickSound(); showDialogue(NO_RETURN_MESSAGE); }
    } else if (currentGameState == GAME_STATE_LIBRARY2) {
        if (dialogueTicksLeft > 0) dialogueTicksLeft--;
        // book: PRESS X in range opens the black-screen quiz (file I/O)
        if (isLibrary2BookInRange() && isInteractKeyPressed()) {
            if (dialogueTicksLeft <= 0) {
                startBookQuiz();
                return;
            }
        }
        // next floor after the library: X at top line advances to the big gate
        if (exitCooldownTicks == 0 && isPlayerAtNextFloor() && isInteractKeyPressed()) {
            playClickSound();
            switchToBigGate();
            return;
        }
        // second-half roam, no way back (ESC blocked with popup)
        int library2MoveSpeed = 4;
        playerIsMoving = 0;
        if ((isKeyPressed('w') || isKeyPressed('W')) || isSpecialKeyPressed(GLUT_KEY_UP))    { playerSquareY += library2MoveSpeed; playerFacing = 0; playerIsMoving = 1; }
        if ((isKeyPressed('s') || isKeyPressed('S')) || isSpecialKeyPressed(GLUT_KEY_DOWN))  { playerSquareY -= library2MoveSpeed; playerFacing = 1; playerIsMoving = 1; }
        if ((isKeyPressed('a') || isKeyPressed('A')) || isSpecialKeyPressed(GLUT_KEY_LEFT))  { playerSquareX -= library2MoveSpeed; playerFacing = 3; playerIsMoving = 1; }
        if ((isKeyPressed('d') || isKeyPressed('D')) || isSpecialKeyPressed(GLUT_KEY_RIGHT)) { playerSquareX += library2MoveSpeed; playerFacing = 2; playerIsMoving = 1; }
        {
            double library2FeetX = playerSquareX + 24.0, library2FeetY = (double)playerSquareY;
            resolveLibrary2Border(&library2FeetX, &library2FeetY);
            playerSquareX = (int)(library2FeetX - 24.0);
            playerSquareY = (int)library2FeetY;
        }
        if (playerSquareX < 6) playerSquareX = 6;
        if (playerSquareY < 60) playerSquareY = 60;
        if (playerSquareX > SCREEN_WIDTH - 54) playerSquareX = SCREEN_WIDTH - 54;
        if (playerSquareY > SCREEN_HEIGHT - 90) playerSquareY = SCREEN_HEIGHT - 90;
        // one-way destiny: ESC cannot go back (blocked with popup)
        if (isKeyPressed(27)) { playClickSound(); showDialogue(NO_RETURN_MESSAGE); }
    } else if (currentGameState == GAME_STATE_BIGGATE) {
        if (dialogueTicksLeft > 0) dialogueTicksLeft--;
        // gate access: X at the step front center enters the boss room
        if (exitCooldownTicks == 0 && isPlayerAtGateExit() && isInteractKeyPressed()) {
            playClickSound();
            switchToBossRoom();
            return;
        }
        // courtyard roam inside the scanned gate loop, no way back (ESC blocked with popup)
        int bigGateMoveSpeed = 4;
        playerIsMoving = 0;
        if ((isKeyPressed('w') || isKeyPressed('W')) || isSpecialKeyPressed(GLUT_KEY_UP))    { playerSquareY += bigGateMoveSpeed; playerFacing = 0; playerIsMoving = 1; }
        if ((isKeyPressed('s') || isKeyPressed('S')) || isSpecialKeyPressed(GLUT_KEY_DOWN))  { playerSquareY -= bigGateMoveSpeed; playerFacing = 1; playerIsMoving = 1; }
        if ((isKeyPressed('a') || isKeyPressed('A')) || isSpecialKeyPressed(GLUT_KEY_LEFT))  { playerSquareX -= bigGateMoveSpeed; playerFacing = 3; playerIsMoving = 1; }
        if ((isKeyPressed('d') || isKeyPressed('D')) || isSpecialKeyPressed(GLUT_KEY_RIGHT)) { playerSquareX += bigGateMoveSpeed; playerFacing = 2; playerIsMoving = 1; }
        {
            double bigGateFeetX = playerSquareX + 24.0, bigGateFeetY = (double)playerSquareY;
            resolveBigGateBorder(&bigGateFeetX, &bigGateFeetY);
            playerSquareX = (int)(bigGateFeetX - 24.0);
            playerSquareY = (int)bigGateFeetY;
        }
        if (playerSquareX < 6) playerSquareX = 6;
        if (playerSquareY < 60) playerSquareY = 60;
        if (playerSquareX > SCREEN_WIDTH - 54) playerSquareX = SCREEN_WIDTH - 54;
        if (playerSquareY > SCREEN_HEIGHT - 90) playerSquareY = SCREEN_HEIGHT - 90;
        // one-way destiny: ESC cannot go back to the library (blocked with popup)
        if (isKeyPressed(27)) { playClickSound(); showDialogue(NO_RETURN_MESSAGE); }
    } else if (currentGameState == GAME_STATE_BOSSROOM) {
        if (dialogueTicksLeft > 0) dialogueTicksLeft--;
        // throne challenge: PRESS X in range hears his line, battle starts once it fades
        if (!bossChallenged && isBossInTalkRange() && isInteractKeyPressed()) {
            if (dialogueTicksLeft <= 0) {
                playClickSound();
                showDialogue("So you're finally here. We meet again, although you dont know. Defeat me and find your answers");
                bossChallenged = 1;
            }
        }
        // throne-hall roam inside the scanned aisle loop, no way back (ESC blocked with popup)
        int bossRoomMoveSpeed = 4;
        playerIsMoving = 0;
        if ((isKeyPressed('w') || isKeyPressed('W')) || isSpecialKeyPressed(GLUT_KEY_UP))    { playerSquareY += bossRoomMoveSpeed; playerFacing = 0; playerIsMoving = 1; }
        if ((isKeyPressed('s') || isKeyPressed('S')) || isSpecialKeyPressed(GLUT_KEY_DOWN))  { playerSquareY -= bossRoomMoveSpeed; playerFacing = 1; playerIsMoving = 1; }
        if ((isKeyPressed('a') || isKeyPressed('A')) || isSpecialKeyPressed(GLUT_KEY_LEFT))  { playerSquareX -= bossRoomMoveSpeed; playerFacing = 3; playerIsMoving = 1; }
        if ((isKeyPressed('d') || isKeyPressed('D')) || isSpecialKeyPressed(GLUT_KEY_RIGHT)) { playerSquareX += bossRoomMoveSpeed; playerFacing = 2; playerIsMoving = 1; }
        {
            double bossRoomFeetX = playerSquareX + 24.0, bossRoomFeetY = (double)playerSquareY;
            resolveBossRoomBorder(&bossRoomFeetX, &bossRoomFeetY);
            playerSquareX = (int)(bossRoomFeetX - 24.0);
            playerSquareY = (int)bossRoomFeetY;
        }
        if (playerSquareX < 6) playerSquareX = 6;
        if (playerSquareY < 60) playerSquareY = 60;
        if (playerSquareX > SCREEN_WIDTH - 54) playerSquareX = SCREEN_WIDTH - 54;
        if (playerSquareY > SCREEN_HEIGHT - 90) playerSquareY = SCREEN_HEIGHT - 90;
        // challenged + line faded -> trial transition into the last brawl
        if (bossChallenged && dialogueTicksLeft <= 0) {
            playClickSound();
            startLastBrawl();
            return;
        }
        // one-way destiny: ESC cannot go back to the gate (blocked with popup)
        if (isKeyPressed(27)) { playClickSound(); showDialogue(NO_RETURN_MESSAGE); }
    } else if (currentGameState == GAME_STATE_SPLASH) {
        // black-screen sword splash, then the finale battle (ESC blocked with popup)
        if (splashTicksLeft > 0) splashTicksLeft--;
        if (splashTicksLeft <= 0) {
            playClickSound();
            switchToBattle();
        }
        // one-way destiny: ESC cannot go back to scene 9 (blocked with popup)
        if (isKeyPressed(27)) { playClickSound(); showDialogue(NO_RETURN_MESSAGE); }
    } else if (currentGameState == GAME_STATE_BATTLE) {
        if (dialogueTicksLeft > 0) dialogueTicksLeft--;
        if (battlePhase == PHASE_OVER) {
            // win -> victory roam, lose -> fresh rematch (both need the banner read first)
            if (dialogueTicksLeft <= 0 && isInteractKeyPressed()) {
                playClickSound();
                if (battleResult == 1) enterVictory();
                else { resetBattle(); showDialogue("REMATCH! CR CRACKS HIS KNUCKLES."); }
            }
        } else if (battlePhase == PHASE_INPUT) {
            // key-hold safe: choosing flips phase the same tick
            if (isKeyPressed('1')) battleChooseMove(MOVE_SLASH);
            else if (isKeyPressed('2')) battleChooseMove(MOVE_SMITE);
            else if (isKeyPressed('3')) battleChooseMove(MOVE_HEAL);
            else if (isKeyPressed('4')) battleChooseMove(MOVE_SHIELD);
        } else if (battlePhase == PHASE_PLAYER_ANIM) {
            battleAnimTick++;
            if (battleAnimTick >= MOVE_FRAMES[battleSelectedMove] * BATTLE_ANIM_TICKS) applyPlayerMove();
        } else if (battlePhase == PHASE_ENEMY_ANIM) {
            battleAnimTick++;
            if (battleAnimTick >= ATK_FRAMES * 8) applyEnemyMove();
        }
    } else if (currentGameState == GAME_STATE_VICTORY) {
        if (dialogueTicksLeft > 0) dialogueTicksLeft--;
        // right edge: X continues to scene 10 (one-way roam, ESC blocked with popup)
        if (exitCooldownTicks == 0 && isPlayerAtVictoryExit() && isInteractKeyPressed()) {
            playClickSound();
            switchToScene10();
            return;
        }
        int victoryMoveSpeed = 4;
        playerIsMoving = 0;
        if ((isKeyPressed('w') || isKeyPressed('W')) || isSpecialKeyPressed(GLUT_KEY_UP))    { playerSquareY += victoryMoveSpeed; playerFacing = 0; playerIsMoving = 1; }
        if ((isKeyPressed('s') || isKeyPressed('S')) || isSpecialKeyPressed(GLUT_KEY_DOWN))  { playerSquareY -= victoryMoveSpeed; playerFacing = 1; playerIsMoving = 1; }
        if ((isKeyPressed('a') || isKeyPressed('A')) || isSpecialKeyPressed(GLUT_KEY_LEFT))  { playerSquareX -= victoryMoveSpeed; playerFacing = 3; playerIsMoving = 1; }
        if ((isKeyPressed('d') || isKeyPressed('D')) || isSpecialKeyPressed(GLUT_KEY_RIGHT)) { playerSquareX += victoryMoveSpeed; playerFacing = 2; playerIsMoving = 1; }
        {
            double victoryFeetX = playerSquareX + 24.0, victoryFeetY = (double)playerSquareY;
            resolveVictoryBorder(&victoryFeetX, &victoryFeetY);
            playerSquareX = (int)(victoryFeetX - 24.0);
            playerSquareY = (int)victoryFeetY;
        }
        if (playerSquareX < 6) playerSquareX = 6;
        if (playerSquareY < 60) playerSquareY = 60;
        if (playerSquareX > SCREEN_WIDTH - 54) playerSquareX = SCREEN_WIDTH - 54;
        if (playerSquareY > SCREEN_HEIGHT - 90) playerSquareY = SCREEN_HEIGHT - 90;
        // one-way destiny: ESC cannot go back (blocked with popup)
        if (isKeyPressed(27)) { playClickSound(); showDialogue(NO_RETURN_MESSAGE); }
    } else if (currentGameState == GAME_STATE_BOOKQUIZ) {
        // quiz runs on fixedUpdate ticks (not mouse events): A/B keys + feedback timing
        if (isKeyPressed(27)) { // ESC backs out to library2
            playClickSound();
            exitCooldownTicks = EXIT_COOLDOWN_TICKS;
            currentGameState = GAME_STATE_LIBRARY2;
            return;
        }
        if (bookQuizFeedbackTicks > 0) {
            bookQuizFeedbackTicks--;
            if (bookQuizFeedbackTicks <= 0) {
                if (bookQuizPending == 1) { bookQuizIndex++; bookQuizMessage[0] = 0; }
                else if (bookQuizPending == 2) { bookQuizIndex = 0; sprintf_s(bookQuizMessage, "Restarting from Q1..."); }
                else if (bookQuizPending == 3) { switchToBigGate(); return; } // all correct -> next scene
                bookQuizPending = 0;
            }
            return;
        }
        if (bookQuizTotal <= 0) return; // files missing: ESC only
        {
            static int lastADown = 0, lastBDown = 0;
            int aDown = isKeyPressed('a') || isKeyPressed('A');
            int bDown = isKeyPressed('b') || isKeyPressed('B');
            if (aDown && !lastADown) answerBookQuiz('A');
            else if (bDown && !lastBDown) answerBookQuiz('B');
            lastADown = aDown; lastBDown = bDown;
        }
    } else if (currentGameState == GAME_STATE_LASTBRAWL) {
        // ==== LASTBRAWL revertable block 4/4b: fight loop (remove to revert) ====
        if (dialogueTicksLeft > 0) dialogueTicksLeft--;
        if (brawlSwordTick > 0) { brawlSwordTick++; if (brawlSwordTick > BRAWL_SWORD_FRAMES * BRAWL_SWORD_TICKS) brawlSwordTick = 0; }
        if (brawlNovaTick > 0) { brawlNovaTick++; if (brawlNovaTick > (HOLY_NOVA_FRAMES - 1) * BRAWL_NOVA_TICKS) brawlNovaTick = 0; }
        if (brawlOver == 3) {
            // boss dying: dissolve plays, input locked, then the black ending screen
            brawlDieTick++;
            if (brawlDieTick >= BRAWL_DIE_TICKS) {
                brawlDieTick = 0;
                exitCooldownTicks = EXIT_COOLDOWN_TICKS;
                currentGameState = GAME_STATE_ENDING;
                return;
            }
        }
        else if (brawlOver == 0) {
            if (brawlHeroCooldown > 0) brawlHeroCooldown--;
            if (brawlBossCooldown > 0) brawlBossCooldown--;
            // auto-face: both fighters track each other's side (pass-through allowed)
            brawlHeroFaceR = (brawlBossX >= brawlHeroX) ? 1 : 0;
            brawlBossFaceL = (brawlHeroX <= brawlBossX) ? 1 : 0;
            playerFacing = brawlHeroFaceR ? 2 : 3;
            // hero: A/D or arrows, free pass-through (arena bounds only, hit logic unchanged)
            playerIsMoving = 0;
            if ((isKeyPressed('a') || isKeyPressed('A')) || isSpecialKeyPressed(GLUT_KEY_LEFT)) { brawlHeroX -= 4; playerIsMoving = 1; }
            if ((isKeyPressed('d') || isKeyPressed('D')) || isSpecialKeyPressed(GLUT_KEY_RIGHT)) { brawlHeroX += 4; playerIsMoving = 1; }
            if (brawlHeroX < BRAWL_MIN_X) brawlHeroX = BRAWL_MIN_X;
            if (brawlHeroX > BRAWL_MAX_X) brawlHeroX = BRAWL_MAX_X;
            // hero strike: SPACE edge-triggered, holy sword lands at half-swing if in range
            {
                static int lastSpaceDown = 0;
                int spaceDown = isKeyPressed(' ');
                if (spaceDown && !lastSpaceDown && brawlHeroCooldown == 0 && brawlHeroAtkTick == 0) {
                    playClickSound();
                    brawlHeroAtkTick = 1; brawlHeroCooldown = BRAWL_HERO_COOLDOWN;
                }
                lastSpaceDown = spaceDown;
            }
            if (brawlHeroAtkTick > 0) {
                brawlHeroAtkTick++;
                int brawlGap = brawlBossX - brawlHeroX; if (brawlGap < 0) brawlGap = -brawlGap;
                if (brawlHeroAtkTick == BRAWL_ATK_ANIM / 2 && brawlGap <= BRAWL_ATK_RANGE) {
                    brawlSwordTick = 1;
                    brawlBossHp++;
                    if (brawlBossHp >= HERO_HP_STAGES - 1) { brawlBossHp = HERO_HP_STAGES - 1; brawlOver = 3; brawlDieTick = 1; brawlNovaTick = 1; exitCooldownTicks = EXIT_COOLDOWN_TICKS; }
                }
                if (brawlHeroAtkTick >= BRAWL_ATK_ANIM) brawlHeroAtkTick = 0;
            }
            // boss AI: close in from either side, swipe in range, holy nova bursts on the hero
            if (brawlOver == 0) {
                int brawlChase = brawlHeroX - brawlBossX;
                int brawlDist = (brawlChase < 0) ? -brawlChase : brawlChase;
                if (brawlDist > BRAWL_ATK_RANGE) {
                    if (brawlBossAtkTick == 0) brawlBossX += (brawlChase > 0) ? 2 : -2;
                    if (brawlBossX < BRAWL_MIN_X) brawlBossX = BRAWL_MIN_X;
                    if (brawlBossX > BRAWL_MAX_X) brawlBossX = BRAWL_MAX_X;
                } else if (brawlBossCooldown == 0 && brawlBossAtkTick == 0) {
                    playClickSound();
                    brawlBossAtkTick = 1; brawlBossCooldown = BRAWL_BOSS_COOLDOWN;
                }
            }
            if (brawlBossAtkTick > 0) {
                brawlBossAtkTick++;
                int brawlHitGap = brawlBossX - brawlHeroX; if (brawlHitGap < 0) brawlHitGap = -brawlHitGap;
                if (brawlBossAtkTick == BRAWL_ATK_ANIM / 2 && brawlHitGap <= BRAWL_ATK_RANGE) {
                    brawlNovaTick = 1;
                    brawlHeroHp++;
                    if (brawlHeroHp >= HERO_HP_STAGES - 1) { brawlHeroHp = HERO_HP_STAGES - 1; brawlOver = 2; exitCooldownTicks = EXIT_COOLDOWN_TICKS; }
                }
                if (brawlBossAtkTick >= BRAWL_ATK_ANIM) brawlBossAtkTick = 0;
            }
        } else if (brawlOver == 2 && exitCooldownTicks == 0 && isInteractKeyPressed()) {
            // lost -> first scene with the defeat transition (win path auto-goes to ENDING)
            playClickSound();
            switchToPlayScene(); startInterstitial("You've lost yet again", GAME_STATE_PLAY);
            return;
        }
        // ==== end LASTBRAWL block 4/4b ====
    } else if (currentGameState == GAME_STATE_ENDING) {
        // ==== ENDING revertable block 3/3b: X returns to the menu (remove to revert) ====
        if (exitCooldownTicks == 0 && isInteractKeyPressed()) {
            playClickSound();
            returnToMainMenu();
            return;
        }
        // ==== end ENDING block 3/3b ====
    } else if (currentGameState == GAME_STATE_INTERSTITIAL) {
        // black-screen hold, then land on the pending scene
        if (interstitialTicksLeft > 0) interstitialTicksLeft--;
        if (interstitialTicksLeft <= 0 && interstitialNextState >= 0) {
            playClickSound();
            finishInterstitial();
        }
    }
}


// Asset loader: every image sits under Assets// - change the root only here.
unsigned int loadAsset(const char* relPath) {
    char full[512];
    sprintf_s(full, "Assets//%s", relPath);
    return iLoadImage(full);
}

int main() {
    // Audio — open once at boot, // separators (Guide §3.1)
    // All tracks are MP3.
    openAudioAlias("Assets//Audios//ForMainMenu.mp3", "menumusic");
    openAudioAlias("Assets//Audios//mp3forClicks.mp3", "clicksfx");
    openAudioAlias("Assets//Audios//AfterCheckpoint1.mp3", "aftercheckpoint");
    openAudioAlias("Assets//Audios//RBD.mp3", "rbd"); // interstitial sting for every transition
    startMusicLoop("menumusic");

    layoutButtons();

    iInitialize(SCREEN_WIDTH, SCREEN_HEIGHT, "Tales Of Austia");

    // Load AFTER iInitialize (needs GL context — Reference §2)
    menuBackgroundTexture  = loadAsset("Main//Main_screen_background.jpg");
    aboutBackgroundTexture = loadAsset("Main//about.jpg");
    playBackgroundTexture  = loadAsset("Main//firstScene.jpg");
    scene2BackgroundTexture = loadAsset("Main//floor.jpg");
    scene3BackgroundTexture = loadAsset("Main//floor.jpg"); // same map bg as scene 2
    scene4BackgroundTexture = loadAsset("Main//scene4.jpg"); // hall art
    scene6BackgroundTexture = loadAsset("Main//doorOpen.jpg"); // open-door map
    scene7BackgroundTexture = loadAsset("Main//maze.jpg"); // quiz maze
    bossBackgroundTexture = loadAsset("Main//bossForC2.jpg"); // finale battle map
    classroom1Texture = loadAsset("afterScene10//classroom//classroom1.jpg"); // first half
    classroom2Texture = loadAsset("afterScene10//classroom//classroom2.jpg"); // second half
    montTexture = loadAsset("afterScene10//classroom//3_transparent.png"); // Mont
    noplaTexture = loadAsset("afterScene10//classroom//2_transparent.png"); // Nopla
    classCrTexture = loadAsset("afterScene10//classroom//1_transparent.png"); // classroom CR
    rinoTexture = loadAsset("afterScene10//classroom//Rino.png"); // Rino near board
    library1Texture = loadAsset("afterScene10//classroom//library1.jpg"); // first half
    library2Texture = loadAsset("afterScene10//classroom//library2.jpg"); // second half
    library2BookTexture = loadAsset("afterScene10//classroom//image.png"); // demonic book for library2
    planeBgTexture = loadAsset("planeGame//background.png"); // Rino's game sky
    planeTexture = loadAsset("planeGame//plane.png"); // Rino's game flyer
    planeTowerTexture = loadAsset("planeGame//tower.png"); // Rino's game tower column
    planeHeartTexture = loadAsset("planeGame//hp_heart.png"); // Rino's game HP heart
    bigGateTexture = loadAsset("afterScene10//bigGate.png"); // gate courtyard
    bossRoomTexture = loadAsset("afterScene10//bossRoom1.png"); // throne hall
    bossThroneTexture = loadAsset("afterScene10//boss//darkBoss_sprites_transparent//idle_throne//idle_throne_S.png"); // seated boss
    // ==== LASTBRAWL revertable block 2/4: sprite loads (remove to revert) ====
    heroBrawlAtkTexture = loadAsset("mainCharacter_sprites//aimed_shot_E_c6_y36-218_x953.png"); // hero strike, faces right
    heroBrawlAtkWTexture = loadAsset("mainCharacter_sprites//aimed_shot_MR.png"); // mirrored E: true left-facing strike
    bossBrawlStanceWTexture = loadAsset("afterScene10//boss//darkBoss_sprites_transparent//special_abyssalCage//special_cape_stance_W.png"); // true left-facing idle
    bossBrawlStanceETexture = loadAsset("afterScene10//boss//darkBoss_sprites_transparent//special_abyssalCage//special_cape_stance_MR.png"); // mirrored W: true right-facing idle
    bossBrawlAtkTexture = loadAsset("afterScene10//boss//darkBoss_sprites_transparent//attack_shadowClaw//attack_shadowClaw_W.png"); // boss swipe
    bossBrawlClawETexture = loadAsset("afterScene10//boss//darkBoss_sprites_transparent//attack_shadowClaw//attack_shadowClaw_MR.png"); // mirrored W: true rightward swipe
    // ==== BRAWLBAR revertable block 2/2: staged bar loads (remove to revert) ====
    bossBarTexture[0] = loadAsset("DragonHpBar_parts//bossbar_png//FirstForm//bossbar_100_full.png");
    bossBarTexture[1] = loadAsset("DragonHpBar_parts//bossbar_png//FirstForm//bossbar_70_high.png");
    bossBarTexture[2] = loadAsset("DragonHpBar_parts//bossbar_png//FirstForm//bossbar_40_mid.png");
    bossBarTexture[3] = loadAsset("DragonHpBar_parts//bossbar_png//FirstForm//bossbar_35_mid.png");
    bossBarTexture[4] = loadAsset("DragonHpBar_parts//bossbar_png//FirstForm//bossbar_15_low.png");
    bossBarTexture[5] = loadAsset("DragonHpBar_parts//bossbar_png//FirstForm//bossbar_00_empty.png");
    // ==== end BRAWLBAR block 2/2 ====
    // ==== ENDING revertable block 2/3: death sprite load (remove to revert) ====
    bossBrawlDieTexture = loadAsset("afterScene10//boss//darkBoss_sprites_transparent//dodge//dodge_W.png"); // shadow-dissolve death
    // ==== end ENDING block 2/3 ====
    // ==== end LASTBRAWL block 2/4 ====
    crCngTexture[0] = loadAsset("Cr cng//crCng1.png");
    crCngTexture[1] = loadAsset("Cr cng//crCng2.png");
    crCngTexture[2] = loadAsset("Cr cng//crCng3.png");
    crCngTexture[3] = loadAsset("Cr cng//crCng4.png");
    crAtkTexture[0] = loadAsset("cr atk//crAtk1.png");
    crAtkTexture[1] = loadAsset("cr atk//crAtk2.png");
    crAtkTexture[2] = loadAsset("cr atk//crAtk3.png");
    crAtkTexture[3] = loadAsset("cr atk//crAtk4.png");
    crAtkTexture[4] = loadAsset("cr atk//crAtk5.png");
    swordTexture[0] = loadAsset("SwordOfJustice//Frames//SwordOfJustice_01.png");
    swordTexture[1] = loadAsset("SwordOfJustice//Frames//SwordOfJustice_02.png");
    swordTexture[2] = loadAsset("SwordOfJustice//Frames//SwordOfJustice_03.png");
    swordTexture[3] = loadAsset("SwordOfJustice//Frames//SwordOfJustice_04.png");
    swordTexture[4] = loadAsset("SwordOfJustice//Frames//SwordOfJustice_05.png");
    swordTexture[5] = loadAsset("SwordOfJustice//Frames//SwordOfJustice_06.png");
    swordTexture[6] = loadAsset("SwordOfJustice//Frames//SwordOfJustice_07.png");
    swordTexture[7] = loadAsset("SwordOfJustice//Frames//SwordOfJustice_08.png");
    swordTexture[8] = loadAsset("SwordOfJustice//Frames//SwordOfJustice_09.png");
    swordTexture[9] = loadAsset("SwordOfJustice//Frames//SwordOfJustice_10.png");
    swordTexture[10] = loadAsset("SwordOfJustice//Frames//SwordOfJustice_11.png");
    swordTexture[11] = loadAsset("SwordOfJustice//Frames//SwordOfJustice_12.png");
    swordTexture[12] = loadAsset("SwordOfJustice//Frames//SwordOfJustice_13.png");
    moveSlashTex[0] = loadAsset("HolySlash_A//Frames//HolySlash_A_01.png");
    moveSlashTex[1] = loadAsset("HolySlash_A//Frames//HolySlash_A_02.png");
    moveSlashTex[2] = loadAsset("HolySlash_A//Frames//HolySlash_A_03.png");
    moveSlashTex[3] = loadAsset("HolySlash_A//Frames//HolySlash_A_04.png");
    moveSlashTex[4] = loadAsset("HolySlash_A//Frames//HolySlash_A_05.png");
    moveSmiteTex[0] = loadAsset("Smite//Frames//Smite_01.png");
    moveSmiteTex[1] = loadAsset("Smite//Frames//Smite_02.png");
    moveSmiteTex[2] = loadAsset("Smite//Frames//Smite_03.png");
    moveSmiteTex[3] = loadAsset("Smite//Frames//Smite_04.png");
    moveSmiteTex[4] = loadAsset("Smite//Frames//Smite_05.png");
    moveSmiteTex[5] = loadAsset("Smite//Frames//Smite_06.png");
    moveSmiteTex[6] = loadAsset("Smite//Frames//Smite_07.png");
    moveSmiteTex[7] = loadAsset("Smite//Frames//Smite_08.png");
    moveSmiteTex[8] = loadAsset("Smite//Frames//Smite_09.png");
    moveSmiteTex[9] = loadAsset("Smite//Frames//Smite_10.png");
    moveSmiteTex[10] = loadAsset("Smite//Frames//Smite_11.png");
    moveHealTex[0] = loadAsset("Heal//Frames//Heal_01.png");
    moveHealTex[1] = loadAsset("Heal//Frames//Heal_02.png");
    moveHealTex[2] = loadAsset("Heal//Frames//Heal_03.png");
    moveHealTex[3] = loadAsset("Heal//Frames//Heal_04.png");
    moveHealTex[4] = loadAsset("Heal//Frames//Heal_05.png");
    moveHealTex[5] = loadAsset("Heal//Frames//Heal_06.png");
    moveHealTex[6] = loadAsset("Heal//Frames//Heal_07.png");
    moveHealTex[7] = loadAsset("Heal//Frames//Heal_08.png");
    moveHealTex[8] = loadAsset("Heal//Frames//Heal_09.png");
    moveHealTex[9] = loadAsset("Heal//Frames//Heal_10.png");
    moveHealTex[10] = loadAsset("Heal//Frames//Heal_11.png");
    moveHealTex[11] = loadAsset("Heal//Frames//Heal_12.png");
    moveShieldTex[0] = loadAsset("HolyShield//Frames//HolyShield_01.png");
    moveShieldTex[1] = loadAsset("HolyShield//Frames//HolyShield_02.png");
    moveShieldTex[2] = loadAsset("HolyShield//Frames//HolyShield_03.png");
    moveShieldTex[3] = loadAsset("HolyShield//Frames//HolyShield_04.png");
    moveShieldTex[4] = loadAsset("HolyShield//Frames//HolyShield_05.png");
    moveShieldTex[5] = loadAsset("HolyShield//Frames//HolyShield_06.png");
    moveShieldTex[6] = loadAsset("HolyShield//Frames//HolyShield_07.png");
    moveShieldTex[7] = loadAsset("HolyShield//Frames//HolyShield_08.png");
    moveShieldTex[8] = loadAsset("HolyShield//Frames//HolyShield_09.png");
    moveShieldTex[9] = loadAsset("HolyShield//Frames//HolyShield_10.png");
    moveShieldTex[10] = loadAsset("HolyShield//Frames//HolyShield_11.png");
    castAnimationTexture = loadAsset("mainCharacter_sprites//castSpecialAnimation.png"); // hero-sized cast
    holyNovaTexture[0] = loadAsset("Frames//HolyNova_01.png"); // falling head
    holyNovaTexture[1] = loadAsset("Frames//HolyNova_02.png");
    holyNovaTexture[2] = loadAsset("Frames//HolyNova_03.png");
    holyNovaTexture[3] = loadAsset("Frames//HolyNova_04.png");
    holyNovaTexture[4] = loadAsset("Frames//HolyNova_05.png");
    holyNovaTexture[5] = loadAsset("Frames//HolyNova_06.png");
    holyNovaTexture[6] = loadAsset("Frames//HolyNova_07.png");
    holyNovaTexture[7] = loadAsset("Frames//HolyNova_09.png"); // 08 missing on disk
    holyNovaTexture[8] = loadAsset("Frames//HolyNova_10.png");
    dragonHpTexture[0] = loadAsset("DragonHpBar_parts//dragon_part_1.png"); // full
    dragonHpTexture[1] = loadAsset("DragonHpBar_parts//dragon_part_2.png");
    dragonHpTexture[2] = loadAsset("DragonHpBar_parts//dragon_part_3.png");
    dragonHpTexture[3] = loadAsset("DragonHpBar_parts//dragon_part_4.png");
    dragonHpTexture[4] = loadAsset("DragonHpBar_parts//dragon_part_5.png");
    dragonHpTexture[5] = loadAsset("DragonHpBar_parts//dragon_part_6.png");
    dragonHpTexture[6] = loadAsset("DragonHpBar_parts//dragon_part_7.png");
    dragonHpTexture[7] = loadAsset("DragonHpBar_parts//dragon_part_8.png"); // empty
    srand((unsigned)time(0)); // random meteor spawn positions
    heroTexture = loadAsset("Cr//Cr (1).png"); // fallback only
    xKeycapTexture = loadAsset("fonts//buttons//letters//04_07_X.png"); // X talk-prompt keycap
    crIdleTexture[0] = loadAsset("Cr//Cr (1).png");
    crIdleTexture[1] = loadAsset("Cr//Cr (2).png");
    crIdleTexture[2] = loadAsset("Cr//Cr (3).png");
    crIdleTexture[3] = loadAsset("Cr//Cr (4).png");
    crIdleTexture[4] = loadAsset("Cr//Cr (5).png");
    crIdleTexture[5] = loadAsset("Cr//Cr (6).png");
    crIdleTexture[6] = loadAsset("Cr//Cr (7).png");
    crIdleTexture[7] = loadAsset("Cr//Cr (8).png");
    seniorIdleTexture[0] = loadAsset("senior idle//senior1.png");
    seniorIdleTexture[1] = loadAsset("senior idle//senior2.png");
    seniorIdleTexture[2] = loadAsset("senior idle//senior3.png");
    seniorIdleTexture[3] = loadAsset("senior idle//senior4.png");
    seniorIdleTexture[4] = loadAsset("senior idle//senior5.png");
    seniorIdleTexture[5] = loadAsset("senior idle//senior6.png");
    // mainCharacter_sprites: idle/walk + direction (0=up 1=down 2=right 3=left)
    heroIdle[0] = loadAsset("mainCharacter_sprites//idle_up.png");
    heroIdle[1] = loadAsset("mainCharacter_sprites//idle_down.png");
    heroIdle[2] = loadAsset("mainCharacter_sprites//idle_right.png");
    heroIdle[3] = loadAsset("mainCharacter_sprites//idle_left.png");
    heroWalkA[0] = loadAsset("mainCharacter_sprites//walk_up_1.png");
    heroWalkA[1] = loadAsset("mainCharacter_sprites//walk_down_1.png");
    heroWalkA[2] = loadAsset("mainCharacter_sprites//walk_right_1.png");
    heroWalkA[3] = loadAsset("mainCharacter_sprites//walk_left_1.png");
    heroWalkB[0] = loadAsset("mainCharacter_sprites//walk_up_2.png");
    heroWalkB[1] = loadAsset("mainCharacter_sprites//walk_down_2.png");
    heroWalkB[2] = loadAsset("mainCharacter_sprites//walk_right_2.png");
    heroWalkB[3] = loadAsset("mainCharacter_sprites//walk_left_2.png");
    if (!menuBackgroundTexture) {
        unsigned int fallbackBackgroundTexture = loadAsset("CAN BE CHANGED//mainground.jpg");
        if (fallbackBackgroundTexture) menuBackgroundTexture = fallbackBackgroundTexture;
    }

    // UI kit from ui/ folder
    menuPanelTexture       = loadAsset("ui//panel_large_hq.png");
    buttonBarNormalTexture = loadAsset("ui//bar_long_dark.png");
    buttonBarHoverTexture  = loadAsset("ui//bar_long_light.png");
    buttonCornerTextures[0] = loadAsset("ui//border (1).png");
    buttonCornerTextures[1] = loadAsset("ui//border (2).png");
    buttonCornerTextures[2] = loadAsset("ui//border (3).png");
    buttonCornerTextures[3] = loadAsset("ui//border (4).png");

    iStart();
    return 0;
}

