// ------------------------------------------------------------------------
// Scenes.hpp  -  one renderer per state.
// drawSceneMenu .. drawSceneEnding, picked by iDraw on currentGameState.
// Draw only; input lives in iMouse/fixedUpdate (iMain.cpp) and the flow
// decisions in SceneFlow.hpp.
// ------------------------------------------------------------------------
#pragma once

// -------------------------------------------------------------
// drawScene helpers — one per state (Guide §3.2: drawScene* per state)
// -------------------------------------------------------------
void drawSceneMenu() {
    if (menuBackgroundTexture) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, menuBackgroundTexture);
    else { iSetColor(18, 18, 24); iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); }

    // Main card from ui/panel_large_hq.png — centered (JPG art is the title)
    if (menuPanelTexture) {
        iShowImage(MENU_PANEL_LEFT, MENU_PANEL_BOTTOM, MENU_PANEL_WIDTH, MENU_PANEL_HEIGHT, menuPanelTexture);
    } else {
        // solid fallback only when ui panel missing
        int panelCenterX = MENU_PANEL_CENTER_X;
        iSetColor(32, 28, 24);
        iFilledRectangle(panelCenterX - 170, 200, 340, 300);
        iSetColor(95, 80, 45);
        iRectangle(panelCenterX - 170, 200, 340, 300);
    }

    drawButton(playButton);
    drawButton(aboutButton);
    drawButton(musicButton);
    drawButton(exitButton);
}

void drawSceneAbout() {
    // Show the about.jpg artwork full-screen as-is (no parchment panel covering it)
    if (aboutBackgroundTexture) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, aboutBackgroundTexture);
    else { iSetColor(20, 20, 26); iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); }

    // contrast strip behind BACK — padded 2px around the 140x38 button
    iSetColor(20, 20, 24);
    iFilledRectangle(backButton.left - 2, backButton.bottom - 2, backButton.width + 4, backButton.height + 4);
    iSetColor(90, 80, 45);
    iRectangle(backButton.left - 2, backButton.bottom - 2, backButton.width + 4, backButton.height + 4);

    drawButton(backButton);

    // ui footer strip on about screen — thin light bar at bottom
    if (buttonBarNormalTexture) iShowImage(0, 0, SCREEN_WIDTH, 19, buttonBarNormalTexture);
}

void drawScenePlay() {
    if (playBackgroundTexture) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, playBackgroundTexture);
    else { iSetColor(30, 30, 36); iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); }

    // HUD bands — solid backing + ui bar strip pinned to its edge
    iSetColor(28, 28, 32);
    iFilledRectangle(0, SCREEN_HEIGHT - 70, SCREEN_WIDTH, 70);
    iFilledRectangle(0, 0, SCREEN_WIDTH, 54);
    if (buttonBarNormalTexture) {
        iShowImage(0, SCREEN_HEIGHT - 70, SCREEN_WIDTH, 19, buttonBarNormalTexture);
        iShowImage(0, 35, SCREEN_WIDTH, 19, buttonBarNormalTexture);
    } else {
        iSetColor(90, 80, 45);
        iFilledRectangle(0, SCREEN_HEIGHT - 70, SCREEN_WIDTH, 2);
        iFilledRectangle(0, 54, SCREEN_WIDTH, 2);
    }

    iSetColor(210, 210, 215);
    drawCenteredText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 62, (char*)"WASD / Arrows to move  |  ESC for Menu | B = borders", GLUT_BITMAP_8_BY_13);

    // collision border debug outline, gray (set showCollisionBorders 0 to hide)
    if (showCollisionBorders) {
        iSetColor(150, 150, 155);
        drawLoopOutline(collisionLoopX, collisionLoopY, COLLISION_LOOP_POINTS);
    }

    drawHero();

    iSetColor(220, 220, 220);
    char playerPositionText[64];
    sprintf_s(playerPositionText, "Pos: (%d , %d)", playerSquareX, playerSquareY);
    iText(12, 22, playerPositionText, GLUT_BITMAP_8_BY_13);

    // No markers or prompts: the book art itself is the cue. Walk onto it + X to pick up.
    if (dialogueTicksLeft > 0) drawPromptBox(dialogueText);
    else if (isPlayerAtGateDoor()) drawPromptBox("PRESS X TO ENTER");

    drawButton(backButton);
}

void drawSceneScene2() {
    if (scene2BackgroundTexture) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, scene2BackgroundTexture);
    else { iSetColor(30, 30, 36); iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); }

    iSetColor(28, 28, 32);
    iFilledRectangle(0, SCREEN_HEIGHT - 70, SCREEN_WIDTH, 70);
    iFilledRectangle(0, 0, SCREEN_WIDTH, 54);
    iSetColor(210, 210, 215);
    drawCenteredText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 62, (char*)"You showed the ID card and passed the gate  |  No way back | B = borders", GLUT_BITMAP_8_BY_13);

    // scene-2 collision outline, gray (set showCollisionBorders 0 to hide)
    if (showCollisionBorders) {
        iSetColor(150, 150, 155);
        drawLoopOutline(scene2LoopX, scene2LoopY, SCENE2_LOOP_POINTS);
        iRectangle(SENIOR_BLOCK_X, SENIOR_BLOCK_Y, SENIOR_BLOCK_W, SENIOR_BLOCK_H);
        // back-exit marker (top-right corner, same as scenes 3 and 5)
        iRectangle(BACK_EXIT_X0 - 10, BACK_EXIT_Y - BACK_EXIT_BAND, (BACK_EXIT_X1 + 10) - (BACK_EXIT_X0 - 10), (BACK_EXIT_Y + 20) - (BACK_EXIT_Y - BACK_EXIT_BAND));
    }

    // senior NPC standing at 408,450 (idle animation) + shadow
    if (seniorIdleTexture[0]) {
        iSetColor(25, 25, 25);
        iFilledEllipse(SENIOR_NPC_X + SENIOR_NPC_W / 2, SENIOR_NPC_Y + 10, 30, 10);
        int seniorFrame = (animationTickCounter / 12) % 6;
        iShowImage(SENIOR_NPC_X, SENIOR_NPC_Y, SENIOR_NPC_W, SENIOR_NPC_H, seniorIdleTexture[seniorFrame]);
    }

    drawHero();

    iSetColor(220, 220, 220);
    char scene2PositionText[64];
    sprintf_s(scene2PositionText, "Pos: (%d , %d)", playerSquareX, playerSquareY);
    iText(12, 22, scene2PositionText, GLUT_BITMAP_8_BY_13);

    // talk prompt / active dialogue line in the bottom box (one-way: no back prompt)
    if (dialogueTicksLeft > 0) drawPromptBox(dialogueText);
    else if (isPlayerAtNextFloor()) drawPromptBox("PRESS X TO GO TO NEXT FLOOR");
    else if (isSeniorInTalkRange()) drawPromptBox("PRESS X TO TALK");

    drawButton(backButton);
}

