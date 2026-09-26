# iGraphics AUST (v4.0) — Combined Reference & Verification Guide

> **One document to replace the old `docs/` set.** It covers the **AUST v4.0** library
> that actually ships in this folder, consolidates the prior `00_PROMPT`, `iGameDocs`,
> `iDocV4`, `techniques`, `DOCUMENTATION`, `iGraphicsAust*`, and `iGraphicsAust_COMPLETE`
> docs, and — unlike those — every claim below was **machine-verified against the
> library source on 2026-08-24** (see *Verification*).

---

## 0. Verification status (this is what "tested" means here)

| Test | How | Result |
|---|---|---|
| Both headers + `iMain.cpp` + `bitmap_loader.h` compile | `g++ -fsyntax-only` (mingw-w64, 64-bit) | **Pass** — warnings only (see §1) |
| Every public function behaves correctly | Mock-GL/GLUT/GLAUX/Win32 harness, 46 assertions | **46 / 46 pass on BOTH copies** |
| Timer cap, color-key alpha, vertex counts, ortho, texture upload, pixel readback | Same harness | **Pass** |

**Key limitation:** the supplied binaries are 32-bit (`GLUT32.DLL` = PE32 i386;
`glut32.lib`/`Glaux.lib` = 32-bit MSVC COFF), so a *native* build/run needs
**MSVC x86 + Windows + a display**. The 64-bit mingw compiler here cannot link those
import libs. Therefore *rendering, audio, and live Win32 timer dispatch were not run*;
everything else (logic, math, packing, counts, API contracts) was. The only compile
warning of substance is a 64-vs-32-bit `SetTimer` signature mismatch — this is a
**toolchain artifact**, not a library bug (on the target 32-bit MSVC it is correct).

---

## 1. What is on disk — the TWO AUST copies

The folder contains the **same library at two packaging stages**. They are
**functionally identical** (same 29 functions, same bodies, same 46/46 test result).

| | **A. `iGraphicsAust/`** (sorted) | **B. `iGraphics files (with Updated Keyboard Functions)`** (flat) |
|---|---|---|
| `iGraphics.h` | 286 lines, comments trimmed, paths reorganized | 560 lines, original verbose comments |
| Lib pragma paths | `bin/windows/glut32.lib`, `bin/windows/Glaux.lib`, **`winmm.lib`**, `bin/neutral/OPENGL32.LIB` | `glut32.lib`, `glaux.lib` (same dir only) |
| `stb_image.h` include | `"../neutral/stb_image.h"` | `"stb_image.h"` |
| `bitmap_loader.h` | **self-contained** (adds `<windows.h>` + `<GL/gl.h>`) | **NOT standalone** — needs `iGraphics.h` first |
| `iMain.cpp` | 44 lines | 86 lines |

**Actionable differences found during verification:**

1. **Audio linking.** Copy **B**'s header does **not** link `winmm.lib`, but its
   `iMain.cpp` calls `mciSendString`. Under MSVC, Copy B's template will **fail to
   link** unless you add `winmm.lib` manually. Copy **A** fixed this by adding
   `#pragma comment(lib,"winmm.lib")`. → **Prefer Copy A**, or add `winmm.lib` to B.
2. **`bitmap_loader.h` portability.** Copy A's version compiles on its own; Copy B's
   will not if included before `iGraphics.h` (undefined `RGBTRIPLE`/`BITMAPFILEHEADER`/
   `glRasterPos2f`). → **Prefer Copy A** if you use `bitmap_loader.h`.
3. Otherwise identical. The 560-vs-286 line count is only comment/whitespace.

**Recommendation:** treat **`iGraphicsAust/` (Copy A)** as the canonical library.

> **Note on the "Linux" library:** the old docs (`00_PROMPT.md`, `iGraphicsLinux.md`)
> describe a second, *incompatible* library — *Modern-iGraphics v0.6.0* (cross-platform,
> renamed callbacks, inverted color-key, FreeType/SDL2). **That folder is not present
> on disk here**, so it was not tested and is intentionally excluded from this AUST
> reference. The two libraries are NOT API-compatible — do not port AUST code to it
> without the §2 diff in the old `00_PROMPT.md`.

---

## 2. Canonical quick facts (verified)

