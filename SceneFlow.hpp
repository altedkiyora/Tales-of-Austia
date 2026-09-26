// ------------------------------------------------------------------------
// SceneFlow.hpp  -  the arrows between screens.
// Presence/talk-zone checks, dialogue, book-quiz reading/grading, the
// switch/return/finish transition functions, debug warp, and the battle
// input/eval steps. Lives BEFORE Scenes so every renderer can call it.
// ------------------------------------------------------------------------
#pragma once

void switchToBigGate(); // forward: quiz success proceeds to the next scene
void playClickSound(); // forward: used by startBookQuiz
void answerBookQuiz(char pick); // forward: keyboard + mouse clicks answer too

void startInterstitial(const char* text, int nextState); // defined with the scene switches below
void startInterstitial3(const char* line1, const char* line2, const char* line3, int nextState);
void finishInterstitial();

void showDialogue(const char* message) {
    strcpy_s(dialogueText, message);
    dialogueTicksLeft = DIALOGUE_SHOW_TICKS;
}


int isSeniorInTalkRange() {
    int npcFeetX = SENIOR_NPC_X + SENIOR_NPC_W / 2;
    int npcFeetY = SENIOR_NPC_Y + 10;
    int dx = (playerSquareX + 24) - npcFeetX;
    int dy = playerSquareY - npcFeetY;
    return dx * dx + dy * dy < SENIOR_TALK_RADIUS * SENIOR_TALK_RADIUS;
}

int isPlayerAtNextFloor() {
    int feetX = playerSquareX + 24;
    int feetY = playerSquareY;
    return feetX >= NEXT_FLOOR_X0 - 10 && feetX <= NEXT_FLOOR_X1 + 10 &&
           feetY >= NEXT_FLOOR_Y - NEXT_FLOOR_BAND && feetY <= NEXT_FLOOR_Y + 20;
}

// Scene-5 back exit (top-right corner): X here returns to scene 4
int isPlayerAtBackExit() {
    int feetX = playerSquareX + 24;
    int feetY = playerSquareY;
    return feetX >= BACK_EXIT_X0 - 10 && feetX <= BACK_EXIT_X1 + 10 &&
           feetY >= BACK_EXIT_Y - BACK_EXIT_BAND && feetY <= BACK_EXIT_Y + 20;
}

int isPlayerAtVictoryExit() {
    return playerSquareX + 24 >= VICTORY_EXIT_X;
}

// Scene-6 door peek zone (bottom doors): X here peeks through the door
int isPlayerAtDoorZone() {
    int feetX = playerSquareX + 24;
    int feetY = playerSquareY;
    return feetX >= DOOR_ZONE_X0 - 10 && feetX <= DOOR_ZONE_X1 + 10 &&
           feetY >= DOOR_ZONE_Y - DOOR_ZONE_BAND && feetY <= DOOR_ZONE_Y + 20;
}

// Scene-6 up exit (top of left arm): X here advances to scene 7 (maze)
int isPlayerAtScene6UpExit() {
    int feetX = playerSquareX + 24;
    int feetY = playerSquareY;
    return feetX >= SCENE6_UP_X0 - 10 && feetX <= SCENE6_UP_X1 + 10 &&
           feetY >= SCENE6_UP_Y - SCENE6_UP_BAND && feetY <= SCENE6_UP_Y + 20;
}

// Scene-7 maze goal (center): X here advances to scene 8
int isPlayerAtMazeGoal() {
    int feetX = playerSquareX + 24;
    int feetY = playerSquareY;
    return feetX >= MAZE_GOAL_X0 - 10 && feetX <= MAZE_GOAL_X1 + 10 &&
           feetY >= MAZE_GOAL_Y0 && feetY <= MAZE_GOAL_Y1;
}

// BigGate access zone (step front center): X here enters the boss room
int isPlayerAtGateExit() {
    int feetX = playerSquareX + 24;
    int feetY = playerSquareY;
    return feetX >= GATE_EXIT_X0 - 10 && feetX <= GATE_EXIT_X1 + 10 &&
           feetY >= GATE_EXIT_Y - GATE_EXIT_BAND && feetY <= GATE_EXIT_Y + 20;
}

int isCrInTalkRange() {
    int npcFeetX = CR_NPC_X + CR_NPC_W / 2;
    int npcFeetY = CR_NPC_Y + 10;
    int dx = (playerSquareX + 24) - npcFeetX;
    int dy = playerSquareY - npcFeetY;
    return dx * dx + dy * dy < CR_TALK_RADIUS * CR_TALK_RADIUS;
}