void drawSceneScene3() {
    // same map bg as scene 2 (floor.jpg)
    if (scene3BackgroundTexture) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, scene3BackgroundTexture);
    else if (scene2BackgroundTexture) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, scene2BackgroundTexture);
    else { iSetColor(30, 30, 36); iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); }

    iSetColor(28, 28, 32);
    iFilledRectangle(0, SCREEN_HEIGHT - 70, SCREEN_WIDTH, 70);
    iFilledRectangle(0, 0, SCREEN_WIDTH, 54);
    iSetColor(210, 210, 215);
    drawCenteredText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 62, (char*)"Next floor  |  No way back | B = borders", GLUT_BITMAP_8_BY_13);

    if (showCollisionBorders) {
        iSetColor(150, 150, 155);
        drawLoopOutline(scene2LoopX, scene2LoopY, SCENE2_LOOP_POINTS);
        iRectangle(CR_BLOCK_X, CR_BLOCK_Y, CR_BLOCK_W, CR_BLOCK_H);
        // back-exit marker (top-right corner)
        iRectangle(BACK_EXIT_X0 - 10, BACK_EXIT_Y - BACK_EXIT_BAND, (BACK_EXIT_X1 + 10) - (BACK_EXIT_X0 - 10), (BACK_EXIT_Y + 20) - (BACK_EXIT_Y - BACK_EXIT_BAND));
    }

    // CR NPC on senior's old spot + shadow
    if (crIdleTexture[0]) {
        iSetColor(25, 25, 25);
        iFilledEllipse(CR_NPC_X + CR_NPC_W / 2, CR_NPC_Y + 10, 30, 10);
        int crFrame = (animationTickCounter / 12) % 8;
        iShowImage(CR_NPC_X, CR_NPC_Y, CR_NPC_W, CR_NPC_H, crIdleTexture[crFrame]);
    } else if (heroTexture) {
        iSetColor(25, 25, 25);
        iFilledEllipse(CR_NPC_X + CR_NPC_W / 2, CR_NPC_Y + 10, 30, 10);
        iShowImage(CR_NPC_X, CR_NPC_Y, CR_NPC_W, CR_NPC_H, heroTexture);
    }

    drawHero();

    iSetColor(220, 220, 220);
    char scene3PositionText[64];
    sprintf_s(scene3PositionText, "Pos: (%d , %d)", playerSquareX, playerSquareY);
    iText(12, 22, scene3PositionText, GLUT_BITMAP_8_BY_13);

    if (dialogueTicksLeft > 0) drawPromptBox(dialogueText);
    else if (crHasTalked && isCrInTalkRange()) drawPromptBox("PRESS X TO CAST");
    else if (isCrInTalkRange()) drawPromptBox("PRESS X TO TALK");

    drawButton(backButton);
}

void drawSceneTransition() {
    // cast animation at the hero's spot, hero-sized (mainCharacter_sprites//castSpecialAnimation.png)
    if (scene3BackgroundTexture) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, scene3BackgroundTexture);
    else if (scene2BackgroundTexture) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, scene2BackgroundTexture);
    else { iSetColor(30, 30, 36); iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); }

    iSetColor(28, 28, 32);
    iFilledRectangle(0, SCREEN_HEIGHT - 70, SCREEN_WIDTH, 70);
    iFilledRectangle(0, 0, SCREEN_WIDTH, 54);
    iSetColor(210, 210, 215);
    drawCenteredText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 62, (char*)"Casting...", GLUT_BITMAP_8_BY_13);

    // CR NPC stays on his spot
    if (crIdleTexture[0]) {
        iSetColor(25, 25, 25);
        iFilledEllipse(CR_NPC_X + CR_NPC_W / 2, CR_NPC_Y + 10, 30, 10);
        int crFrame = (animationTickCounter / 12) % 8;
        iShowImage(CR_NPC_X, CR_NPC_Y, CR_NPC_W, CR_NPC_H, crIdleTexture[crFrame]);
    } else if (heroTexture) {
        iSetColor(25, 25, 25);
        iFilledEllipse(CR_NPC_X + CR_NPC_W / 2, CR_NPC_Y + 10, 30, 10);
        iShowImage(CR_NPC_X, CR_NPC_Y, CR_NPC_W, CR_NPC_H, heroTexture);
    }

    // cast frame where the hero stands, at hero size (96x72, same rect as drawHero)
    {
        int castW = 96, castH = 72;
        int castLeft = playerSquareX + 24 - castW / 2;
        int castBottom = playerSquareY - 10;
        iSetColor(25, 25, 25);
        iFilledEllipse(playerSquareX + 24, playerSquareY + 2, 26, 8);
        if (castAnimationTexture) iShowImage(castLeft, castBottom, castW, castH, castAnimationTexture);
        else drawHero();
    }

    drawButton(backButton);
}

void drawSceneScene4() {
    // hall art from Main//scene4.jpg
    if (scene4BackgroundTexture) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, scene4BackgroundTexture);
    else { iSetColor(30, 30, 36); iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); }

    iSetColor(28, 28, 32);
    iFilledRectangle(0, SCREEN_HEIGHT - 70, SCREEN_WIDTH, 70);
    iFilledRectangle(0, 0, SCREEN_WIDTH, 54);
    iSetColor(210, 210, 215);
    {
        char scene4HeaderText[64];
        int scene4SecondsLeft = (scene4TimerTicks + 62) / 63; // ceil(ticks / 62.5)
        if (scene4SecondsLeft < 0) scene4SecondsLeft = 0;
        sprintf_s(scene4HeaderText, "Hall  |  %ds  |  No way back | B = borders", scene4SecondsLeft);
        drawCenteredText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 62, scene4HeaderText, GLUT_BITMAP_8_BY_13);
    }

    if (showCollisionBorders) {
        iSetColor(150, 150, 155);
        drawLoopOutline(scene4LoopX, scene4LoopY, SCENE4_LOOP_POINTS);
        // live hitboxes: hero green, falling objects red — overlap = a counted touch
        {
            int boxLeft, boxBottom, boxW, boxH;
            iSetColor(80, 220, 100);
            heroHitbox(&boxLeft, &boxBottom, &boxW, &boxH);
            iRectangle(boxLeft, boxBottom, boxW, boxH);
            iSetColor(220, 80, 80);
            for (int i = 0; i < HOLY_NOVA_MAX; i++) if (holyNovas[i].active) {
                novaHitbox(&holyNovas[i], &boxLeft, &boxBottom, &boxW, &boxH);
                iRectangle(boxLeft, boxBottom, boxW, boxH);
            }
        }
    }

    // dragon HP bar (top-left under the header, 2x scale) + touch counter
    if (dragonHpTexture[heroHpStage]) iShowImage(16, SCREEN_HEIGHT - 70 - 8 - 64, 224, 64, dragonHpTexture[heroHpStage]);
    {
        char hitsText[48];
        if (heroHpStage >= HERO_HP_STAGES - 1) sprintf_s(hitsText, "TOUCHED: %d  (HP EMPTY)", heroHitCount);
        else sprintf_s(hitsText, "TOUCHED: %d  (next dmg at %d)", heroHitCount, (heroHpStage + 1) * HITS_PER_HP_STAGE);
        iSetColor(220, 220, 220);
        iText(18, SCREEN_HEIGHT - 70 - 8 - 64 - 18, hitsText, GLUT_BITMAP_8_BY_13);
    }

    // HolyNova shower: falling 01s + floor-burst frames (drawn behind the hero)
    for (int i = 0; i < HOLY_NOVA_MAX; i++) if (holyNovas[i].active) {
        if (holyNovas[i].state == 0) {
            if (holyNovaTexture[0]) iShowImage(holyNovas[i].x, holyNovas[i].y, HOLY_NOVA_W, HOLY_NOVA_H, holyNovaTexture[0]);
        } else {
            int burstFrame = 1 + holyNovas[i].animTick / HOLY_NOVA_BURST_TICKS_PER_FRAME; // 1..8
            if (burstFrame < HOLY_NOVA_FRAMES && holyNovaTexture[burstFrame])
                iShowImage(holyNovas[i].x, holyNovas[i].y, HOLY_NOVA_W, HOLY_NOVA_H, holyNovaTexture[burstFrame]);
        }
    }

    drawHero();

    iSetColor(220, 220, 220);
    char scene4PositionText[64];
    sprintf_s(scene4PositionText, "Pos: (%d , %d)", playerSquareX, playerSquareY);
    iText(12, 22, scene4PositionText, GLUT_BITMAP_8_BY_13);

    // minigame banner: always on while dodging the shower
    if (dialogueTicksLeft > 0) drawPromptBox(dialogueText);
    else drawPromptBox("this is a dream !!");

    drawButton(backButton);
}