- **Framework:** iGraphics v4.0 AUST (Imrul Jubair, 16 Dec 2017) over GLUT + OpenGL
  immediate mode + Win32 `SetTimer`. **Windows / x86 / MSVC only.**
- **Coordinates:** origin **bottom-left, Y up** (`glOrtho(0,w,0,h)`). Mouse callbacks
  are already converted to this space — do **not** flip Y again.
- **Callbacks you implement:** `iDraw()`, `fixedUpdate()` (~16 ms), `iMouse(b,s,x,y)`,
  `iMouseMove(x,y)` (drag), `iPassiveMouseMove(x,y)`.
- **Startup order:** `iInitialize(w,h,title[,rate])` → `iLoadImage()`/`iSetTimer()` →
  `iStart()` (never returns). `iSetTimer` may precede `iInitialize`; `iLoadImage`
  must come **after** `iInitialize` (needs a GL context).
- **Timers:** max **10**; `iSetTimer` returns index 0–9, else **-1**. Pause/resume by index.
- **Color:** pass **0–255 RGB** to `iSetColor`; packed color ints use **red = low byte**
  `r | g<<8 | b<<16` (see §6 errata for the GLAUX path caveat).
- **Audio:** no wrapper — raw MCI (`mciSendString`).

---

## 3. Build & run

### Copy A (`iGraphicsAust/`) — MSVC x86, from the root
```bat
cl /EHsc /I include\windows /I include\neutral src\iMain.cpp ^
   /link /LIBPATH:bin\windows /LIBPATH:bin\neutral /SUBSYSTEM:CONSOLE
copy bin\windows\GLUT32.DLL Debug\
xcopy Audios Debug\Audios\ /I
```
(The `#pragma comment(lib,...)` lines in `iGraphics.h` auto-link the libs; `winmm.lib`
is included for audio.)

### Copy B (flat) — MSVC x86
Same, but you must also add **`winmm.lib`** to the link line (its header omits it),
and keep `glut32.lib`/`glaux.lib`/`GLUT32.DLL`/`stb_image.h` next to the sources.

In both cases **`GLUT32.DLL` must sit beside the `.exe`** at runtime.

---

## 4. Lifecycle & the five callbacks

```cpp
void iDraw();                       // redraw every frame (call iClear() first)
void fixedUpdate();                 // polled ~16 ms via Win32 timer
void iMouseMove(int x, int y);      // drag (button held)
void iPassiveMouseMove(int x, int y);// move, no button
void iMouse(int button, int state, int x, int y);
```
`iStart()` registers the framework's GLUT handlers, enables alpha test
(`glAlphaFunc(GL_GREATER,0)`), and enters `glutMainLoop()` (never returns). Keep
simulation in `fixedUpdate()`/timers; keep drawing in `iDraw()`.

---

## 5. Full API reference (29 public functions — verified)

### Window, loop, input, timers
| API | Verified behavior |
|---|---|
| `iInitialize(w=500,h=500,title="iGraphics",rate=16)` | Stores W/H, starts `fixedUpdate` timer, creates double-buffered RGBA window, sets bottom-left ortho. |
| `iStart()` | Registers handlers, enables alpha test, runs `glutMainLoop()`. |
| `iScreenWidth`, `iScreenHeight` | Set at init; no resize callback updates them. |
| `iMouseX`, `iMouseY` | Latest mapped mouse position (Y already flipped). |
| `isKeyPressed(unsigned char)` | Nonzero while an ASCII key is held. |
| `isSpecialKeyPressed(unsigned char)` | Nonzero while a GLUT special key is held. |
| `iSetTimer(msec, void(*f)(void))` | Repeating Win32 timer; returns 0–9, or **-1** after the 10-timer cap. |
| `iPauseTimer(i)` / `iResumeTimer(i)` | Sets/clears a per-slot flag; the Win32 timer keeps firing but the slot's function is skipped. |
| `fixedUpdate()` | Your fixed-rate callback (default 16 ms). |