int isNpcInTalkRange(int npcX, int npcY, int npcW) {
    int npcFeetX = npcX + npcW / 2;
    int npcFeetY = npcY + 10;
    int dx = (playerSquareX + 24) - npcFeetX;
    int dy = playerSquareY - npcFeetY;
    return dx * dx + dy * dy < CLASS_TALK_RADIUS * CLASS_TALK_RADIUS;
}

int isMontInTalkRange() { return isNpcInTalkRange(MONT_NPC_X, MONT_NPC_Y, MONT_NPC_W); }
int isNoplaInTalkRange() { return isNpcInTalkRange(NOPLA_NPC_X, NOPLA_NPC_Y, NOPLA_NPC_W); }
int isClassCrInTalkRange() { return isNpcInTalkRange(CLASSCR_NPC_X, CLASSCR_NPC_Y, CLASSCR_NPC_W); }
int isRinoInTalkRange() { return isNpcInTalkRange(RINO_NPC_X, RINO_NPC_Y, RINO_NPC_W); }
int isLibrary2BookInRange() { return isNpcInTalkRange(LIBRARY2_BOOK_X, LIBRARY2_BOOK_Y, LIBRARY2_BOOK_W); }
int isBossInTalkRange() { return isNpcInTalkRange(BOSS_THRONE_X, BOSS_THRONE_Y, BOSS_THRONE_W); }

// ---- Book quiz file I/O (C concepts: fopen / fgets / fclose) ----
int loadBookQuiz() {
    bookQuizLineCount = 0; bookQuizTotal = 0; bookQuizIndex = 0;
    bookQuizMessage[0] = 0; bookQuizFeedbackTicks = 0; bookQuizPending = 0;
    for (int i = 0; i < BOOK_QUIZ_MAX_Q; i++) {
        bookQuizQText[i][0] = 0; bookQuizOptA[i][0] = 0; bookQuizOptB[i][0] = 0;
        bookQuizCorrect[i] = 0;
    }
    FILE* qf = fopen("Assets//afterScene10//Questions.txt", "r");
    if (!qf) { sprintf_s(bookQuizMessage, "Missing Questions.txt (ESC to go back)"); return 0; }
    char line[256];
    while (bookQuizLineCount < BOOK_QUIZ_MAX_LINES && fgets(line, sizeof(line), qf)) {
        size_t len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) line[--len] = 0;
        strncpy_s(bookQuizLines[bookQuizLineCount], line, _TRUNCATE);
        bookQuizLineCount++;
    }
    fclose(qf);
    // parse lines into per-question blocks: "N. text" starts a question,
    // following non-empty lines form the options blob which is split at "B."
    int qi = 0;
    for (int li = 0; li < bookQuizLineCount && qi < BOOK_QUIZ_MAX_Q; li++) {
        char* ln = bookQuizLines[li];
        if (ln[0] == 0 || ln[0] == '*') continue;
        if (!(ln[0] >= '0' && ln[0] <= '9')) continue;
        strncpy_s(bookQuizQText[qi], ln, _TRUNCATE);
        char blob[320] = "";
        for (int lj = li + 1; lj < bookQuizLineCount; lj++) {
            char* nx = bookQuizLines[lj];
            if (nx[0] == 0) break;
            if (nx[0] >= '0' && nx[0] <= '9') break;
            if (nx[0] == '*') break;
            if (blob[0]) strncat_s(blob, "  ", _TRUNCATE);
            strncat_s(blob, nx, _TRUNCATE);
        }
        // trim leading spaces of blob
        char* b = blob;
        while (*b == ' ' || *b == '\t') b++;
        char* split = strstr(b, "B.");
        if (!split) split = strstr(b, "B ");
        if (split && split > b) {
            size_t aLen = (size_t)(split - b);
            while (aLen > 0 && (b[aLen - 1] == ' ' || b[aLen - 1] == '\t')) aLen--;
            if (aLen >= sizeof(bookQuizOptA[qi])) aLen = sizeof(bookQuizOptA[qi]) - 1;
            strncpy_s(bookQuizOptA[qi], sizeof(bookQuizOptA[qi]), b, aLen);
            strncpy_s(bookQuizOptB[qi], split, _TRUNCATE);
        } else if (b[0]) {
            strncpy_s(bookQuizOptA[qi], b, _TRUNCATE);
            sprintf_s(bookQuizOptB[qi], "B");
        } else {
            sprintf_s(bookQuizOptA[qi], "A");
            sprintf_s(bookQuizOptB[qi], "B");
        }
        qi++;
    }
    FILE* af = fopen("Assets//afterScene10//Answers.txt", "r");
    if (!af) { sprintf_s(bookQuizMessage, "Missing Answers.txt (ESC to go back)"); return 0; }
    while (bookQuizTotal < BOOK_QUIZ_MAX_Q && fgets(line, sizeof(line), af)) {
        char ans = 0;
        for (int i = 0; line[i]; i++) {
            if (isalpha((unsigned char)line[i])) { ans = (char)toupper((unsigned char)line[i]); break; }
        }
        if (ans == 'A' || ans == 'B') bookQuizCorrect[bookQuizTotal++] = ans;
    }
    fclose(af);
    if (bookQuizTotal == 0) { sprintf_s(bookQuizMessage, "No answers found (ESC to go back)"); return 0; }
    for (int i = 0; i < bookQuizTotal; i++) {
        if (bookQuizQText[i][0] == 0) sprintf_s(bookQuizQText[i], "Question %d", i + 1);
        if (bookQuizOptA[i][0] == 0) sprintf_s(bookQuizOptA[i], "A");
        if (bookQuizOptB[i][0] == 0) sprintf_s(bookQuizOptB[i], "B");
    }
    return 1;
}

