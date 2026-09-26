// ------------------------------------------------------------------------
// PlaneGame.hpp  -  Rino's flappy-style plane minigame (own header, like
// Brawl.hpp / Battle.hpp). Hold SPACE to climb, release to fall, dodge the
// top-hanging / bottom-standing towers for 50 seconds. Win -> Library,
// lose (HP empty) -> back to the Rino talk scene in Classroom2 for retry.
// Uses only plane* / PLANE_* names: shares no state with any other scene.
// ------------------------------------------------------------------------
#pragma once

// Forward declarations: defined later in the single translation unit
// (SceneFlow.hpp and iMain.cpp). Declared here so this header can be
// included before them without creating an include cycle.
void switchToLibrary();
void showDialogue(const char* message);
void playClickSound();

// ---- Plane art (Assets//planeGame//, loaded once in main()) ----
unsigned int planeBgTexture = 0; // planeGame//background.png (full-screen sky)
unsigned int planeTexture = 0; // planeGame//plane.png (the flyer)
unsigned int planeTowerTexture = 0; // planeGame//tower.png (tower column)
unsigned int planeHeartTexture = 0; // planeGame//hp_heart.png (HP, top-left)

// ---- Tuning ----
const int PLANE_TIME_LIMIT_TICKS = 3125; // 50s at ~62.5 ticks/s (same math as scene 4/8)
const int PLANE_MAX_TOWERS = 8;
const int PLANE_X = 200; // plane stays at fixed X, world scrolls left
const int PLANE_W = 120, PLANE_H = 80;
const int PLANE_TOWER_W = 110;
const int PLANE_SPAWN_X = 1210; // just off the right edge (1200 wide)
const int PLANE_MIN_Y = 70, PLANE_MAX_Y = 700; // fly band (above bottom bar, below header)
const int PLANE_TOWER_MIN_H = 180, PLANE_TOWER_MAX_H = 420;
const int PLANE_START_HP = 3;
const int PLANE_INVULN_TICKS = 70; // ~1.1s mercy flicker after a hit

// Difficulty ramp: 4 phases over the 50s run. Speed/spawn/height rise with
// elapsed time, but towers stay single dodgeable columns and HP/invuln are
// untouched, so the late game is hotter without ever becoming unfair.
void planeDifficulty(int elapsedTicks, int* outSpeed, int* outSpawnEvery, int* outMaxH, int* outPhase) {
    int elapsedSec = elapsedTicks / 63; // ~62.5 ticks/s
    if (elapsedSec < 15) { *outSpeed = 5; *outSpawnEvery = 100; *outMaxH = 360; *outPhase = 0; }
    else if (elapsedSec < 30) { *outSpeed = 6; *outSpawnEvery = 90; *outMaxH = 400; *outPhase = 1; }
    else if (elapsedSec < 42) { *outSpeed = 7; *outSpawnEvery = 78; *outMaxH = 430; *outPhase = 2; }
    else { *outSpeed = 8; *outSpawnEvery = 65; *outMaxH = 450; *outPhase = 3; }
}

struct PlaneTower {
    int active; // 0 free, 1 scrolling
    int x; // left edge
    int h; // tower height in px
    int fromTop; // 1 hangs from the top (head down), 0 stands on the floor (head up)
    int hitCounted; // 1 once it has damaged the plane (no double hits)
};

// ---- Run state (reset on every entry, never read/written elsewhere) ----
PlaneTower planeTowers[PLANE_MAX_TOWERS] = {};
double planeY = 400.0; // bottom edge of the plane sprite
double planeVY = 0.0; // vertical velocity (+ = climbing)
int planeTimerTicks = 0; // counts down to the win
int planeHp = PLANE_START_HP; // hits remaining
int planeInvulnTicks = 0; // mercy flicker after a hit
int planeSpawnTimer = 0;
int planePhase = 0; // difficulty phase 0..3, derived from elapsed time
int savedPlaneClassroom2X = 576, savedPlaneClassroom2Y = 350; // Rino talk spot to restore on fail