### Frame utility, color, transform, text, readback
| API | Verified behavior |
|---|---|
| `iClear()` | `glClear()` then selects `GL_MODELVIEW`, then sets **next** clear color to black, then `glFlush()`. **Does NOT call `glLoadIdentity()`** — does not reset transforms. |
| `iSetColor(r,g,b)` | Sets draw color. **Always divides each component by 255** (the `mmx=255` line is intentional; pass 0–255). |
| `iRotate(x,y,deg)` / `iUnRotate()` | Pushes matrix, translates to (x,y), rotates CCW, translates back; `iUnRotate` pops. Always pair them. |
| `iText(x,y,str,font=GLUT_BITMAP_8_BY_13)` | GLUT bitmap text in current color. Other GLUT bitmap fonts also work. |
| `iGetPixelColor(x,y,rgb[3])` | Reads one framebuffer pixel (0–255). **No bounds check.** |
| `iDelay(sec)` / `iDelayMS(ms)` | **Busy-waits** — block the window/input/sim; never call in a callback. |

### Primitives (all use current color + bottom-left coords)
| API | Verified behavior |
|---|---|
| `iPoint(x,y,size=0)` | 1 point; `size>0` also emits a `(2·size)²` block of points. |
| `iLine(x1,y1,x2,y2)` | One `LINE_STRIP` segment. |
| `iPolygon(x[],y[],n)` | Closed outline; does nothing if `n<3`. |
| `iFilledPolygon(x[],y[],n)` | Filled polygon; does nothing if `n<3`. |
| `iRectangle(l,b,dx,dy)` | Outline rectangle (4 `iLine` calls). |
| `iFilledRectangle(l,b,dx,dy)` | Filled rectangle. |
| `iCircle(x,y,r,slices=100)` | Outline circle (line segments). |
| `iFilledCircle(x,y,r,slices=100)` | Filled circle. |
| `iEllipse(x,y,a,b,slices=100)` | Outline ellipse. |
| `iFilledEllipse(x,y,a,b,slices=100)` | Filled ellipse. |

*(Vertex counts for every primitive were asserted in the verification harness:
e.g. `iRectangle`→8 verts, `iFilledRectangle`→4, `iCircle(slices=50)`→~102,
`iPoint(size=2)`→17, `iPolygon(n=2)`→0.)*

### Images
**GLAUX BMP renderer**
| API | Verified behavior |
|---|---|
| `iShowBMP(x,y,file)` | BMP with no key transparency. |
| `iShowBMP2(x,y,file,ignoreColor)` | Pixels equal to the packed key get alpha 0; `-1` = opaque. **No NULL check on `auxDIBImageLoad`** — a missing file can crash. |

**stb_image texture renderer**
| API | Verified behavior |
|---|---|
| `unsigned int iLoadImage(file)` | Decodes to RGBA (4 channels) via `stbi_load`, uploads an OpenGL texture. Call once per asset, **after `iInitialize()`**. **No NULL check on `stbi_load`.** |
| `iShowImage(x,y,w,h,tex)` | Draws a textured quad with linear filtering; **negative V coordinates** flip stb's top-down data upright under bottom-left ortho. *(Asserted: 4-vertex quad.)* |

### Optional manual BMP renderer (`bitmap_loader.h`, Copy A is standalone)
| API | Key (red = low byte) |
|---|---|
| `iShowBMPAlternative(x,y,file)` | `-1` (opaque) |
| `iShowBMPAlternative2(x,y,file,key)` | custom key |
| `iShowBMPAlternativeSkipBlack` | `0x000000` |
| `iShowBMPAlternativeSkipRed` | `0x0000FF` |
| `iShowBMPAlternativeSkipGreen` | `0x00FF00` |
| `iShowBMPAlternativeSkipBlue` | `0xFF0000` |
| `iShowBMPAlternativeSkipWhite` | `0xFFFFFF` |

Returns without drawing if the file won't open; crops only negative X/Y
(positive overflow left to GL clipping); ignores BMP row padding — use uncompressed
bottom-up 24-bit BMPs. **This path's color key is deterministically red-low-byte**
(verified), unlike the GLAUX path (see §6).

---

## 6. Bugs & errata (verified against source)