void startBookQuiz() {
    loadBookQuiz(); // fills questions + answers via file I/O, resets progress
    playClickSound();
    dialogueTicksLeft = 0;
    exitCooldownTicks = EXIT_COOLDOWN_TICKS;
    currentGameState = GAME_STATE_BOOKQUIZ;
}

// One answer: immediate you are correct / you are wrong, then next / restart / finish.
void answerBookQuiz(char pick) {
    if (currentGameState != GAME_STATE_BOOKQUIZ) return;
    if (bookQuizTotal <= 0 || bookQuizIndex >= bookQuizTotal) return;
    if (bookQuizFeedbackTicks > 0) return; // feedback showing: input locked
    if (pick != 'A' && pick != 'B') return;
    playClickSound();
    if (pick == bookQuizCorrect[bookQuizIndex]) {
        sprintf_s(bookQuizMessage, "you are correct");
        bookQuizPending = (bookQuizIndex + 1 >= bookQuizTotal) ? 3 : 1;
    } else {
        sprintf_s(bookQuizMessage, "you are wrong");
        bookQuizPending = 2;
    }
    bookQuizFeedbackTicks = 100; // ~1.6s to read the feedback
}

int isInteractKeyPressed() {
    return isKeyPressed('x') || isKeyPressed('X');
}

// True while pushing against the top-border gate (same zone that X enters).
int isPlayerAtGateDoor() {
    int feetCenterX = playerSquareX + 24;
    return feetCenterX >= GATE_X0 && feetCenterX <= GATE_X1 && playerSquareY >= GATE_TRIGGER_Y - 12;
}

int playerDistSqTo(int pointX, int pointY) {
    int dx = (playerSquareX + 24) - pointX;
    int dy = playerSquareY - pointY;
    return dx * dx + dy * dy;
}

void startMusicLoop(const char* alias); // gated music starters, defined below
void restartMusic(const char* alias);
void stopAllMusic();
void startStageMusic(); // aftercheckpoint past Online, menumusic before (defined below)
void returnToMainMenu(); // defined with the menu switches below
void playClickSound();

void returnToPlayScene() {
    playerSquareX = savedScene1X;
    playerSquareY = savedScene1Y;
    playerFacing = 0;
    playerIsMoving = 0;
    exitCooldownTicks = EXIT_COOLDOWN_TICKS;
    currentGameState = GAME_STATE_PLAY;
}

// scene-2 position saved on next-floor entry, restored on return
int savedScene2X = 576, savedScene2Y = 350;
void switchToScene3() {
    savedScene2X = playerSquareX;
    savedScene2Y = playerSquareY;
    playerSquareX = SCREEN_WIDTH / 2 - 24;
    playerSquareY = 350;
    playerFacing = 0;
    playerIsMoving = 0;
    dialogueTicksLeft = 0;
    seniorPenQueued = 0;
    crHasTalked = 0;
    castAnimTicksLeft = 0;
    startInterstitial("the void is watching ", GAME_STATE_SCENE3);
}
void returnToScene2() {
    playerSquareX = savedScene2X;
    playerSquareY = savedScene2Y;
    playerFacing = 1;
    playerIsMoving = 0;
    dialogueTicksLeft = 0;
    seniorPenQueued = 0;
    exitCooldownTicks = EXIT_COOLDOWN_TICKS;
    currentGameState = GAME_STATE_SCENE2;
}