void drawSceneInterstitial() {
    // black screen + white per-transition text (3-line mode for long messages)
    iSetColor(0, 0, 0);
    iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    iSetColor(255, 255, 255);
    if (interstitialLine2[0] || interstitialLine3[0]) {
        if (interstitialText[0]) drawCenteredText(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 32, interstitialText, GLUT_BITMAP_TIMES_ROMAN_24);
        if (interstitialLine2[0]) drawCenteredText(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, interstitialLine2, GLUT_BITMAP_TIMES_ROMAN_24);
        if (interstitialLine3[0]) drawCenteredText(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 32, interstitialLine3, GLUT_BITMAP_TIMES_ROMAN_24);
    } else {
        drawCenteredText(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, interstitialText, GLUT_BITMAP_TIMES_ROMAN_24);
    }
    drawButton(backButton);
}

void drawSceneScene5() {
    // empty map (same floor.jpg bg as scene 2/3) — no senior, no CR
    if (scene2BackgroundTexture) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, scene2BackgroundTexture);
    else if (scene3BackgroundTexture) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, scene3BackgroundTexture);
    else { iSetColor(30, 30, 36); iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); }

    iSetColor(28, 28, 32);
    iFilledRectangle(0, SCREEN_HEIGHT - 70, SCREEN_WIDTH, 70);
    iFilledRectangle(0, 0, SCREEN_WIDTH, 54);
    iSetColor(210, 210, 215);
    drawCenteredText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 62, (char*)"Empty hall  |  No way back  |  Right-click for Menu | B = borders", GLUT_BITMAP_8_BY_13);

    if (showCollisionBorders) {
        iSetColor(150, 150, 155);
        drawLoopOutline(scene2LoopX, scene2LoopY, SCENE2_LOOP_POINTS);
    }

    drawHero();

    iSetColor(220, 220, 220);
    char scene5PositionText[64];
    sprintf_s(scene5PositionText, "Pos: (%d , %d)", playerSquareX, playerSquareY);
    iText(12, 22, scene5PositionText, GLUT_BITMAP_8_BY_13);

    if (dialogueTicksLeft > 0) drawPromptBox(dialogueText);
    else if (isPlayerAtNextFloor()) drawPromptBox("PRESS X TO GO TO NEXT FLOOR");

    // NOTE: no BACK button here — the hero cannot return to scene 4 (hall)
}

void drawSceneScene6() {
    // one-way map (doorOpen.jpg bg, same layout) — no NPC, no way back
    // collision border stays the previous one (scene2Loop, same as scene 5)
    if (scene6BackgroundTexture) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, scene6BackgroundTexture);
    else if (scene2BackgroundTexture) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, scene2BackgroundTexture);
    else { iSetColor(30, 30, 36); iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); }

    iSetColor(28, 28, 32);
    iFilledRectangle(0, SCREEN_HEIGHT - 70, SCREEN_WIDTH, 70);
    iFilledRectangle(0, 0, SCREEN_WIDTH, 54);
    iSetColor(210, 210, 215);
    drawCenteredText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 62, (char*)"Scene 6  |  No way back  |  Right-click for Menu | B = borders", GLUT_BITMAP_8_BY_13);

    if (showCollisionBorders) {
        iSetColor(150, 150, 155);
        drawLoopOutline(scene6LoopX, scene6LoopY, SCENE6_LOOP_POINTS);
        // door-peek marker (bottom doors)
        iRectangle(DOOR_ZONE_X0 - 10, DOOR_ZONE_Y - DOOR_ZONE_BAND, (DOOR_ZONE_X1 + 10) - (DOOR_ZONE_X0 - 10), (DOOR_ZONE_Y + 20) - (DOOR_ZONE_Y - DOOR_ZONE_BAND));
        // up-exit marker (top of left arm)
        iRectangle(SCENE6_UP_X0 - 10, SCENE6_UP_Y - SCENE6_UP_BAND, (SCENE6_UP_X1 + 10) - (SCENE6_UP_X0 - 10), (SCENE6_UP_Y + 20) - (SCENE6_UP_Y - SCENE6_UP_BAND));
    }

    drawHero();

    iSetColor(220, 220, 220);
    char scene6PositionText[64];
    sprintf_s(scene6PositionText, "Pos: (%d , %d)", playerSquareX, playerSquareY);
    iText(12, 22, scene6PositionText, GLUT_BITMAP_8_BY_13);

    if (dialogueTicksLeft > 0) drawPromptBox(dialogueText);
    else if (isPlayerAtDoorZone()) drawPromptBox("PRESS X TO PEEK");
    else if (isPlayerAtScene6UpExit()) drawPromptBox("PRESS X TO GO TO NEXT FLOOR");
    // NOTE: no BACK button here — the hero cannot return past scene 5
}

void drawSceneScene7() {
    // maze quiz map (Main//maze.jpg) — top-right start, no way back
    if (scene7BackgroundTexture) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, scene7BackgroundTexture);
    else if (scene2BackgroundTexture) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, scene2BackgroundTexture);
    else { iSetColor(30, 30, 36); iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); }

    iSetColor(28, 28, 32);
    iFilledRectangle(0, SCREEN_HEIGHT - 70, SCREEN_WIDTH, 70);
    iFilledRectangle(0, 0, SCREEN_WIDTH, 54);
    iSetColor(210, 210, 215);
    drawCenteredText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 62, (char*)"Maze  |  No way back  |  Right-click for Menu | B = borders", GLUT_BITMAP_8_BY_13);

    if (showCollisionBorders) {
        iSetColor(150, 150, 155);
        drawLoopOutline(scene7LoopX, scene7LoopY, SCENE7_LOOP_POINTS);
        // hedge wall runs in white, open segments only (no last->first wrap edge)
        iSetColor(220, 220, 220);
        for (int wallDbgI = 0; wallDbgI + 1 < MAZE_WALL_POINTS; wallDbgI++)
            iLine(mazeWallX[wallDbgI], mazeWallY[wallDbgI], mazeWallX[wallDbgI + 1], mazeWallY[wallDbgI + 1]);
        // maze-goal marker (center)
        iSetColor(150, 150, 155);
        iRectangle(MAZE_GOAL_X0 - 10, MAZE_GOAL_Y0, (MAZE_GOAL_X1 + 10) - (MAZE_GOAL_X0 - 10), MAZE_GOAL_Y1 - MAZE_GOAL_Y0);
    }

    drawHero();

    iSetColor(220, 220, 220);
    char scene7PositionText[64];
    sprintf_s(scene7PositionText, "Pos: (%d , %d)", playerSquareX, playerSquareY);
    iText(12, 22, scene7PositionText, GLUT_BITMAP_8_BY_13);

    if (dialogueTicksLeft > 0) drawPromptBox(dialogueText);
    else if (isPlayerAtMazeGoal()) drawPromptBox("Acquire ink storm");
    // NOTE: no BACK button here — the hero cannot return past scene 6
}