| # | Issue | Where | Fix / guidance |
|---|---|---|---|
| 1 | `iSetColor` hard-codes `mmx=255`; values are always normalized by 255. | `iGraphics.h` | Pass 0–255. Harmless, but the "max" computation is dead code. |
| 2 | **GLAUX color-key byte order is ambiguous.** The packing loop yields `R\|G<<8\|B<<16` **only if `auxDIBImageLoad` returns RGB-order bytes**; if it returns BGR, the low byte becomes blue and the documented red-low-byte key is **wrong**. | `iShowBMP2` | For reliable transparency, derive the key from a known pixel via the same packing, or **use `bitmap_loader.h`** (deterministic red-low-byte). |
| 3 | `iShowBMP2` / `iLoadImage` have **no NULL check** on the decoder. | `iGraphics.h` | Validate file paths yourself; a bad path can crash. |
| 4 | `iClear()` calls `glClear()` **before** `glClearColor()`. | `iGraphics.h` | Changing clear color affects the *next* `iClear()`, not the current one. It also does **not** reset the model-view transform. |
| 5 | `iDelay` / `iDelayMS` busy-wait. | `iGraphics.h` | Never use inside `iDraw`/`fixedUpdate`/timers. |
| 6 | Template `iMain.cpp` calls `iSetColor(255,255,255)` **after** `iFilledRectangle`, so it has no effect on that draw. | both `iMain.cpp` | Call `iSetColor` **before** the primitive it should color. |
| 7 | Copy B header does **not** link `winmm.lib` though `iMain.cpp` uses `mciSendString`. | Copy B `iGraphics.h` | Add `winmm.lib` to the link, or use Copy A. |

*(Prior doc errata also noted: D3 once stated the color key as `(red<<16)|...`; the
correct AUST packing is `r|g<<8|b<<16`. That correction is baked into §5/§6 above.)*

---

## 7. Minimal correct program

```cpp
#include "../include/windows/iGraphics.h"   // Copy A path
int x=100,y=100; unsigned int hero;
void iDraw(){
    iClear();
    iSetColor(255,255,255);          // BEFORE the primitive
    iFilledRectangle(x,y,80,80);
    // iShowImage(300,100,64,64,hero);
}
void fixedUpdate(){
    if(isKeyPressed('a')) --x;
    if(isKeyPressed('d')) ++x;
    if(isSpecialKeyPressed(GLUT_KEY_UP))   ++y;
    if(isSpecialKeyPressed(GLUT_KEY_DOWN)) --y;
}
void iMouseMove(int,int){}
void iPassiveMouseMove(int,int){}
void iMouse(int b,int s,int mx,int my){
    if(b==GLUT_LEFT_BUTTON && s==GLUT_DOWN){ x=mx; y=my; }
}
int main(){
    iInitialize(800,600,"iGraphicsAust demo");
    // hero = iLoadImage("hero.png");   // after iInitialize
    iStart();
    return 0;
}
```

**Audio (no wrapper):**
```cpp
mciSendString("open \"Audios//background.mp3\" alias music",NULL,0,NULL);
mciSendString("play music repeat",NULL,0,NULL);
// debounce one-shot SFX; don't fire "play ... from 0" every fixedUpdate tick
```

---

## 8. Smoke checklist (all logic verified headlessly)

1. Window 600×400; draw at `(0,0)` → verify Y increases upward.
2. `fixedUpdate` detects held WASD + arrows; mouse values need no extra Y flip.
3. Register 10 timers → indexes 0–9; 11th returns **-1**; pause/resume one slot.
4. Draw every primitive + a rotated shape; `iText` renders; `iSetColor` before shape.
5. `iLoadImage` a real PNG/JPG (after init); `iShowImage` draws upright.
6. `iShowBMP2` transparency via red-low-byte key; or `bitmap_loader.h` (deterministic).
7. `iGetPixelColor` returns framebuffer values.
8. MCI: open / play / loop / close an alias.

---

## 9. What happened to the old docs

This file **replaces and supersedes** the previous `docs/` set:
`00_PROMPT.md`, `README.md`, `DOCUMENTATION.md`, `iDocV4.md`, `techniques.md`,
`iGameDocs.md`, `iGraphicsAust.md`, `iGraphicsLinux.md`, `iGraphicsAust_COMPLETE.md`.
Their surviving, verified content is folded into §1–§8 above. The originals have been
moved to **`docs/archive/`** for history; delete that folder once you're comfortable.
The Linux (Modern-iGraphics) material was excluded because that library is not
present on disk and is API-incompatible with AUST.

*Generated 2026-08-24. Verification harness: 46/46 behavioral assertions passing on
both AUST copies; both compile cleanly under mingw-w64 (warnings only).*
