// ------------------------------------------------------------------------
// Brawl.hpp  -  LASTBRAWL scene (throne-hall 2D brawler) state + art.
// Kept as its own header so the revertable LASTBRAWL/BRAWLBAR/ENDING blocks
// stay physically together and are easy to diff or roll back wholesale.
// ------------------------------------------------------------------------
#pragma once

// ==== LASTBRAWL revertable block 1/4: state (throne-hall 2D brawler; MID fight untouched) ====
// Side-view duel: hero left (faces right), boss right (faces left). Feet-X tracked
// directly. HP uses dedicated 0..7 stages on dragon-bar art (never MID's globals).
unsigned int heroBrawlAtkTexture = 0; // mainCharacter_sprites//aimed_shot_E (strike, faces right)
unsigned int heroBrawlAtkWTexture = 0; // aimed_shot_MR (mirrored E: true left-facing strike)
unsigned int bossBrawlStanceWTexture = 0; // darkBoss special_cape_stance_W (true left-facing idle)
unsigned int bossBrawlStanceETexture = 0; // special_cape_stance_MR (mirrored W: true right-facing idle)
unsigned int bossBrawlAtkTexture = 0; // darkBoss attack_shadowClaw_W (leftward swipe)
unsigned int bossBrawlClawETexture = 0; // attack_shadowClaw_MR (mirrored W: true rightward swipe)
// ==== BRAWLBAR revertable block 1/2: distinct enemy bar (remove to revert) ====
// Red dragon-head staged bar (1024x254 files) vs the MC's green 8-part bar.
unsigned int bossBarTexture[6] = {0, 0, 0, 0, 0, 0}; // 100_full, 70_high, 40_mid, 35_mid, 15_low, 00_empty
const int BOSS_BAR_STAGE[8] = {0, 0, 1, 1, 2, 3, 4, 5}; // hp stage 0..7 -> file (nearest remaining HP)
// ==== end BRAWLBAR block 1/2 ====
int brawlHeroFaceR = 1, brawlBossFaceL = 1; // auto-face: hero faces boss side, boss faces hero side
const int BRAWL_FLOOR_Y = 150;
const int BRAWL_MIN_X = 150, BRAWL_MAX_X = 1000; // feet-X bounds (pass-through allowed)
const int BRAWL_ATK_RANGE = 110; // feet-distance for a hit to land
const int BRAWL_HERO_COOLDOWN = 30; // ticks between hero strikes (~0.5s)
const int BRAWL_BOSS_COOLDOWN = 150; // ticks between boss swipes (~2.4s)
const int BRAWL_ATK_ANIM = 24; // strike animation length (ticks); impact at half
const int BRAWL_SWORD_FRAMES = 13, BRAWL_SWORD_TICKS = 4; // holy-sword overlay pacing
const int BRAWL_NOVA_TICKS = 6; // holy-nova burst pacing (same as scene-4 shower)
int brawlHeroX = 300, brawlBossX = 800; // feet X
int brawlHeroHp = 0, brawlBossHp = 0; // 0 full .. 7 empty
int brawlHeroAtkTick = 0, brawlHeroCooldown = 0; // swing timer / strike lock
int brawlBossAtkTick = 0, brawlBossCooldown = 0;
int brawlSwordTick = 0, brawlNovaTick = 0; // effect overlays (0 = inactive)
int brawlOver = 0; // 0 fighting, 2 lost, 3 boss dying (win auto-goes to ENDING)
// ==== ENDING revertable block 1/3: boss death + ending state (remove to revert) ====
unsigned int bossBrawlDieTexture = 0; // darkBoss dodge_W: shadow-dissolve death
const int BRAWL_DIE_TICKS = 140; // ~2.2s dissolve before the black screen
int brawlDieTick = 0; // progress of the dissolve (0 = inactive)
// ==== end ENDING block 1/3 ====
// ==== end LASTBRAWL block 1/4 ====