void drawSceneScene8() {
    // race map (same floor.jpg bg as scene 2): top-left goal in 10s, HP at 80%
    if (scene2BackgroundTexture) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, scene2BackgroundTexture);
    else if (scene3BackgroundTexture) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, scene3BackgroundTexture);
    else { iSetColor(30, 30, 36); iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); }

    iSetColor(28, 28, 32);
    iFilledRectangle(0, SCREEN_HEIGHT - 70, SCREEN_WIDTH, 70);
    iFilledRectangle(0, 0, SCREEN_WIDTH, 54);
    iSetColor(210, 210, 215);
    drawCenteredText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 62, (char*)"Scene 8  |  Reach the top-left!  |  No way back | B = borders", GLUT_BITMAP_8_BY_13);

    // big middle-top countdown (red in the last 3s)
    {
        char raceTimerText[16];
        int raceSecondsLeft = (scene8TimerTicks + 62) / 63; // ceil(ticks / 62.5)
        if (raceSecondsLeft < 0) raceSecondsLeft = 0;
        sprintf_s(raceTimerText, "%d", raceSecondsLeft);
        if (raceSecondsLeft <= 3) iSetColor(220, 60, 60);
        else iSetColor(255, 255, 255);
        drawCenteredText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 112, raceTimerText, GLUT_BITMAP_TIMES_ROMAN_24);
    }

    if (showCollisionBorders) {
        iSetColor(150, 150, 155);
        drawLoopOutline(scene2LoopX, scene2LoopY, SCENE2_LOOP_POINTS);
    }

    // HP bar (top-left under the header, 2x scale)
    if (dragonHpTexture[heroHpStage]) iShowImage(16, SCREEN_HEIGHT - 70 - 8 - 64, 224, 64, dragonHpTexture[heroHpStage]);

    drawHero();

    iSetColor(220, 220, 220);
    char scene8PositionText[64];
    sprintf_s(scene8PositionText, "Pos: (%d , %d)", playerSquareX, playerSquareY);
    iText(12, 22, scene8PositionText, GLUT_BITMAP_8_BY_13);

    if (dialogueTicksLeft > 0) drawPromptBox(dialogueText);
    else if (scene8GoalReached && isPlayerAtNextFloor()) drawPromptBox("PRESS X TO GO TO NEXT SCENE");

    drawButton(backButton);
}

void drawSceneScene9() {
    // same floor.jpg map — attack CR waits at the old CR spot, no way back
    if (scene2BackgroundTexture) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, scene2BackgroundTexture);
    else if (scene3BackgroundTexture) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, scene3BackgroundTexture);
    else { iSetColor(30, 30, 36); iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); }

    iSetColor(28, 28, 32);
    iFilledRectangle(0, SCREEN_HEIGHT - 70, SCREEN_WIDTH, 70);
    iFilledRectangle(0, 0, SCREEN_WIDTH, 54);
    iSetColor(210, 210, 215);
    drawCenteredText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 62, (char*)"Scene 9  |  No way back  |  Right-click for Menu | B = borders", GLUT_BITMAP_8_BY_13);

    if (showCollisionBorders) {
        iSetColor(150, 150, 155);
        drawLoopOutline(scene2LoopX, scene2LoopY, SCENE2_LOOP_POINTS);
        iRectangle(ATTACK_BLOCK_X, ATTACK_BLOCK_Y, ATTACK_BLOCK_W, ATTACK_BLOCK_H);
    }

    // CR on the old CR spot + shadow: old sprite while talking, new_cr after
    if (!crTransformed && crIdleTexture[0]) {
        iSetColor(25, 25, 25);
        iFilledEllipse(CR_NPC_X + CR_NPC_W / 2, CR_NPC_Y + 10, 30, 10);
        int crFrame = (animationTickCounter / 12) % 8;
        iShowImage(CR_NPC_X, CR_NPC_Y, CR_NPC_W, CR_NPC_H, crIdleTexture[crFrame]);
    } else if (crTransformed && crCngTexture[0]) {
        iSetColor(25, 25, 25);
        iFilledEllipse(CR_NPC_X + CR_NPC_W / 2, CR_NPC_Y + 10, 30, 10);
        int cngFrame = (animationTickCounter / 12) % CNG_FRAMES;
        iShowImage(CR_NPC_X, CR_NPC_Y, CR_NPC_W, CR_NPC_H, crCngTexture[cngFrame]);
    } else if (heroTexture) {
        iSetColor(25, 25, 25);
        iFilledEllipse(CR_NPC_X + CR_NPC_W / 2, CR_NPC_Y + 10, 30, 10);
        iShowImage(CR_NPC_X, CR_NPC_Y, CR_NPC_W, CR_NPC_H, heroTexture);
    }

    drawHero();

    iSetColor(220, 220, 220);
    char scene9PositionText[64];
    sprintf_s(scene9PositionText, "Pos: (%d , %d)", playerSquareX, playerSquareY);
    iText(12, 22, scene9PositionText, GLUT_BITMAP_8_BY_13);

    if (dialogueTicksLeft > 0) drawPromptBox(dialogueText);
    else if (crAttackTalked && isCrInTalkRange()) drawPromptBox("PRESS X TO FIGHT");
    else if (isCrInTalkRange()) drawPromptBox("PRESS X TO TALK");
    // NOTE: no BACK button here — the hero cannot return past scene 8
}

void drawSceneScene10() {
    // post-Aftermath floor (doorOpen.jpg, same layout as scene 6) — detached state, no quiz exit
    if (scene6BackgroundTexture) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, scene6BackgroundTexture);
    else if (scene2BackgroundTexture) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, scene2BackgroundTexture);
    else { iSetColor(30, 30, 36); iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); }

    iSetColor(28, 28, 32);
    iFilledRectangle(0, SCREEN_HEIGHT - 70, SCREEN_WIDTH, 70);
    iFilledRectangle(0, 0, SCREEN_WIDTH, 54);
    iSetColor(210, 210, 215);
    drawCenteredText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 62, (char*)"Scene 10  |  No way back  |  Right-click for Menu | B = borders", GLUT_BITMAP_8_BY_13);

    if (showCollisionBorders) {
        iSetColor(150, 150, 155);
        drawLoopOutline(scene6LoopX, scene6LoopY, SCENE6_LOOP_POINTS);
        // door-peek marker (bottom doors)
        iRectangle(DOOR_ZONE_X0 - 10, DOOR_ZONE_Y - DOOR_ZONE_BAND, (DOOR_ZONE_X1 + 10) - (DOOR_ZONE_X0 - 10), (DOOR_ZONE_Y + 20) - (DOOR_ZONE_Y - DOOR_ZONE_BAND));
    }

    drawHero();

    iSetColor(220, 220, 220);
    char scene10PositionText[64];
    sprintf_s(scene10PositionText, "Pos: (%d , %d)", playerSquareX, playerSquareY);
    iText(12, 22, scene10PositionText, GLUT_BITMAP_8_BY_13);

    if (dialogueTicksLeft > 0) drawPromptBox(dialogueText);
    else if (isPlayerAtDoorZone()) drawPromptBox("PRESS X TO PEEK");
    else if (isPlayerAtScene6UpExit()) drawPromptBox("PRESS X TO GO TO NEXT FLOOR");
    // NOTE: no BACK button here — one-way destiny (BACK/ESC show the destiny popup)
}

void drawClassroomNpc(int npcX, int npcY, int npcW, int npcH, unsigned int texture) {
    if (!texture) return;
    iSetColor(25, 25, 25);
    iFilledEllipse(npcX + npcW / 2, npcY + 10, 30, 10);
    iShowImage(npcX, npcY, npcW, npcH, texture);
}

