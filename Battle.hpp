// ------------------------------------------------------------------------
// Battle.hpp  -  Pokemon-style finale battle vs CR (MID fight block).
// Move textures, phase/tick consts and the whole battle hand (hp phases,
// cooldowns, anim state). Left untouched - it was already working.
// ------------------------------------------------------------------------
#pragma once

unsigned int crCngTexture[4] = {0, 0, 0, 0}; // Cr cng//crCng1..4.png (attack stance)
unsigned int crAtkTexture[5] = {0, 0, 0, 0, 0}; // cr atk//crAtk1..5.png (enemy attack)
unsigned int swordTexture[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // SwordOfJustice splash
unsigned int moveSlashTex[5] = {0, 0, 0, 0, 0}; // HolySlash_A battle move
unsigned int moveSmiteTex[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // Smite battle move
unsigned int moveHealTex[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // Heal battle move
unsigned int moveShieldTex[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // HolyShield battle move
const int CNG_FRAMES = 4, ATK_FRAMES = 5, SWORD_FRAMES = 13;
const int MOVE_FRAMES[4] = {5, 11, 12, 11}; // slash, smite, heal, shield
const int BATTLE_ANIM_TICKS = 6; // ticks per battle-move frame
const int SPLASH_SHOW_TICKS = 120; // black-screen sword splash hold
int splashTicksLeft = 0;
int crAttackTalked = 0; // 1 after the attack-CR threat line (enables 2nd X: transform)
int crTransformed = 0; // 1 once old CR becomes new_cr (talk phase over)
int crTransformTicks = 0;
const int CR_TRANSFORM_TICKS = 60; // visible transformation beat before the splash
// ---- Pokemon-style finale battle state
const int PHASE_INPUT = 0, PHASE_PLAYER_ANIM = 1, PHASE_ENEMY_ANIM = 2, PHASE_OVER = 3;
const int MOVE_SLASH = 0, MOVE_SMITE = 1, MOVE_HEAL = 2, MOVE_SHIELD = 3;
const int BATTLE_FOE_X = 700, BATTLE_FOE_Y = 380, BATTLE_FOE_SIZE = 160;
const int BATTLE_OPT_X = 880, BATTLE_OPT_W = 260, BATTLE_OPT_H = 48, BATTLE_OPT_GAP = 12, BATTLE_OPT_Y0 = 140;
const char* BATTLE_OPT_LABEL[4] = {"1 INK STORM", "2 SMITE", "3 HEAL", "4 SHIELD"};
int battlePhase = PHASE_INPUT, battleSelectedMove = 0, battleAnimTick = 0, battleResult = 0;
int smiteCooldown = 0, shieldActive = 0, enemyHpStage = 0;
int crDefeated = 0; // 1 after the finale win: victory roam + changed door message
// Victory roam: right-edge border of the boss map -> scene 10
const int VICTORY_EXIT_X = 1100;

