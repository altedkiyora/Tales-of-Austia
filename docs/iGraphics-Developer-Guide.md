# iGraphics Developer Guide

> **Companion to `iGraphics-AUST-Reference.md`** (the verified API reference — read that first).
> This guide covers what the reference doesn't: build recipes, gotchas learned the hard way,
> asset pipelines, and a case study of a complete game that was built and shipped on this\n> framework (code since removed from this workspace — the patterns below are the record).
>
> Audience: anyone starting a new project on, or maintaining, iGraphics v4.0 AUST.

---

## 1. TL;DR for a new project

1. Copy a working skeleton (layout in §5): `include/`, `bin/`, `GLUT32.DLL`, an `assets/` folder.
2. Build with **32-bit MSVC only** (the shipped `glut32.lib`/`Glaux.lib`/`GLUT32.DLL` are PE32 i386).
3. Link `user32.lib gdi32.lib advapi32.lib` — the header pragmas do *not* cover GLAUX's Win32 dependencies.
4. `GLUT32.DLL` and `assets/` must sit beside the `.exe` at runtime.
5. Implement the 5 callbacks (`iDraw`, `fixedUpdate`, `iMouse`, `iMouseMove`, `iPassiveMouseMove`),
   call `iInitialize()` → `iLoadImage()` → `iStart()` in that order.

---

## 2. Build recipes

### 2.1 Command line (proven)

```bat
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86
cl /EHsc /I include\windows /I include\neutral src\main.cpp /Fe:run\Game.exe ^
   /link /LIBPATH:bin\windows /LIBPATH:bin\neutral /SUBSYSTEM:CONSOLE ^
   user32.lib gdi32.lib advapi32.lib
copy bin\windows\GLUT32.DLL run\
xcopy assets run\assets\ /E /I /Y
```

Tested on MSVC 18.00 (VS2013). Newer MSVC versions may also work if they can target x86,
but the 32-bit import libs are the binding constraint — not the C++ level.

### 2.2 Visual Studio project gotchas

- **Platform: Win32 (x86) only.** Any x64 config will fail to link the 32-bit libs.
- **`ImageHasSafeExceptionHandlers = false`** (`/SAFESEH:NO`). The ancient `Glaux.lib` is not
  SAFESEH-clean; VS project builds fail with `LNK2026: module unsafe for SAFESEH image`
  unless this is disabled. (Command-line `cl` builds don't hit this because they don't
  opt into SAFESEH — which is why a working `.bat` can coexist with a failing `.vcxproj`.)
- **Character Set: MultiByte.** The framework and GLUT are ANSI throughout.
- Set the **debugger working directory** to the folder containing `assets/`
  (a `.vcxproj.user` with `LocalDebuggerWorkingDirectory` pointing at your runtime folder), or textures silently fail to load.
- `iGraphics.h` auto-links via `#pragma comment(lib, ...)`: `glut32.lib`, `Glaux.lib`,
  `OPENGL32.LIB`, `winmm.lib` — paths relative to the *project root*, so build from there.

### 2.3 Linker error triage

| Error | Cause | Fix |
|---|---|---|
| `LNK2019 __imp__SetTimer@16`, `__imp__wsprintfA`, `__imp__MessageBoxA`, ... (57 unresolved) | GLAUX/GLUT need Win32 SDK libs not in the pragmas | append `user32.lib gdi32.lib advapi32.lib` |
| `LNK2026 module unsafe for SAFESEH` | old Glaux object modules | `/SAFESEH:NO` / `ImageHasSafeExceptionHandlers=false` |
| `LNK1112 machine type x64 conflicts` | wrong platform | use Win32 |
| app starts then dies instantly | `GLUT32.DLL` not beside exe | copy it (post-build step) |
| window opens, textures black/garbage | assets not found (wrong CWD) | fix working directory; see §4.1 |

---

## 3. Framework behavior you must design around

Verified details are in the reference (§5–§6). The ones that shape **architecture**:

1. **No delta time.** `fixedUpdate()` runs at a fixed rate (~16 ms). Write all simulation
   in *ticks*, not seconds. The §5 case-study game used speeds like `2.7` px/tick for exactly this reason.
2. **Max 10 timers**, and `iSetTimer` returns `-1` past the cap. You rarely need any —
   the case-study game used zero: everything derived from the `fixedUpdate` tick counter.