void drawSceneClassroom1() {
    // first half of the big classroom map (classroom1.jpg), same room as classroom2
    if (classroom1Texture) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, classroom1Texture);
    else if (scene2BackgroundTexture) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, scene2BackgroundTexture);
    else { iSetColor(30, 30, 36); iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); }

    iSetColor(28, 28, 32);
    iFilledRectangle(0, SCREEN_HEIGHT - 70, SCREEN_WIDTH, 70);
    iFilledRectangle(0, 0, SCREEN_WIDTH, 54);
    iSetColor(210, 210, 215);
    drawCenteredText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 62, (char*)"Classroom 1  |  No way back  |  Right-click for Menu | B = borders", GLUT_BITMAP_8_BY_13);

    if (showCollisionBorders) {
        iSetColor(150, 150, 155);
        drawLoopOutline(classroom1LoopX, classroom1LoopY, CLASSROOM1_LOOP_POINTS);
        iRectangle(MONT_BLOCK_X, MONT_BLOCK_Y, MONT_BLOCK_W, MONT_BLOCK_H);
        iRectangle(NOPLA_BLOCK_X, NOPLA_BLOCK_Y, NOPLA_BLOCK_W, NOPLA_BLOCK_H);
    }

    drawClassroomNpc(MONT_NPC_X, MONT_NPC_Y, MONT_NPC_W, MONT_NPC_H, montTexture);
    drawClassroomNpc(NOPLA_NPC_X, NOPLA_NPC_Y, NOPLA_NPC_W, NOPLA_NPC_H, noplaTexture);

    drawHero();

    iSetColor(220, 220, 220);
    char classroom1PositionText[64];
    sprintf_s(classroom1PositionText, "Pos: (%d , %d)", playerSquareX, playerSquareY);
    iText(12, 22, classroom1PositionText, GLUT_BITMAP_8_BY_13);

    if (dialogueTicksLeft > 0) drawPromptBox(dialogueText);
    else if (isMontInTalkRange()) drawPromptBox("PRESS X TO TALK TO MONT");
    else if (isNoplaInTalkRange()) drawPromptBox("PRESS X TO TALK TO NOPLA");
    // NOTE: no BACK button here — the hero cannot return past scene 10
}

void drawSceneClassroom2() {
    // second half of the big classroom map (classroom2.jpg), no interstitial from part 1
    if (classroom2Texture) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, classroom2Texture);
    else if (classroom1Texture) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, classroom1Texture);
    else { iSetColor(30, 30, 36); iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); }

    iSetColor(28, 28, 32);
    iFilledRectangle(0, SCREEN_HEIGHT - 70, SCREEN_WIDTH, 70);
    iFilledRectangle(0, 0, SCREEN_WIDTH, 54);
    iSetColor(210, 210, 215);
    drawCenteredText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 62, (char*)"Classroom 2  |  No way back  |  Right-click for Menu | B = borders", GLUT_BITMAP_8_BY_13);

    if (showCollisionBorders) {
        iSetColor(150, 150, 155);
        drawLoopOutline(classroom2LoopX, classroom2LoopY, CLASSROOM2_LOOP_POINTS);
        iRectangle(CLASSCR_BLOCK_X, CLASSCR_BLOCK_Y, CLASSCR_BLOCK_W, CLASSCR_BLOCK_H);
        iRectangle(RINO_BLOCK_X, RINO_BLOCK_Y, RINO_BLOCK_W, RINO_BLOCK_H);
    }

    drawClassroomNpc(CLASSCR_NPC_X, CLASSCR_NPC_Y, CLASSCR_NPC_W, CLASSCR_NPC_H, classCrTexture);
    drawClassroomNpc(RINO_NPC_X, RINO_NPC_Y, RINO_NPC_W, RINO_NPC_H, rinoTexture);

    drawHero();

    iSetColor(220, 220, 220);
    char classroom2PositionText[64];
    sprintf_s(classroom2PositionText, "Pos: (%d , %d)", playerSquareX, playerSquareY);
    iText(12, 22, classroom2PositionText, GLUT_BITMAP_8_BY_13);

    if (dialogueTicksLeft > 0) drawPromptBox(dialogueText);
    else if (rinoTalked && isRinoInTalkRange()) drawPromptBox("PRESS X TO ACCEPT");
    else if (isRinoInTalkRange()) drawPromptBox("PRESS X TO TALK TO RINO");
    else if (isClassCrInTalkRange()) drawPromptBox("PRESS X TO TALK TO CR");
    // NOTE: no BACK button here — library exit is via Rino only
}

void drawSceneLibrary() {
    // first half of the big library map (library1.jpg), same room as library2
    if (library1Texture) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, library1Texture);
    else { iSetColor(12, 12, 16); iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); }

    iSetColor(28, 28, 32);
    iFilledRectangle(0, SCREEN_HEIGHT - 70, SCREEN_WIDTH, 70);
    iFilledRectangle(0, 0, SCREEN_WIDTH, 54);
    iSetColor(210, 210, 215);
    drawCenteredText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 62, (char*)"Library 1  |  No way back  |  Right-click for Menu | B = borders", GLUT_BITMAP_8_BY_13);

    if (showCollisionBorders) {
        iSetColor(150, 150, 155);
        drawLoopOutline(library1LoopX, library1LoopY, LIBRARY1_LOOP_POINTS);
    }

    drawHero();

    iSetColor(220, 220, 220);
    char libraryPositionText[64];
    sprintf_s(libraryPositionText, "Pos: (%d , %d)", playerSquareX, playerSquareY);
    iText(12, 22, libraryPositionText, GLUT_BITMAP_8_BY_13);

    if (dialogueTicksLeft > 0) drawPromptBox(dialogueText);
    // NOTE: no BACK button here — the hero cannot return past library 1
}

void drawSceneBigGate() {
    // gate courtyard (bigGate.png 1264x847 stretched over 1200x800)
    if (bigGateTexture) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bigGateTexture);
    else { iSetColor(30, 30, 36); iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); }

    iSetColor(28, 28, 32);
    iFilledRectangle(0, SCREEN_HEIGHT - 70, SCREEN_WIDTH, 70);
    iFilledRectangle(0, 0, SCREEN_WIDTH, 54);
    iSetColor(210, 210, 215);
    drawCenteredText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 62, (char*)"Big Gate  |  No way back  |  Right-click for Menu | B = borders", GLUT_BITMAP_8_BY_13);

    if (showCollisionBorders) {
        iSetColor(150, 150, 155);
        drawLoopOutline(bigGateLoopX, bigGateLoopY, BIGGATE_LOOP_POINTS);
        // gate-access marker (step front center)
        iRectangle(GATE_EXIT_X0 - 10, GATE_EXIT_Y - GATE_EXIT_BAND, (GATE_EXIT_X1 + 10) - (GATE_EXIT_X0 - 10), (GATE_EXIT_Y + 20) - (GATE_EXIT_Y - GATE_EXIT_BAND));
    }

    drawHero();

    iSetColor(220, 220, 220);
    char bigGatePositionText[64];
    sprintf_s(bigGatePositionText, "Pos: (%d , %d)", playerSquareX, playerSquareY);
    iText(12, 22, bigGatePositionText, GLUT_BITMAP_8_BY_13);

    if (dialogueTicksLeft > 0) drawPromptBox(dialogueText);
    else if (isPlayerAtGateExit()) drawPromptBox("PRESS X TO ENTER");
    // NOTE: no BACK button here — the hero cannot return to the library
}

void drawSceneBossRoom() {
    // throne hall (bossRoom1.png 1264x847 stretched over 1200x800)
    if (bossRoomTexture) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bossRoomTexture);
    else if (bigGateTexture) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bigGateTexture);
    else { iSetColor(30, 30, 36); iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); }

    iSetColor(28, 28, 32);
    iFilledRectangle(0, SCREEN_HEIGHT - 70, SCREEN_WIDTH, 70);
    iFilledRectangle(0, 0, SCREEN_WIDTH, 54);
    iSetColor(210, 210, 215);
    drawCenteredText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 62, (char*)"Boss Room  |  No way back  |  Right-click for Menu | B = borders", GLUT_BITMAP_8_BY_13);

    if (showCollisionBorders) {
        iSetColor(150, 150, 155);
        drawLoopOutline(bossRoomLoopX, bossRoomLoopY, BOSSROOM_LOOP_POINTS);
    }

    // boss idle on the throne at 552,244 + shadow, 2px bob keeps the animation playing
    if (bossThroneTexture) {
        iSetColor(25, 25, 25);
        iFilledEllipse(BOSS_THRONE_X + BOSS_THRONE_W / 2, BOSS_THRONE_Y + 10, 34, 11);
        int bossBob = ((animationTickCounter / 24) % 2) * 2;
        iShowImage(BOSS_THRONE_X, BOSS_THRONE_Y + bossBob, BOSS_THRONE_W, BOSS_THRONE_H, bossThroneTexture);
    }

    drawHero();

    iSetColor(220, 220, 220);
    char bossRoomPositionText[64];
    sprintf_s(bossRoomPositionText, "Pos: (%d , %d)", playerSquareX, playerSquareY);
    iText(12, 22, bossRoomPositionText, GLUT_BITMAP_8_BY_13);

    if (dialogueTicksLeft > 0) drawPromptBox(dialogueText);
    else if (isBossInTalkRange()) drawPromptBox("PRESS X TO FACE HIM");
    // NOTE: no BACK button here — one-way destiny end for now
}