// Inset AABB hitboxes (smaller than the sprites, so near-misses don't count.
// Tower art is mostly transparent side padding + a gap between spires, so its
// box is narrowed to the solid middle column and shaved top/bottom.)
const int PLANE_HIT_INSET_X = 20, PLANE_HIT_INSET_Y = 16;
const int PLANE_TOWER_HIT_INSET_X = 34;
const int PLANE_TOWER_HIT_INSET_Y = 30;
void planeHitbox(int* boxLeft, int* boxBottom, int* boxW, int* boxH) {
    *boxLeft = PLANE_X + PLANE_HIT_INSET_X;
    *boxBottom = (int)planeY + PLANE_HIT_INSET_Y;
    *boxW = PLANE_W - PLANE_HIT_INSET_X * 2;
    *boxH = PLANE_H - PLANE_HIT_INSET_Y * 2;
}
void planeTowerHitbox(const PlaneTower* tower, int* boxLeft, int* boxBottom, int* boxW, int* boxH) {
    *boxLeft = tower->x + PLANE_TOWER_HIT_INSET_X;
    *boxW = PLANE_TOWER_W - PLANE_TOWER_HIT_INSET_X * 2;
    if (tower->fromTop) *boxBottom = PLANE_MAX_Y - tower->h + PLANE_TOWER_HIT_INSET_Y;
    else *boxBottom = PLANE_MIN_Y + PLANE_TOWER_HIT_INSET_Y;
    *boxH = tower->h - PLANE_TOWER_HIT_INSET_Y * 2;
}
int planeTouchesTower(const PlaneTower* tower) {
    int planeLeft, planeBottom, planeW, planeH, towerLeft, towerBottom, towerW, towerH;
    planeHitbox(&planeLeft, &planeBottom, &planeW, &planeH);
    planeTowerHitbox(tower, &towerLeft, &towerBottom, &towerW, &towerH);
    return towerLeft < planeLeft + planeW && towerLeft + towerW > planeLeft &&
           towerBottom < planeBottom + planeH && towerBottom + towerH > planeBottom;
}

void spawnPlaneTower(int maxH) {
    if (maxH < PLANE_TOWER_MIN_H) maxH = PLANE_TOWER_MIN_H;
    for (int i = 0; i < PLANE_MAX_TOWERS; i++) if (!planeTowers[i].active) {
        planeTowers[i].active = 1;
        planeTowers[i].x = PLANE_SPAWN_X;
        planeTowers[i].h = PLANE_TOWER_MIN_H + rand() % (maxH - PLANE_TOWER_MIN_H + 1);
        planeTowers[i].fromTop = rand() % 2; // head from the top or from the bottom
        planeTowers[i].hitCounted = 0;
        break;
    }
}

void resetPlaneGame() {
    for (int i = 0; i < PLANE_MAX_TOWERS; i++) planeTowers[i].active = 0;
    planeY = 400.0;
    planeVY = 0.0;
    planeTimerTicks = PLANE_TIME_LIMIT_TICKS; // 50s countdown
    planeHp = PLANE_START_HP;
    planeInvulnTicks = 0;
    planeSpawnTimer = 40; // first tower arrives shortly after entry
    planePhase = 0;
    spawnPlaneTower(350); // gentle opening towers
    spawnPlaneTower(350);
}

void switchToPlaneGame() {
    // Entry from Rino's 2nd X: stash the talk spot, start fresh, no interstitial.
    savedPlaneClassroom2X = playerSquareX;
    savedPlaneClassroom2Y = playerSquareY;
    resetPlaneGame();
    playerIsMoving = 0;
    dialogueTicksLeft = 0;
    exitCooldownTicks = EXIT_COOLDOWN_TICKS; // held X must not leak into the minigame
    currentGameState = GAME_STATE_PLANEGAME;
}