3. **`iDraw()` is immediate mode.** There is no scene graph; you redraw the world every frame.
   Structure code as `drawScene*()` functions per game state (menu/play/pause/dead/transition).
4. **No render-state management.** `iShowImage` enables/disables texture binding per call and
   uses `GL_REPLACE` (vertex color is *ignored* on images — no tinting, no fading via `iSetColor`).
   You may `glEnable(GL_BLEND)` inside `iDraw` for soft particles/overlays; disable it before
   drawing opaque HUD. Alpha test (`GL_GREATER,0`) is already on from `iStart()`.
5. **Y-up, bottom-left origin.** Mouse coords are pre-flipped; don't flip again.
6. **Busy-wait `iDelay`/`iDelayMS`** freeze everything — never in callbacks.
7. **`iLoadImage` has no error check** — a wrong path yields texture 0 and garbage or a crash.
   Validate asset paths at least once in dev (see §4.1).
8. **`iClear()` clears with the *previous* frame's `iSetColor`.** If you want a custom
   background, call `iSetColor(bg)` at the *end* of `iDraw`.

### 3.1 Audio (MCI) patterns

```cpp
mc("open \"assets//music//background.mp3\" alias bgmusic");  // note // separators
mc("play bgmusic repeat");
mc("open \"assets//sfx//hit.wav\" alias hit");
void sfx(const char* a){ char c[96]; sprintf(c,"play %s from 0",a); mc(c); }  // retrigger
```

- Open every alias once at startup; retrigger with `play <alias> from 0`.
- MCI plays **wav/mp3/wma**. It does **not** play OGG — Kenney SFX packs are OGG, so either
  convert them or synthesize your own WAVs (a ~100-line PowerShell script (System.IO.BinaryWriter, sine/square/noise oscillators + exponential envelopes — see §4.4) generates a
  full retro SFX set with no dependencies).
- Process exit cleans up MCI; there is no clean shutdown path from `iStart()` anyway.

---

## 4. Asset pipelines that work here

### 4.1 Texture loading rules

- `iLoadImage` (stb_image) loads **PNG/JPG/TGA/BMP/...** — always use PNG with alpha.
- `iShowImage` draws the **whole texture only** — there is no sub-rectangle/UV cropping.
  Consequence: **spritesheets must be pre-sliced** into per-frame images (see §4.2).
- Draw sprites centered with `iShowImage(x - w/2, y - h/2, w, h, tex)`; the `w,h` args
  scale freely, which is how you mix 16px tiles with 64px character frames.
- Dev-time sanity check: enumerate every `assets/...` path referenced in code and
  `Test-Path` each against the runtime folder before shipping.

### 4.2 LPC paper-doll characters (bake + slice)

LPC sheets are **layered** (BODY/TORSO/LEGS/FEET/HEAD/BELT/HANDS/WEAPON) at 64×64 per frame,
4 direction rows (up, left, down, right), N frames per animation. Since `iShowImage` can't
crop, bake layers into single sheets then slice to individual frames:

- A PowerShell script using `System.Drawing` — composites chosen layers per animation
  (alpha over, correct bottom-to-top order: BODY → FEET → LEGS → TORSO → BELT → HEAD → HANDS → WEAPON),
  then emits `walk_<dir>_<frame>.png` etc. ready for `iLoadImage`.
- Gotchas learned:
  - Folder naming is inconsistent: body layer is `BODY_male`, `BODY_human`, `BODY_skeleton`
    or `BODY_animation` depending on the animation folder.
  - Skeleton has **no** thrust/bow sheets — only the human body does. Recolor the human
    (green tint = zombie) instead of hunting for sheets that don't exist.
  - Animation lengths: walk 9 (frame 0 = idle), slash 6, spellcast 7, thrust 8, hurt 6, bow 13.
- In-game animation = `tex[anim][dir][(ticks_since_start / TICKS_PER_FRAME) % len]`.

### 4.3 Mapping unnamed Kenney tiles

Packs like Tiny Dungeon ship `tile_0000.png … tile_0131.png` with no names. Build a labeled
contact sheet (grid of tiles + index captions) and read it once — that's how
the case-study game's tile table (floor 0000/0012/0024, wall 0013/0014, torch wall 0029,\nchests 0090/0091, gold 0066, potion 0115, critters 0120–0124) was mapped.
The `.tmx` in the pack also reveals commonly-used GIDs (mask Tiled flip flags with `0x1FFFFFFF`).