void drawSceneLibrary2() {
    // second half of the big library map (library2.jpg), no interstitial from part 1
    if (library2Texture) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, library2Texture);
    else if (library1Texture) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, library1Texture);
    else { iSetColor(12, 12, 16); iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); }

    iSetColor(28, 28, 32);
    iFilledRectangle(0, SCREEN_HEIGHT - 70, SCREEN_WIDTH, 70);
    iFilledRectangle(0, 0, SCREEN_WIDTH, 54);
    iSetColor(210, 210, 215);
    drawCenteredText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 62, (char*)"Library 2  |  No way back  |  Right-click for Menu | B = borders", GLUT_BITMAP_8_BY_13);

    if (showCollisionBorders) {
        iSetColor(150, 150, 155);
        drawLoopOutline(library2LoopX, library2LoopY, LIBRARY2_LOOP_POINTS);
    }

    // demonic book in top-left, drawn small (resized via W/H)
    if (library2BookTexture) iShowImage(LIBRARY2_BOOK_X, LIBRARY2_BOOK_Y, LIBRARY2_BOOK_W, LIBRARY2_BOOK_H, library2BookTexture);

    drawHero();

    iSetColor(220, 220, 220);
    char library2PositionText[64];
    sprintf_s(library2PositionText, "Pos: (%d , %d)", playerSquareX, playerSquareY);
    iText(12, 22, library2PositionText, GLUT_BITMAP_8_BY_13);

    if (dialogueTicksLeft > 0) drawPromptBox(dialogueText);
    else if (isLibrary2BookInRange()) drawPromptBox("Open the book");
    else if (isPlayerAtNextFloor()) drawPromptBox("PRESS X TO GO TO NEXT FLOOR");
    // NOTE: no BACK button here — one-way destiny end for now
}

void drawSceneBookQuiz() {
    // black screen straight from the book; one question in a dialogue box + A/B option boxes
    iSetColor(0, 0, 0);
    iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    iSetColor(255, 240, 180);
    {
        char title[32];
        if (bookQuizTotal > 0) sprintf_s(title, "THE BOOK OPENS... Q %d/%d", bookQuizIndex + 1 <= bookQuizTotal ? bookQuizIndex + 1 : bookQuizTotal, bookQuizTotal);
        else sprintf_s(title, "THE BOOK OPENS...");
        drawCenteredText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 40, title, GLUT_BITMAP_HELVETICA_18);
    }
    if (bookQuizTotal > 0 && bookQuizIndex < bookQuizTotal) {
        // dialogue box with the current question (ui bar strip + corner frame, no X keycap)
        int boxLeft = 60, boxBottom = 260, boxWidth = SCREEN_WIDTH - 120, boxHeight = 150;
        if (buttonBarNormalTexture) iShowImage(boxLeft, boxBottom, boxWidth, boxHeight, buttonBarNormalTexture);
        else { iSetColor(28, 22, 18); iFilledRectangle(boxLeft, boxBottom, boxWidth, boxHeight); }
        drawCornerFrame(boxLeft, boxBottom, boxWidth, boxHeight, 16);
        iSetColor(255, 240, 180);
        drawCenteredText(SCREEN_WIDTH / 2, boxBottom + boxHeight / 2 + 18, bookQuizQText[bookQuizIndex], GLUT_BITMAP_HELVETICA_18);
        iSetColor(200, 200, 205);
        drawCenteredText(SCREEN_WIDTH / 2, boxBottom + boxHeight / 2 - 18, (char*)"Choose A or B below  (click or press A / B)", GLUT_BITMAP_8_BY_13);
        // 2 option boxes using the previous UI buttons
        quizOptAButton.text = bookQuizOptA[bookQuizIndex];
        quizOptBButton.text = bookQuizOptB[bookQuizIndex];
        drawButton(quizOptAButton);
        drawButton(quizOptBButton);
    } else if (bookQuizTotal <= 0) {
        iSetColor(220, 220, 220);
        drawCenteredText(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, (char*)"Quiz files missing.", GLUT_BITMAP_HELVETICA_18);
    }
    // per-answer feedback: you are correct / you are wrong
    if (bookQuizMessage[0]) {
        if (bookQuizMessage[8] == 'c') iSetColor(120, 230, 140);
        else if (bookQuizMessage[8] == 'w') iSetColor(255, 120, 120);
        else iSetColor(255, 200, 130);
        drawCenteredText(SCREEN_WIDTH / 2, 120, bookQuizMessage, GLUT_BITMAP_HELVETICA_18);
    }
    iSetColor(150, 150, 155);
    iText(80, 40, (char*)"Wrong answer restarts at Q1. All correct -> next scene. ESC back.", GLUT_BITMAP_8_BY_13);
}

void drawSceneSplash() {
    // black screen + sword-of-justice splash across the window
    iSetColor(0, 0, 0);
    iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    {
        int swordFrame = (SPLASH_SHOW_TICKS - splashTicksLeft) / 9; // 13 frames over ~117 ticks
        if (swordFrame < 0) swordFrame = 0;
        if (swordFrame >= SWORD_FRAMES) swordFrame = SWORD_FRAMES - 1;
        if (swordTexture[swordFrame]) iShowImage(408, 16, 384, 768, swordTexture[swordFrame]); // 6x, aspect kept
    }
    drawButton(backButton);
}