// Cast -> scene 4 flow: 2nd X in scene 3 enters TRANSITION (hero-sized cast at hero spot),
// then auto-switches to SCENE4 (Main//scene4.jpg) when the timer runs out.
void startCastAnimation() {
    castAnimTicksLeft = CAST_ANIM_SHOW_TICKS;
    dialogueTicksLeft = 0;
    playerIsMoving = 0;
    currentGameState = GAME_STATE_TRANSITION;
}
void spawnHolyNovaAt(int novaX, int novaY) {
    for (int i = 0; i < HOLY_NOVA_MAX; i++) if (!holyNovas[i].active) {
        holyNovas[i].active = 1;
        holyNovas[i].state = 0; // falling
        holyNovas[i].x = novaX;
        holyNovas[i].y = novaY;
        holyNovas[i].animTick = 0;
        holyNovas[i].hitCounted = 0;
        break;
    }
}
int holyNovaRandomX() {
    return SCENE4_MIN_FEET_X + rand() % (SCENE4_MAX_FEET_X - SCENE4_MIN_FEET_X - HOLY_NOVA_W);
}
void switchToScene4() {
    savedScene3X = playerSquareX;
    savedScene3Y = playerSquareY;
    castAnimTicksLeft = 0;
    playerSquareX = SCREEN_WIDTH / 2 - 24;
    playerSquareY = 314; // feet mid-ledge (SCENE4_MIN_Y..SCENE4_MAX_Y)
    scene4TimerTicks = SCENE4_TIME_LIMIT_TICKS; // 15s countdown -> scene 5
    heroHitCount = 0; // fresh HP bar for the minigame run
    heroHpStage = 0;
    // fresh meteor shower: clear + pre-seed a few mid-fall so the sky is alive on entry
    for (int i = 0; i < HOLY_NOVA_MAX; i++) holyNovas[i].active = 0;
    holyNovaSpawnTimer = HOLY_NOVA_SPAWN_EVERY;
    for (int s = 0; s < 4; s++) {
        // keep pre-seeds away from the hero: no free hits on arrival
        int seedX = holyNovaRandomX();
        for (int tries = 0; tries < 8 && abs((seedX + HOLY_NOVA_W / 2) - (playerSquareX + 24)) < 170; tries++)
            seedX = holyNovaRandomX();
        spawnHolyNovaAt(seedX, SCENE4_MAX_Y + rand() % (SCREEN_HEIGHT - SCENE4_MAX_Y));
    }
    playerFacing = 0;
    playerIsMoving = 0;
    dialogueTicksLeft = 0;
    startInterstitial("the void is watching ", GAME_STATE_SCENE4);
}
void returnToScene3() {
    playerSquareX = savedScene3X;
    playerSquareY = savedScene3Y;
    playerFacing = 1;
    playerIsMoving = 0;
    dialogueTicksLeft = 0;
    castAnimTicksLeft = 0;
    exitCooldownTicks = EXIT_COOLDOWN_TICKS;
    currentGameState = GAME_STATE_SCENE3;
}
void switchToScene5() {
    // one-way: no saved position, no way back to scene 4 (hall)
    playerSquareX = SCREEN_WIDTH / 2 - 24;
    playerSquareY = 350;
    playerFacing = 0;
    playerIsMoving = 0;
    dialogueTicksLeft = 0;
    // black interstitial (RBD) plays first; checkpoint music starts when it lands
    startInterstitial("the void is watching ", GAME_STATE_SCENE5);
}
// Black interstitial with per-transition text + RBD. Callers do their position
// setup first, then route here instead of setting currentGameState directly.
void startInterstitial(const char* text, int nextState) {
    strcpy_s(interstitialText, text);
    interstitialLine2[0] = 0; // single-line by default (3-line set by startInterstitial3)
    interstitialLine3[0] = 0;
    interstitialTicksLeft = INTERSTITIAL_SHOW_TICKS;
    interstitialNextState = nextState;
    dialogueTicksLeft = 0;
    playerIsMoving = 0;
    exitCooldownTicks = EXIT_COOLDOWN_TICKS;
    mciSendString("stop menumusic", NULL, 0, NULL);
    mciSendString("stop aftercheckpoint", NULL, 0, NULL);
    restartMusic("rbd");
    currentGameState = GAME_STATE_INTERSTITIAL;
}
void startInterstitial3(const char* line1, const char* line2, const char* line3, int nextState) {
    startInterstitial(line1, nextState);
    strcpy_s(interstitialLine2, line2);
    strcpy_s(interstitialLine3, line3);
}
// Scene-6 door peek: black screen epitaph, then back to scene 6 automatically
void startDoorPeek() {
    startInterstitial3("Here lies the SR and he is knocked out",
                       "with an empty wallet after an intro",
                       "session with immediates.",
                       GAME_STATE_SCENE6);
}
void finishInterstitial() {
    mciSendString("stop rbd", NULL, 0, NULL);
    int nextState = interstitialNextState;
    interstitialNextState = -1;
    // checkpoint loop runs from the end of Online till run end, menu loop before that
    if (checkpointMusic || nextState == GAME_STATE_SCENE5) {
        startStageMusic();
        if (nextState == GAME_STATE_SCENE5)
            showDialogue("cr gelo koi???"); // hero notices CR is gone (shown on arrival)
    } else
        startMusicLoop("menumusic");
    if (nextState == GAME_STATE_SCENE9) {
        playerSquareX = 1026; // feet (1050,660) top-right inside the scene-2 loop
        playerSquareY = 660;
        playerFacing = 1;
        playerIsMoving = 0;
        dialogueTicksLeft = 0;
        crAttackTalked = 0; // fresh encounter: old CR talks first
        crTransformed = 0;
        crTransformTicks = 0;
    }
    currentGameState = nextState;
}
void switchToScene6() {
    // one-way door: no saved position, no way back to scene 5
    playerSquareX = SCREEN_WIDTH / 2 - 24;
    playerSquareY = 300; // feet (600,300) inside the scene-6 bottom hall
    playerFacing = 0;
    playerIsMoving = 0;
    dialogueTicksLeft = 0;
    penRealizationQueued = 0;
    startInterstitial("the void is watching ", GAME_STATE_SCENE6);
}
void switchToScene10() {
    // post-Aftermath floor: same doorOpen.jpg layout, distinct state (no quiz exit)
    playerSquareX = SCREEN_WIDTH / 2 - 24;
    playerSquareY = 300; // feet (600,300) inside the bottom hall
    playerFacing = 0;
    playerIsMoving = 0;
    dialogueTicksLeft = 0;
    penRealizationQueued = 0;
    startInterstitial("the void is watching ", GAME_STATE_SCENE10);
}
void switchToClassroom1() {
    // next floor of classroom comes up (one-way, no way back to scene 10)
    playerSquareX = SCREEN_WIDTH / 2 - 24;
    playerSquareY = 350; // feet (600,350) inside the scene-2 loop
    playerFacing = 0;
    playerIsMoving = 0;
    dialogueTicksLeft = 0;
    penRealizationQueued = 0;
    seniorPenQueued = 0;
    rinoTalked = 0;
    startInterstitial("You still think you are made for this? these floors are real?", GAME_STATE_CLASSROOM1);
}
void switchToClassroom2() {
    // same classroom, second half: no black screen, no interstitial (scrollable map)
    playerSquareX = SCREEN_WIDTH / 2 - 24;
    playerSquareY = 350; // feet (600,350) inside the scene-2 loop
    playerFacing = 0;
    playerIsMoving = 0;
    dialogueTicksLeft = 0;
    rinoTalked = 0; // fresh Rino encounter: line first, library next
    exitCooldownTicks = EXIT_COOLDOWN_TICKS; // held X must not fire twice on arrival
    currentGameState = GAME_STATE_CLASSROOM2;
}
void switchToLibrary() {
    // Rino beat over: snap out of the classroom dream into the big library (first half)
    playerSquareX = SCREEN_WIDTH / 2 - 24;
    playerSquareY = 350; // feet (600,350) inside the scene-2 loop
    playerFacing = 0;
    playerIsMoving = 0;
    dialogueTicksLeft = 0;
    rinoTalked = 0;
    startInterstitial("You cant be this naive, snap out of your head", GAME_STATE_LIBRARY);
}
void switchToLibrary2() {
    // same library, second half: no black screen, no interstitial (scrollable map)
    playerSquareX = SCREEN_WIDTH / 2 - 24;
    playerSquareY = 350; // feet (600,350) inside the scene-2 loop
    playerFacing = 0;
    playerIsMoving = 0;
    dialogueTicksLeft = 0;
    exitCooldownTicks = EXIT_COOLDOWN_TICKS; // held X must not fire twice on arrival
    currentGameState = GAME_STATE_LIBRARY2;
}
void switchToBigGate() {
    // next floor after the library: gate courtyard (one-way, no way back)
    playerSquareX = SCREEN_WIDTH / 2 - 24;
    playerSquareY = 120; // feet (600,120) inside the bigGate loop
    playerFacing = 0;
    playerIsMoving = 0;
    dialogueTicksLeft = 0;
    startInterstitial("the void is watching ", GAME_STATE_BIGGATE);
}
void switchToBossRoom() {
    // through the gate: throne hall (one-way, no way back)
    playerSquareX = SCREEN_WIDTH / 2 - 24;
    playerSquareY = 120; // feet (600,120) inside the bossRoom loop
    playerFacing = 0;
    playerIsMoving = 0;
    dialogueTicksLeft = 0;
    bossChallenged = 0; // fresh throne encounter: line first, battle next
    startInterstitial("the void is watching ", GAME_STATE_BOSSROOM);
}
void switchToScene8() {
    // race entry: right-top spawn facing the top-left goal, HP set to 80%
    playerSquareX = 1026; // feet (1050,660) inside the scene-2 loop
    playerSquareY = 660;
    playerFacing = 3; // facing W toward the goal
    playerIsMoving = 0;
    dialogueTicksLeft = 0;
    penRealizationQueued = 0;
    scene8TimerTicks = SCENE8_TIME_LIMIT_TICKS; // 10s countdown
    scene8GoalReached = 0;
    heroHitCount = 0;
    heroHpStage = 1; // 80% -> dragon_part_2
    startInterstitial("the void is watching ", GAME_STATE_SCENE8);
}
void returnToScene7() {
    // voluntary return (BACK/ESC): maze entry, no penalty
    playerSquareX = 976; // feet (1000,660)
    playerSquareY = 660;
    playerFacing = 1;
    playerIsMoving = 0;
    dialogueTicksLeft = 0;
    penRealizationQueued = 0;
    exitCooldownTicks = EXIT_COOLDOWN_TICKS;
    startStageMusic();
    currentGameState = GAME_STATE_SCENE7;
}
void returnToScene7AfterFail() {
    // race fail: cast hangover, late line, -1 HP stage (one fifth)
    returnToScene7();
    if (heroHpStage < HERO_HP_STAGES - 1) heroHpStage++;
    showDialogue("i was late to arrive");
}
void switchToScene7() {
    // maze entry: top-right start, one-way (no way back to scene 6)
    playerSquareX = 976; // feet (1000,660): inside loop, clear of the x=1044 wall run
    playerSquareY = 660;
    playerFacing = 1; // facing down into the maze
    playerIsMoving = 0;
    dialogueTicksLeft = 0;
    penRealizationQueued = 0;
    startInterstitial("QOIZ STARTS", GAME_STATE_SCENE7);
}
// Black-screen sword splash before the finale battle (stage music keeps playing)
void startSplash() {
    splashTicksLeft = SPLASH_SHOW_TICKS;
    dialogueTicksLeft = 0;
    playerIsMoving = 0;
    exitCooldownTicks = EXIT_COOLDOWN_TICKS;
    currentGameState = GAME_STATE_SPLASH;
}
void resetBattle() {
    enemyHpStage = 0;
    heroHpStage = 0; // final battle starts on a full bar
    smiteCooldown = 0;
    shieldActive = 0;
    battlePhase = PHASE_INPUT;
    battleResult = 0;
    battleAnimTick = 0;
    dialogueTicksLeft = 0;
}
void switchToBattle() {
    // MC left facing the foe, foe on the right — Pokemon staging, closed in
    playerSquareX = 426; // feet (450,300)
    playerSquareY = 300;
    playerFacing = 2;
    playerIsMoving = 0;
    resetBattle();
    exitCooldownTicks = EXIT_COOLDOWN_TICKS;
    startStageMusic();
    currentGameState = GAME_STATE_BATTLE;
}
void startLastBrawl() {
    // ==== LASTBRAWL revertable block 3/4: entry (throne challenge -> trial -> brawl) ====
    brawlHeroX = 300; brawlBossX = 800;
    brawlHeroHp = 0; brawlBossHp = 0;
    brawlHeroFaceR = 1; brawlBossFaceL = 1; // spawn sides: hero left, boss right
    brawlHeroAtkTick = 0; brawlHeroCooldown = 0;
    brawlBossAtkTick = 0; brawlBossCooldown = 90; // opening beat before the first swipe
    brawlSwordTick = 0; brawlNovaTick = 0; brawlOver = 0;
    bossChallenged = 0;
    playerFacing = 2; playerIsMoving = 0; dialogueTicksLeft = 0;
    startInterstitial("Will you be able to overcome your trial this time, my liege?", GAME_STATE_LASTBRAWL);
    // ==== end LASTBRAWL block 3/4 ====
}
void enterVictory() {
    // post-fight free roam on the boss map (hero stays where the fight ended)
    crDefeated = 1; // the scene-6 door now tells a different story
    playerIsMoving = 0;
    dialogueTicksLeft = 0;
    penRealizationQueued = 0;
    exitCooldownTicks = EXIT_COOLDOWN_TICKS;
    startStageMusic();
    currentGameState = GAME_STATE_VICTORY;
}
void battleChooseMove(int move) {
    if (currentGameState != GAME_STATE_BATTLE || battlePhase != PHASE_INPUT) return;
    if (move == MOVE_SMITE && smiteCooldown > 0) {
        playClickSound();
        showDialogue("SMITE RECHARGING!");
        return;
    }
    battleSelectedMove = move;
    battleAnimTick = 0;
    battlePhase = PHASE_PLAYER_ANIM;
    playClickSound();
}
unsigned int battleMoveFrame(int move, int frame) {
    if (move == MOVE_SLASH) return moveSlashTex[frame];
    if (move == MOVE_SMITE) return moveSmiteTex[frame];
    if (move == MOVE_HEAL) return moveHealTex[frame];
    return moveShieldTex[frame];
}
void applyPlayerMove() {
    if (battleSelectedMove == MOVE_SLASH) enemyHpStage += 2;
    else if (battleSelectedMove == MOVE_SMITE) { enemyHpStage += 3; smiteCooldown = 2; }
    else if (battleSelectedMove == MOVE_HEAL) { heroHpStage -= 2; if (heroHpStage < 0) heroHpStage = 0; }
    else if (battleSelectedMove == MOVE_SHIELD) shieldActive = 1;
    playClickSound(); // impact feedback
    if (enemyHpStage >= HERO_HP_STAGES) {
        enemyHpStage = HERO_HP_STAGES - 1; // cap at empty bar (index 7, never 8)
        battlePhase = PHASE_OVER;
        battleResult = 1;
        showDialogue("CR DEFEATED! THE VOID RELEASES YOU.");
    } else {
        battlePhase = PHASE_ENEMY_ANIM;
        battleAnimTick = 0;
    }
}
void applyEnemyMove() {
    int foeDamage = shieldActive ? 0 : (1 + rand() % 2);
    heroHpStage += foeDamage;
    shieldActive = 0;
    if (smiteCooldown > 0) smiteCooldown--;
    playClickSound();
    if (heroHpStage >= HERO_HP_STAGES) {
        heroHpStage = HERO_HP_STAGES - 1; // cap at empty bar (index 7, never 8)
        battlePhase = PHASE_OVER;
        battleResult = 2;
        showDialogue("MC FAINTED... PRESS X TO REMATCH.");
    } else {
        battlePhase = PHASE_INPUT;
    }
}

