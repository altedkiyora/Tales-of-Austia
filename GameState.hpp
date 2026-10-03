// ------------------------------------------------------------------------
// GameState.hpp  -  the mutable handbag of the run.
// Everything that changes while playing lives here: quest/barrier flags,
// cooldowns, position saves, dialogue text, the holy-nova minigame data,
// dragon-HP counters, the music toggle, ID-card/gate state.
// ------------------------------------------------------------------------
#pragma once

// ---- Book quiz (black screen, C file I/O from Questions.txt / Answers.txt) ----
// One question at a time in a dialogue box + 2 UI option buttons (A/B).
// Every answer shows "you are correct" / "you are wrong"; wrong restarts at Q1.
const int BOOK_QUIZ_MAX_LINES = 30, BOOK_QUIZ_LINE_LEN = 160, BOOK_QUIZ_MAX_Q = 10;
char bookQuizLines[30][160];
int bookQuizLineCount = 0;
char bookQuizQText[10][160];
char bookQuizOptA[10][96];
char bookQuizOptB[10][96];
char bookQuizCorrect[10];
int bookQuizTotal = 0, bookQuizIndex = 0;
char bookQuizMessage[160] = "";
int bookQuizFeedbackTicks = 0; // pause showing you are correct / you are wrong
int bookQuizPending = 0; // 0 none, 1 next Q, 2 restart at Q1, 3 finish -> big gate

// ---- HolyNova meteor shower (scene 4): 01 falls from above, 02..07+09+10 burst on floor hit
const int HOLY_NOVA_FRAMES = 9; // 01..07, 09, 10 (08 missing on disk)
unsigned int holyNovaTexture[HOLY_NOVA_FRAMES] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
const int HOLY_NOVA_MAX = 12;
const int HOLY_NOVA_FALL_SPEED = 6; // px/tick downward
const int HOLY_NOVA_SPAWN_EVERY = 25; // ticks between spawns
const int HOLY_NOVA_BURST_TICKS_PER_FRAME = 6;
const int HOLY_NOVA_W = 128, HOLY_NOVA_H = 64;
struct HolyNova { int active; int x; int y; int state; int animTick; int hitCounted; }; // state 0=falling, 1=burst
HolyNova holyNovas[HOLY_NOVA_MAX] = {};
int holyNovaSpawnTimer = 0;

const int HITS_PER_HP_STAGE = 5;
const int HERO_HP_STAGES = 8;
int heroHitCount = 0;
int heroHpStage = 0; // 0 = full (part_1) .. 7 = empty (part_8)
int penRealizationQueued = 0; // retired (pen beat removed): always 0, kept for save coherence
int seniorPenQueued = 0; // 1 after senior talk, shows Pen unlock once senior line fades
int crHasTalked = 0; // 1 after first CR dialogue was shown (enables 2nd X for cast)
int castAnimTicksLeft = 0;
int castReturnState = GAME_STATE_SCENE4; // where TRANSITION lands: scene 4 (cast) or 7 (race fail)
// Scene-8 race: top-left goal in 10s, HP starts at 80% (stage 1), -1 stage per fail
const int SCENE8_TIME_LIMIT_TICKS = 625; // 10s at ~62.5 ticks/s
int scene8TimerTicks = 0;
int scene8GoalReached = 0;
const int CAST_ANIM_SHOW_TICKS = 120; // hero-sized cast duration before auto scene switch
int savedScene3X = 576, savedScene3Y = 350; // restored when leaving scene 4 with BACK/ESC
// 15s scene-4 timer (fixedUpdate ticks every ~16ms -> 62.5 ticks/s)
const int SCENE4_TIME_LIMIT_TICKS = 938;
int scene4TimerTicks = 0;
int savedScene4X = 576, savedScene4Y = 314; // unused (scene 5 is one-way, no return to hall)
// ---- Interstitial (black screen + white text + RBD) before every forward scene change
const int INTERSTITIAL_SHOW_TICKS = 150; // ~2.4s at 16ms ticks
char interstitialText[128] = "the void is watching ";
char interstitialLine2[128] = "";
char interstitialLine3[128] = "";
int interstitialTicksLeft = 0;
int interstitialNextState = -1;

int rinoTalked = 0; // 1 after Rino line was shown (enables 2nd X: library transition)
int bossChallenged = 0; // 1 after the throne line was shown (battle starts once it fades)

int playerFacing = 1; // start facing S (toward viewer)
int playerIsMoving = 0;

// ---- Animation (ticks, no delta time — Guide §3.1) ----
int animationTickCounter = 0;

// ---- Mouse (bottom-left, Y already flipped — Reference §2) ----
int currentMouseX = 0, currentMouseY = 0;

// ---- Demo player square (kept from template, only moves in GAME_STATE_PLAY) ----
int playerSquareX = 100;
int playerSquareY = 100;

// ---- ID card pickup + gate + dialogue (scene 1) ----
const int ID_CARD_X = 515;  // book art in firstScene.jpg (was 470: ring mismatched the art)
const int ID_CARD_Y = 500;
const int ID_CARD_PICKUP_RADIUS = 55; // forgiving radius, no marker to guide pixel-perfect stands
const int GATE_X0 = 480;    // dark archway at top center: crossing band
const int GATE_X1 = 720;
const int GATE_TRIGGER_Y = 540; // pushing against the top edge inside this x-band
const int GATE_COOLDOWN_TICKS = 120;
int hasIdCard = 0;
int gateCooldownTicks = 0;
// Chained-exit guard: a held X must not fire a second zone exit on arrival.
// Set on every scene change, drained each tick, required to be 0 by all X-exits.
const int EXIT_COOLDOWN_TICKS = 60; // ~1s at 16ms ticks
int exitCooldownTicks = 0;
char dialogueText[160] = ""; // 160: throne line is 95 chars (80 crashed strcpy_s -> msvcr fatal)
int dialogueTicksLeft = 0;
const int DIALOGUE_SHOW_TICKS = 180;
// One-way destiny: going down floors is blocked on every floor (Guide §5.1 FSM)
const char* NO_RETURN_MESSAGE = "You can only proceed, towards your destiny, that is.";

int musicEnabled = 1; // global music on/off toggle (menu MUSIC button / M key)
int checkpointMusic = 0; // 1 from the end of Online (scene-3 cast) till run end: aftercheckpoint loops
int showCollisionBorders = 0; // B key toggles outline visibility; collision physics stay active

// scene-1 position saved on gate entry, restored on return (else containment yanks the hero)
int savedScene1X = 576, savedScene1Y = 376;