void returnToClassroom2AfterFail() {
    // Lose: back to the Rino talk scene exactly where the run left it, retry ready.
    playerSquareX = savedPlaneClassroom2X;
    playerSquareY = savedPlaneClassroom2Y;
    playerFacing = 0;
    playerIsMoving = 0;
    rinoTalked = 0; // fresh Rino encounter: line first, accept next
    exitCooldownTicks = EXIT_COOLDOWN_TICKS;
    currentGameState = GAME_STATE_CLASSROOM2;
    showDialogue("Rino: That was close! Catch your breath and try again.");
}

// Per-tick update, called only from the GAME_STATE_PLANEGAME branch of fixedUpdate.
void updatePlaneGame() {
    if (dialogueTicksLeft > 0) dialogueTicksLeft--;
    if (planeInvulnTicks > 0) planeInvulnTicks--;

    // Win: survived the full 50s -> next state (Library, like Rino's old 2nd X).
    if (planeTimerTicks > 0) planeTimerTicks--;
    if (planeTimerTicks <= 0) {
        playClickSound();
        switchToLibrary();
        return;
    }

    // Flappy lift: holding SPACE climbs, released gravity pulls down.
    const double PLANE_GRAVITY = 0.45;
    const double PLANE_THRUST = 1.0;
    const double PLANE_MAX_RISE = 6.0, PLANE_MAX_FALL = -7.0;
    planeVY -= PLANE_GRAVITY;
    if (isKeyPressed(' ')) planeVY += PLANE_THRUST;
    if (planeVY > PLANE_MAX_RISE) planeVY = PLANE_MAX_RISE;
    if (planeVY < PLANE_MAX_FALL) planeVY = PLANE_MAX_FALL;
    planeY += planeVY;
    if (planeY < PLANE_MIN_Y) { planeY = PLANE_MIN_Y; planeVY = 0; }
    if (planeY > (double)(PLANE_MAX_Y - PLANE_H)) { planeY = (double)(PLANE_MAX_Y - PLANE_H); planeVY = 0; }

    // Difficulty for this tick from elapsed time (hotter, never unfair).
    // Phase-ups get a one-line Rino callout so the ramp never feels cheap.
    int planeSpeed = 4, planeSpawnEvery = 120, planeMaxH = 340, planeNewPhase = 0;
    planeDifficulty(PLANE_TIME_LIMIT_TICKS - planeTimerTicks, &planeSpeed, &planeSpawnEvery, &planeMaxH, &planeNewPhase);
    if (planeNewPhase > planePhase) {
        planePhase = planeNewPhase;
        playClickSound();
        if (planePhase == 1) showDialogue("Rino: Warm-up over, faster now!");
        else if (planePhase == 2) showDialogue("Rino: Halfway there, keep steady!");
        else if (planePhase == 3) showDialogue("Rino: Final stretch, hold on!");
    }

    // Scroll towers in from the right, retire past the left edge.
    if (planeSpawnTimer > 0) planeSpawnTimer--;
    if (planeSpawnTimer <= 0) {
        planeSpawnTimer = planeSpawnEvery;
        spawnPlaneTower(planeMaxH);
    }
    for (int i = 0; i < PLANE_MAX_TOWERS; i++) if (planeTowers[i].active) {
        planeTowers[i].x -= planeSpeed;
        if (planeTowers[i].x + PLANE_TOWER_W < 0) planeTowers[i].active = 0;
        // Tower touch: one damage per tower, then mercy invulnerability.
        if (planeTowers[i].active && !planeTowers[i].hitCounted && planeInvulnTicks <= 0 &&
            planeTouchesTower(&planeTowers[i])) {
            planeTowers[i].hitCounted = 1;
            planeInvulnTicks = PLANE_INVULN_TICKS;
            planeHp--;
            playClickSound();
            if (planeHp <= 0) {
                planeHp = 0;
                returnToClassroom2AfterFail();
                return;
            }
        }
    }

    // Give up: ESC returns to the Rino talk scene (no penalty beyond the retry).
    if (isKeyPressed(27)) {
        playClickSound();
        returnToClassroom2AfterFail();
        return;
    }
}