// ---- Debug warp (testing cheat): N jumps instantly to the next scene,
// bypassing requirements and the interstitial. BACK stays coherent via saves.
void debugWarpTo(int scene) {
    stopAllMusic();
    dialogueTicksLeft = 0;
    penRealizationQueued = 0;
    seniorPenQueued = 0;
    rinoTalked = 0;
    castAnimTicksLeft = 0;
    crHasTalked = 0;
    exitCooldownTicks = EXIT_COOLDOWN_TICKS;
    playerIsMoving = 0;
    hasIdCard = 1; // never trap a warp tester behind the scene-1 gate
    if (scene == GAME_STATE_PLAY) {
        playerSquareX = SCREEN_WIDTH / 2 - 24; playerSquareY = SCREEN_HEIGHT / 2 - 24;
        playerFacing = 0;
    } else if (scene == GAME_STATE_SCENE2 || scene == GAME_STATE_SCENE3 || scene == GAME_STATE_SCENE5) {
        playerSquareX = SCREEN_WIDTH / 2 - 24; playerSquareY = 350;
        playerFacing = 0;
    } else if (scene == GAME_STATE_SCENE4) {
        playerSquareX = SCREEN_WIDTH / 2 - 24; playerSquareY = 314;
        playerFacing = 0;
        scene4TimerTicks = SCENE4_TIME_LIMIT_TICKS;
        heroHitCount = 0; heroHpStage = 0;
        for (int i = 0; i < HOLY_NOVA_MAX; i++) holyNovas[i].active = 0;
        holyNovaSpawnTimer = HOLY_NOVA_SPAWN_EVERY;
        for (int s = 0; s < 4; s++) spawnHolyNovaAt(holyNovaRandomX(), SCENE4_MAX_Y + rand() % (SCREEN_HEIGHT - SCENE4_MAX_Y));
    } else if (scene == GAME_STATE_SCENE6) {
        playerSquareX = SCREEN_WIDTH / 2 - 24; playerSquareY = 300;
        playerFacing = 0;
    } else if (scene == GAME_STATE_SCENE7) {
        playerSquareX = 976; playerSquareY = 660;
        playerFacing = 1;
    } else if (scene == GAME_STATE_SCENE8) {
        playerSquareX = 1026; playerSquareY = 660;
        playerFacing = 3;
        scene8TimerTicks = SCENE8_TIME_LIMIT_TICKS;
        scene8GoalReached = 0;
        heroHitCount = 0;
        heroHpStage = 1;
    } else if (scene == GAME_STATE_SCENE9) {
        playerSquareX = 1026; playerSquareY = 660; // feet (1050,660) top-right
        playerFacing = 1;
        crAttackTalked = 0; // fresh encounter: old CR talks first
        crTransformed = 0;
        crTransformTicks = 0;
    } else if (scene == GAME_STATE_SCENE10) {
        playerSquareX = SCREEN_WIDTH / 2 - 24; playerSquareY = 300;
        playerFacing = 0;
    } else if (scene == GAME_STATE_CLASSROOM1 || scene == GAME_STATE_CLASSROOM2 || scene == GAME_STATE_LIBRARY || scene == GAME_STATE_LIBRARY2) {
        playerSquareX = SCREEN_WIDTH / 2 - 24; playerSquareY = 350;
        playerFacing = 0;
        rinoTalked = 0; // fresh Rino encounter
    } else if (scene == GAME_STATE_PLANEGAME) {
        resetPlaneGame(); // fresh 50s run, 3 HP, towers cleared
        playerFacing = 0;
        playerIsMoving = 0;
        dialogueTicksLeft = 0;
        exitCooldownTicks = EXIT_COOLDOWN_TICKS;
    } else if (scene == GAME_STATE_BIGGATE || scene == GAME_STATE_BOSSROOM) {
        playerSquareX = SCREEN_WIDTH / 2 - 24; playerSquareY = 120;
        playerFacing = 0;
        bossChallenged = 0; // fresh throne encounter
    }
    // warp past Online lands in the checkpoint loop, like a real run would
    checkpointMusic = (scene != GAME_STATE_MENU && scene != GAME_STATE_ABOUT &&
                       scene != GAME_STATE_PLAY && scene != GAME_STATE_SCENE2 &&
                       scene != GAME_STATE_SCENE3 && scene != GAME_STATE_INTERSTITIAL);
    startStageMusic();
    printf("WARP -> %d\n", scene);
    currentGameState = scene;
}
void debugWarpNext() {
    // stash current spot so BACK still works after a warp
    if (currentGameState == GAME_STATE_MENU || currentGameState == GAME_STATE_ABOUT) debugWarpTo(GAME_STATE_PLAY);
    else if (currentGameState == GAME_STATE_PLAY) { savedScene1X = playerSquareX; savedScene1Y = playerSquareY; debugWarpTo(GAME_STATE_SCENE2); }
    else if (currentGameState == GAME_STATE_SCENE2) { savedScene2X = playerSquareX; savedScene2Y = playerSquareY; debugWarpTo(GAME_STATE_SCENE3); }
    else if (currentGameState == GAME_STATE_SCENE3) { savedScene3X = playerSquareX; savedScene3Y = playerSquareY; debugWarpTo(GAME_STATE_SCENE4); }
    else if (currentGameState == GAME_STATE_SCENE4) { savedScene4X = playerSquareX; savedScene4Y = playerSquareY; debugWarpTo(GAME_STATE_SCENE5); }
    else if (currentGameState == GAME_STATE_SCENE5) debugWarpTo(GAME_STATE_SCENE6);
    else if (currentGameState == GAME_STATE_SCENE6) debugWarpTo(GAME_STATE_SCENE7);
    else if (currentGameState == GAME_STATE_SCENE7) debugWarpTo(GAME_STATE_SCENE8);
    else if (currentGameState == GAME_STATE_SCENE8) debugWarpTo(GAME_STATE_SCENE9);
    else if (currentGameState == GAME_STATE_SCENE9) switchToBattle();
    else if (currentGameState == GAME_STATE_SPLASH) { /* let it land */ }
    else if (currentGameState == GAME_STATE_BATTLE) enterVictory();
    else if (currentGameState == GAME_STATE_VICTORY) debugWarpTo(GAME_STATE_SCENE10);
    else if (currentGameState == GAME_STATE_SCENE10) debugWarpTo(GAME_STATE_CLASSROOM1);
    else if (currentGameState == GAME_STATE_CLASSROOM1) debugWarpTo(GAME_STATE_CLASSROOM2);
    else if (currentGameState == GAME_STATE_CLASSROOM2) debugWarpTo(GAME_STATE_PLANEGAME);
    else if (currentGameState == GAME_STATE_PLANEGAME) debugWarpTo(GAME_STATE_LIBRARY);
    else if (currentGameState == GAME_STATE_LIBRARY) debugWarpTo(GAME_STATE_LIBRARY2);
    else if (currentGameState == GAME_STATE_LIBRARY2) debugWarpTo(GAME_STATE_BIGGATE);
    else if (currentGameState == GAME_STATE_BIGGATE) debugWarpTo(GAME_STATE_BOSSROOM);
    else if (currentGameState == GAME_STATE_BOSSROOM) returnToMainMenu();
    // TRANSITION / INTERSTITIAL: ignored (let them finish)
}

