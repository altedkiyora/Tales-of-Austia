// ------------------------------------------------------------------------
// AssetRegistry.hpp  -  one place that owns the raw texture handles.
// Every PNG the game can draw is loaded ONCE in main() and held here, so
// Scenes/Ui just pick a handle off the shelf. Panel geometry + animation
// frame-count consts hang out here too, next to the art they describe.
// ------------------------------------------------------------------------
#pragma once

// ---- Textures (loaded once after iInitialize — Reference §2) ----
unsigned int menuBackgroundTexture = 0;
unsigned int aboutBackgroundTexture = 0;
unsigned int playBackgroundTexture = 0;
unsigned int scene2BackgroundTexture = 0; // Main//floor.jpg placeholder for scene 2
unsigned int scene3BackgroundTexture = 0; // next floor (same floor.jpg map as scene 2)
unsigned int scene4BackgroundTexture = 0; // Main//scene4.jpg hall
unsigned int scene6BackgroundTexture = 0; // Main//doorOpen.jpg (same layout, open door)
unsigned int scene7BackgroundTexture = 0; // Main//maze.jpg
unsigned int bossBackgroundTexture = 0; // Main//bossForC2.jpg (finale battle map)
unsigned int classroom1Texture = 0; // afterScene10//classroom//classroom1.jpg (first half)
unsigned int classroom2Texture = 0; // afterScene10//classroom//classroom2.jpg (second half)
unsigned int montTexture = 0; // afterScene10//classroom//3_transparent.png (Mont)
unsigned int noplaTexture = 0; // afterScene10//classroom//2_transparent.png (Nopla)
unsigned int classCrTexture = 0; // afterScene10//classroom//1_transparent.png (CR)
unsigned int rinoTexture = 0; // afterScene10//classroom//Rino.png (Rino near board)
unsigned int library1Texture = 0; // afterScene10//classroom//library1.jpg (first half)
unsigned int library2Texture = 0; // afterScene10//classroom//library2.jpg (second half)
unsigned int library2BookTexture = 0; // afterScene10//classroom//image.png (demonic book pickup)
// Book in library2 top-left, resized down via draw size (source is large)
const int LIBRARY2_BOOK_X = 145, LIBRARY2_BOOK_Y = 425;
const int LIBRARY2_BOOK_W = 48, LIBRARY2_BOOK_H = 64;

unsigned int bigGateTexture = 0; // afterScene10//bigGate.png (gate courtyard)
unsigned int bossRoomTexture = 0; // afterScene10//bossRoom1.png (throne hall)
unsigned int bossThroneTexture = 0; // afterScene10//boss//darkBoss_sprites_transparent//idle_throne//idle_throne_S.png
// Boss idle on the throne (bottom-left anchor, drawn with a 2px idle bob)
const int BOSS_THRONE_X = 552, BOSS_THRONE_Y = 244;
const int BOSS_THRONE_W = 110, BOSS_THRONE_H = 100;

unsigned int heroTexture = 0; // Cr//Cr (1).png — fallback (clean alpha, 221x317)
unsigned int xKeycapTexture = 0; // fonts//buttons//letters//04_07_X.png (13x12, talk prompts)
unsigned int seniorIdleTexture[6] = {0, 0, 0, 0, 0, 0}; // senior idle//senior1..6.png (scene-2 NPC)
unsigned int crIdleTexture[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // Cr//Cr (1)..(8).png (scene-3 NPC)
unsigned int castAnimationTexture = 0; // mainCharacter_sprites//castSpecialAnimation.png (hero-sized cast)

// ---- Dragon HP (minigame): 8 bar stages, +1 stage per 5 nova touches
unsigned int dragonHpTexture[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // DragonHpBar_parts//dragon_part_1..8.png

// Main character: idle + 2 walk frames x 4 facings (0=N 1=S 2=E 3=W)
unsigned int heroIdle[4] = {0, 0, 0, 0};
unsigned int heroWalkA[4] = {0, 0, 0, 0};
unsigned int heroWalkB[4] = {0, 0, 0, 0};

// ---- UI kit (ui/ folder) ----
unsigned int menuPanelTexture = 0;
unsigned int buttonBarNormalTexture = 0;
unsigned int buttonBarHoverTexture = 0;
unsigned int buttonCornerTextures[4] = {0, 0, 0, 0}; // border (1)..(4).png 12x13 corners

// Panel geometry — small 370x360 centered horizontally, lowered vertically
// left=(1200-370)/2=415, bottom=140 (was 220 centered, lowered 80 to reveal logo)
const int MENU_PANEL_WIDTH = 370;
const int MENU_PANEL_HEIGHT = 360;
const int MENU_PANEL_LEFT = 415;
const int MENU_PANEL_BOTTOM = 140;
const int MENU_PANEL_CENTER_X = 600; // = SCREEN_WIDTH/2