void drawSceneBattle() {
    // Pokemon-style finale on the boss map: foe right, hero left, options right
    if (bossBackgroundTexture) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bossBackgroundTexture);
    else { iSetColor(18, 18, 24); iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); }

    iSetColor(28, 28, 32);
    iFilledRectangle(0, SCREEN_HEIGHT - 70, SCREEN_WIDTH, 70);
    iFilledRectangle(0, 0, SCREEN_WIDTH, 54);
    iSetColor(210, 210, 215);
    drawCenteredText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 62, (char*)"MID  |  Press 1-4 or Click a move", GLUT_BITMAP_8_BY_13);

    // foe: attack anim on his turn, stance loop otherwise + shadow
    iSetColor(25, 25, 25);
    iFilledEllipse(BATTLE_FOE_X + BATTLE_FOE_SIZE / 2, BATTLE_FOE_Y + 12, 40, 12);
    if (battlePhase == PHASE_ENEMY_ANIM && crAtkTexture[0]) {
        int atkFrame = battleAnimTick / 8;
        if (atkFrame >= ATK_FRAMES) atkFrame = ATK_FRAMES - 1;
        iShowImage(BATTLE_FOE_X, BATTLE_FOE_Y, BATTLE_FOE_SIZE, BATTLE_FOE_SIZE, crAtkTexture[atkFrame]);
    } else if (crCngTexture[0]) {
        int cngFrame = (animationTickCounter / 12) % CNG_FRAMES;
        iShowImage(BATTLE_FOE_X, BATTLE_FOE_Y, BATTLE_FOE_SIZE, BATTLE_FOE_SIZE, crCngTexture[cngFrame]);
    } else if (heroTexture) {
        iShowImage(BATTLE_FOE_X, BATTLE_FOE_Y, BATTLE_FOE_SIZE, BATTLE_FOE_SIZE, heroTexture);
    }
    // foe HP bar + label
    if (dragonHpTexture[enemyHpStage]) iShowImage(676, 548, 224, 64, dragonHpTexture[enemyHpStage]);
    iSetColor(255, 240, 180);
    iText(676, 620, (char*)"CR", GLUT_BITMAP_HELVETICA_18);

    drawHero(); // staged at feet (300,280) facing the foe

    // player HP bar + label
    if (dragonHpTexture[heroHpStage]) iShowImage(16, 150, 224, 64, dragonHpTexture[heroHpStage]);
    iSetColor(255, 240, 180);
    iText(16, 222, (char*)"MC", GLUT_BITMAP_HELVETICA_18);

    // player move animation over the target (foe for hits, hero for heal/shield)
    if (battlePhase == PHASE_PLAYER_ANIM) {
        int animFrame = battleAnimTick / BATTLE_ANIM_TICKS;
        if (animFrame >= MOVE_FRAMES[battleSelectedMove]) animFrame = MOVE_FRAMES[battleSelectedMove] - 1;
        unsigned int animTex = battleMoveFrame(battleSelectedMove, animFrame);
        if (animTex) {
            if (battleSelectedMove == MOVE_HEAL || battleSelectedMove == MOVE_SHIELD)
                iShowImage(386, 262, 128, 128, animTex); // over the hero
            else
                iShowImage(716, 396, 128, 128, animTex); // over the foe
        }
    }

    // 4 option buttons on the right (ui bars + font)
    for (int optI = 0; optI < 4; optI++) {
        int optBottom = BATTLE_OPT_Y0 + optI * (BATTLE_OPT_H + BATTLE_OPT_GAP);
        bool optDisabled = (optI == MOVE_SMITE && smiteCooldown > 0);
        if (optDisabled && buttonBarNormalTexture) iShowImage(BATTLE_OPT_X, optBottom, BATTLE_OPT_W, BATTLE_OPT_H, buttonBarNormalTexture);
        else if (buttonBarHoverTexture) iShowImage(BATTLE_OPT_X, optBottom, BATTLE_OPT_W, BATTLE_OPT_H, buttonBarHoverTexture);
        else { iSetColor(110, 110, 115); iFilledRectangle(BATTLE_OPT_X, optBottom, BATTLE_OPT_W, BATTLE_OPT_H); }
        iSetColor(40, 40, 45);
        const char* optLabel = BATTLE_OPT_LABEL[optI];
        if (optDisabled) optLabel = "2 SMITE [WAIT]";
        drawCenteredText(BATTLE_OPT_X + BATTLE_OPT_W / 2, optBottom + BATTLE_OPT_H / 2 - 6, (char*)optLabel, GLUT_BITMAP_HELVETICA_18);
    }

    if (dialogueTicksLeft > 0) drawPromptBox(dialogueText);
    else if (battlePhase == PHASE_OVER) {
        if (battleResult == 1) drawPromptBox("PRESS X TO ROAM");
        else drawPromptBox("PRESS X TO REMATCH");
    }
    // NOTE: no BACK button here — win/lose exits via the prompt (right-click escapes)
}

// ==== LASTBRAWL revertable block 4/4a: renderer (throne-hall 2D brawl; MID untouched) ====
void drawSceneLastBrawl() {
    // same bossroom map as the throne hall behind the fighters
    if (bossRoomTexture) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bossRoomTexture);
    else { iSetColor(12, 12, 16); iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); }

    iSetColor(28, 28, 32);
    iFilledRectangle(0, SCREEN_HEIGHT - 70, SCREEN_WIDTH, 70);
    iFilledRectangle(0, 0, SCREEN_WIDTH, 54);
    iSetColor(210, 210, 215);
    drawCenteredText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 62, (char*)"LAST TRIAL  |  A/D to move  |  SPACE to strike | B = borders", GLUT_BITMAP_8_BY_13);

    // boss: cape stance facing the hero (W left / E right), claw swipe attacking, flicker dying
    {
        int bossW = 140, bossH = 130;
        int bossLeft = brawlBossX - bossW / 2, bossBottom = BRAWL_FLOOR_Y - 10;
        iSetColor(25, 25, 25);
        iFilledEllipse(brawlBossX, BRAWL_FLOOR_Y + 2, 36, 11);
        unsigned int bossIdle = brawlBossFaceL ? bossBrawlStanceETexture : bossBrawlStanceWTexture;
        if (!bossIdle) bossIdle = brawlBossFaceL ? bossBrawlStanceWTexture : bossBrawlStanceETexture;
        unsigned int bossAtk = brawlBossFaceL ? bossBrawlAtkTexture : bossBrawlClawETexture;
        if (!bossAtk) bossAtk = bossIdle;
        unsigned int bossFrame = bossIdle;
        if (brawlOver == 3 && bossBrawlDieTexture) {
            int diePhase = brawlDieTick / (BRAWL_DIE_TICKS / 4); // 0..3: blink quickens, then gone
            int blink = (diePhase < 3) ? (brawlDieTick / (8 - 2 * diePhase)) % 2 : 1;
            bossFrame = (blink == 0) ? bossBrawlDieTexture : 0;
        }
        else if (brawlBossAtkTick > 0 && bossAtk) bossFrame = bossAtk;
        if (bossFrame) iShowImage(bossLeft, bossBottom, bossW, bossH, bossFrame);
    }

    // hero: faces the boss side; strike pose swinging, walk cycle moving, idle rest
    {
        int heroDir = brawlHeroFaceR ? 2 : 3;
        int heroW = 96, heroH = 72;
        int heroLeft = brawlHeroX - heroW / 2, heroBottom = BRAWL_FLOOR_Y - 10;
        iSetColor(25, 25, 25);
        iFilledEllipse(brawlHeroX, BRAWL_FLOOR_Y + 2, 26, 8);
        unsigned int heroFrame = heroIdle[heroDir];
        if (brawlHeroAtkTick > 0) {
            unsigned int heroAtk = brawlHeroFaceR ? heroBrawlAtkTexture : heroBrawlAtkWTexture;
            if (!heroAtk) heroAtk = brawlHeroFaceR ? heroBrawlAtkWTexture : heroBrawlAtkTexture;
            if (heroAtk) heroFrame = heroAtk;
        }
        else if (playerIsMoving) {
            heroFrame = ((animationTickCounter / 8) % 2 == 0) ? heroWalkA[heroDir] : heroWalkB[heroDir];
            if (!heroFrame) heroFrame = heroIdle[heroDir];
        }
        if (heroFrame) iShowImage(heroLeft, heroBottom, heroW, heroH, heroFrame);
        else if (heroTexture) iShowImage(heroLeft, heroBottom, heroW, heroH, heroTexture);
    }

    // holy-sword overlay on the boss when a hero strike lands
    if (brawlSwordTick > 0) {
        int swordFrame = (brawlSwordTick - 1) / BRAWL_SWORD_TICKS;
        if (swordFrame >= BRAWL_SWORD_FRAMES) swordFrame = BRAWL_SWORD_FRAMES - 1;
        if (swordTexture[swordFrame]) iShowImage(brawlBossX - 64, BRAWL_FLOOR_Y - 10, 128, 128, swordTexture[swordFrame]);
    }
    // holy-nova burst overlay on the hero when a boss swipe lands
    if (brawlNovaTick > 0) {
        int novaFrame = 1 + (brawlNovaTick - 1) / BRAWL_NOVA_TICKS;
        if (novaFrame >= HOLY_NOVA_FRAMES) novaFrame = HOLY_NOVA_FRAMES - 1;
        if (holyNovaTexture[novaFrame]) iShowImage(brawlHeroX - 64, BRAWL_FLOOR_Y - 10, 128, 64, holyNovaTexture[novaFrame]);
    }

    // HP bars: MC green parts bottom-left, boss red dragon-head bar top-right (1024x254 art)
    if (dragonHpTexture[brawlHeroHp]) iShowImage(16, 150, 224, 64, dragonHpTexture[brawlHeroHp]);
    iSetColor(255, 240, 180);
    iText(16, 222, (char*)"MC", GLUT_BITMAP_HELVETICA_18);
    {
        int bossBarFile = BOSS_BAR_STAGE[brawlBossHp];
        if (bossBarTexture[bossBarFile]) iShowImage(720, 600, 464, 115, bossBarTexture[bossBarFile]);
        else if (dragonHpTexture[brawlBossHp]) iShowImage(676, 548, 224, 64, dragonHpTexture[brawlBossHp]);
    }
    iSetColor(255, 240, 180);
    iText(720, 580, (char*)"BOSS", GLUT_BITMAP_HELVETICA_18);

    if (brawlOver == 2) drawPromptBox("PRESS X TO ACCEPT DEFEAT");
}
// ==== end LASTBRAWL block 4/4a ====