### 4.4 Synthesized SFX

`tools/synth_sfx.ps1` writes 22.05 kHz 16-bit PCM WAVs (swing/hit/hurt/pickup/heal/chest/
die/stairs/shoot/click) from sine/square/noise + exponential envelopes. Zero dependencies,
CC0-equivalent output, and they sound intentionally retro next to pixel art.

---

## 5. Case study: structuring a complete game\n\nA ~1100-line dungeon crawler (animated LPC characters, procedural rooms, combat, loot,\nmenus, persistence) was built and shipped on this framework, then removed from this\nworkspace. The structure below is the transferable blueprint — reuse it directly.

### 5.1 Architecture map

| Concern | Where | Pattern |
|---|---|---|
| Game states | `state` (`ST_MENU/HELP/PLAY/PAUSE/DEAD/TRANS`) | flat enum FSM; each state has one `drawScene*()` |
| Simulation | `fixedUpdate()` | fixed ticks; per-entity update fns (`updatePlayer`, `updateEnemy`...) |
| Rendering | `iDraw()` → `drawScenePlay()` | painter's order: floors → walls → props → pickups → enemies → player → fx → particles → lights → HUD |
| World | `grid[GH][GW]`, tile 48px | 0=floor, 1=wall; `solidCell()` is the only collision oracle |
| Dungeon gen | `genLevel()` | random non-overlapping rooms + L-corridors + extra loop link; deco pass (torch/gargoyle/skull walls, floor variants) |
| Camera | `camX/camY` | lerp 0.12 to player, clamped to world; draw = world pos − cam |
| Entities | fixed arrays (`en/pk/bt/pt/ob/ft`) | struct + `bool on`, capped pools — no STL, no allocation after boot |
| Combat | `doSlashHits()`, `damageEnemy/Player` | timed hit windows inside attack animations (`slashT` ticks 12–20 of 30); per-slash hit dedup via `slashId` |
| Textures | `tx*` globals, `loadAll()` | loaded once after `iInitialize`; arrays indexed `[anim][dir][frame]` |
| Audio | `openAudio()` + `sfx("alias")` | MCI aliases opened at boot; music state switches on death/restart |
| Persistence | `hiscore.txt` | single int, written on death if beaten |

### 5.2 Extension recipes

- **New enemy type**: add an `ET_*` enum, a branch in `updateEnemy()` (steer + attack timing),
  textures, and a spawn roll in `genLevel()`. Copy the skeleton's windup→hit-window pattern.
- **New pickup**: add `PU_*`, draw branch in `drawScenePlay()`, auto-collect (gold/potion) or
  E-interact (chest/exit) branch in `updatePickups()`/`interact()`.
- **New weapon**: change `pl.dmg`, reach constants in `doSlashHits()`, and swap the baked
  weapon layer in the LPC bake script.
- **Boss**: reuse `Enemy` with a phase machine keyed on `hp` thresholds (see reference §6.3).
- **Difficulty scaling**: `genLevel()` already scales count/HP/speed off `level` — tune there.

### 5.3 Tuning constants cheat sheet

| Constant | Value | Meaning |
|---|---|---|
| `TILE`, `GW`, `GH` | 48, 40, 30 | world = 1920×1440 px |
| player speed | 2.7 px/tick | ~165 px/s |
| slash | 30 ticks, hits 12–20, cd 34 | ~0.48 s swing |
| skeleton | hp 3+lvl/2, speed 1.55, chase 280 | melee |
| archer | hp 2+lvl/3, bolt 4.6, cd 130 | ranged, level ≥ 2 |
| contact damage | radius 26, i-frames 50 | shared by all touches |

---

## 6. Maintenance notes for this workspace

- `docs/` holds **only** `iGraphics-AUST-Reference.md` (verified API truth) and this guide.\n  Historical docs and the case-study game were intentionally removed — this workspace is\n  documentation-only.
- No code or binaries live here anymore. To start a project, reconstruct the skeleton from\n  §1–§2 and re-generate assets with the pipelines in §4.
- All art/SFX are CC0 (Kenney) or freshly synthesized/baked — safe to redistribute.
- If you re-download Kenney packs or LPC sheets, regenerate frames with a bake/slice script\n  (§4.2) rather than hand-editing assets.

*Generated 2026-08-26. Workspace is documentation-only.*