// ==== ENDING revertable block 3/3a: black ending screen (remove to revert) ====
void drawSceneEnding() {
    iSetColor(0, 0, 0);
    iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    iSetColor(255, 255, 255);
    drawCenteredText(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 24, (char*)"So you're ready to wake up now", GLUT_BITMAP_TIMES_ROMAN_24);
    drawCenteredText(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 24, (char*)"TO BE CONTINUED FOR PART 2", GLUT_BITMAP_TIMES_ROMAN_24);
    iSetColor(150, 150, 155);
    drawCenteredText(SCREEN_WIDTH / 2, 80, (char*)"PRESS X", GLUT_BITMAP_8_BY_13);
}
// ==== end ENDING block 3/3a ====

void drawSceneVictory() {
    // post-fight free roam on the boss map: right edge continues to scene 10
    if (bossBackgroundTexture) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bossBackgroundTexture);
    else { iSetColor(18, 18, 24); iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); }

    iSetColor(28, 28, 32);
    iFilledRectangle(0, SCREEN_HEIGHT - 70, SCREEN_WIDTH, 70);
    iFilledRectangle(0, 0, SCREEN_WIDTH, 54);
    iSetColor(210, 210, 215);
    drawCenteredText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 62, (char*)"Aftermath  |  No way back  |  Right-click for Menu | B = borders", GLUT_BITMAP_8_BY_13);

    if (showCollisionBorders) {
        iSetColor(150, 150, 155);
        drawLoopOutline(aftermathLoopX, aftermathLoopY, AFTERMATH_LOOP_POINTS);
        iRectangle(VICTORY_EXIT_X, 54, SCREEN_WIDTH - VICTORY_EXIT_X, SCREEN_HEIGHT - 70 - 54);
    }

    drawHero();

    iSetColor(220, 220, 220);
    char victoryPositionText[64];
    sprintf_s(victoryPositionText, "Pos: (%d , %d)", playerSquareX, playerSquareY);
    iText(12, 22, victoryPositionText, GLUT_BITMAP_8_BY_13);

    if (dialogueTicksLeft > 0) drawPromptBox(dialogueText);
    else if (isPlayerAtVictoryExit()) drawPromptBox("PRESS X TO CONTINUE");
    // NOTE: no BACK button here — the hero cannot return past the fight
}

void drawScenePlaneGame() {
    // Rino's game: flappy plane over the tower field (planeGame//background.png)
    if (planeBgTexture) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, planeBgTexture);
    else { iSetColor(30, 30, 36); iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); }

    iSetColor(28, 28, 32);
    iFilledRectangle(0, SCREEN_HEIGHT - 70, SCREEN_WIDTH, 70);
    iFilledRectangle(0, 0, SCREEN_WIDTH, 54);
    iSetColor(210, 210, 215);
    drawCenteredText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 62, (char*)"Rino's Game  |  HOLD SPACE to fly  |  Dodge and survive till 50 seconds", GLUT_BITMAP_8_BY_13);

    // big middle-top countdown (red in the last 5s)
    {
        char planeTimerText[16];
        int planeSecondsLeft = (planeTimerTicks + 62) / 63; // ceil(ticks / 62.5)
        if (planeSecondsLeft < 0) planeSecondsLeft = 0;
        sprintf_s(planeTimerText, "%d", planeSecondsLeft);
        if (planeSecondsLeft <= 5) iSetColor(220, 60, 60);
        else iSetColor(255, 255, 255);
        drawCenteredText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 112, planeTimerText, GLUT_BITMAP_TIMES_ROMAN_24);
        // difficulty level 1..4 rises with elapsed time (speed + spawn rate)
        {
            char planeLevelText[16];
            sprintf_s(planeLevelText, "Lv %d", planePhase + 1);
            iSetColor(200, 200, 205);
            drawCenteredText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 134, planeLevelText, GLUT_BITMAP_8_BY_13);
        }
    }

    // HP hearts (top-left under the header)
    iSetColor(255, 240, 180);
    iText(16, SCREEN_HEIGHT - 70 - 8 - 12, (char*)"HP", GLUT_BITMAP_HELVETICA_18);
    for (int heartI = 0; heartI < planeHp; heartI++) {
        int heartLeft = 16 + heartI * 42, heartBottom = SCREEN_HEIGHT - 70 - 8 - 36 - 16;
        if (planeHeartTexture) iShowImage(heartLeft, heartBottom, 36, 36, planeHeartTexture);
        else { iSetColor(220, 60, 60); iFilledRectangle(heartLeft, heartBottom, 36, 36); }
    }

    if (showCollisionBorders) {
        // live hitboxes: plane green, towers red — overlap = a counted touch
        int boxLeft, boxBottom, boxW, boxH;
        iSetColor(80, 220, 100);
        planeHitbox(&boxLeft, &boxBottom, &boxW, &boxH);
        iRectangle(boxLeft, boxBottom, boxW, boxH);
        iSetColor(220, 80, 80);
        for (int i = 0; i < PLANE_MAX_TOWERS; i++) if (planeTowers[i].active) {
            planeTowerHitbox(&planeTowers[i], &boxLeft, &boxBottom, &boxW, &boxH);
            iRectangle(boxLeft, boxBottom, boxW, boxH);
        }
    }

    // towers: top-hanging (head down) or bottom-standing (head up), scrolling left
    for (int i = 0; i < PLANE_MAX_TOWERS; i++) if (planeTowers[i].active) {
        int towerBottom = planeTowers[i].fromTop ? (PLANE_MAX_Y - planeTowers[i].h) : PLANE_MIN_Y;
        if (planeTowerTexture) iShowImage(planeTowers[i].x, towerBottom, PLANE_TOWER_W, planeTowers[i].h, planeTowerTexture);
        else { iSetColor(120, 40, 40); iFilledRectangle(planeTowers[i].x, towerBottom, PLANE_TOWER_W, planeTowers[i].h); }
    }

    // plane: slight tilt with climb/fall, flickers while invulnerable
    {
        int planeBottom = (int)planeY;
        int planeFlicker = (planeInvulnTicks > 0) ? ((animationTickCounter / 4) % 2) : 0;
        if (!planeFlicker) {
            double planeTilt = planeVY * 3.0;
            if (planeTilt > 20.0) planeTilt = 20.0;
            if (planeTilt < -20.0) planeTilt = -20.0;
            if (planeTexture) drawRotatedImage(PLANE_X, planeBottom, PLANE_W, PLANE_H, planeTexture, planeTilt);
            else { iSetColor(220, 200, 120); iFilledRectangle(PLANE_X, planeBottom, PLANE_W, PLANE_H); }
        }
    }

    if (dialogueTicksLeft > 0) drawPromptBox(dialogueText);
    else drawPromptBox("Dodge and survive till 50 seconds");
    // NOTE: no BACK button here — win goes to Library, lose returns to Rino
}

