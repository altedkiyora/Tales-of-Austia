# iGx — Modern Cross-Platform iGraphics AUST Port (Design + Implementation Notes)

> **Superseded in part by the live implementation** at `Downloads/iGx/` (this workspace's
> sibling). This file records the design rationale and the faithful-quirk contract.
> Read it alongside `iGx/README.md`.
>
> **NOTE ON DRIFT:** the body below was written when **GLFW** was the candidate backend.
> It was later changed to **SDL3** (window + input + GL context + SDL3_mixer audio)
> and the project named **iGx**. Where this doc says GLFW, read **SDL3**;
> where it says miniaudio, read **SDL3_mixer**; where it says `igraphics-modern`, read
> **iGx**. The architecture (front/back split), API contract, and quirks are unchanged.
>
> **Goal:** A Linux + Windows "modern" library that keeps the **same public API and
> behavior** as iGraphics AUST v4.0, but replaces the legacy GLUT/OpenGL-immediate
> + Win32-`SetTimer` + GLAUX/MCI internals with a **SDL3 + OpenGL 3.3 core** renderer
> and cross-platform timers/audio/keyboard.
>
> Decisions locked with the owner:
> - **Backend:** SDL3 + OpenGL 3.3 core (shader renderer). Drop `glBegin/glEnd`.
> - **Compat:** Public API **byte-for-byte identical**, including documented quirks/errata.
> - **Toolchain:** CMake + Ninja; **GCC primary, Clang secondary**; system SDL3 packages;
>   `stb_image.h` bundled.
> - **Status as of 2026-08-28:** implemented and smoke-tested on both GCC 16 and
>   Clang 22 (60/60 fixed ticks, color/timer-cap quirks asserted, template demo runs).

---

## 1. Design constraints (carry-over from AUST)

Everything below is the **contract** that must survive the rewrite. Consumers (the
§5 case-study game and any `.cpp` written against AUST) must compile unchanged:

1. **Five callbacks:** `iDraw()`, `fixedUpdate()`, `iMouse(b,s,x,y)`,
   `iMouseMove(x,y)`, `iPassiveMouseMove(x,y)`.
2. **Startup order:** `iInitialize(w,h,title[,rate])` -> `iLoadImage()`/`iSetTimer()` ->
   `iStart()` (never returns).
3. **Coordinates:** bottom-left origin, Y up. Mouse callbacks already flipped — do not
   flip again.
4. **No delta time.** `fixedUpdate()` runs at the configured rate (~16 ms). Simulation
   in *ticks*, never seconds.
5. **Timer cap = 10.** `iSetTimer` returns index 0–9, else `-1`. `iPauseTimer`/`iResumeTimer`
   by index.
6. **`iSetColor` normalizes 0–255** (divide each by 255).
7. **`iShowImage` uses GL_REPLACE** — vertex color ignored; no tinting via `iSetColor`.
8. **Alpha:** pixels with alpha 0 are culled (`GL_ALPHA_TEST, GL_GREATER 0`).
9. **Color key:** packed `r | g<<8 | b<<16` for the GLAUX path; the `bitmap_loader.h`
   path is deterministically **red = low byte**.
10. **Quirks preserved**: `iClear()` sets the *next* clear color and does **not** reset
    transforms; `iDelay/iDelayMS` busy-wait; missing-file decoders have no NULL check.
11. **Audio** is MCI-string style (`mc("play alias from 0")`) in AUST; port keeps an
    alias-based string API but backs it cross-platform.

---

## 2. Architecture

```
igraphics-modern/
  CMakeLists.txt
  include/igraphics/            # PUBLIC headers — NO OS deps, NO GL deps, NO #pragma lib
    igraphics.h                 # the 29 functions + 5 callbacks, pure declarations
    bitmap_loader.h             # standalone BMP loader (red-low-byte keys)
  src/
    igraphics.cpp               # API dispatch, state, quirks (platform-neutral)
    render/                     # OpenGL 3.3 core renderer
      gl_context.hpp/.cpp       # window creation via GLFW, ortho matrix, swap
      shader.hpp/.cpp           # minimal shader compile/link + uniforms
      renderer.hpp/.cpp         # batched quad/polygon/triangle path
      texture.cpp               # stb_image decode + texture upload; BMP color-key
    platform/
      backend.hpp               # abstract: window, input, timers, pump, audio
      backend_glfw.cpp          # GLFW backend (Linux + Windows; X11/Wayland, Win32)
      timer.cpp                 # fixedUpdate clock + 10-slot timer pool
      input.cpp                 # keyPressed[512]/specialKeyPressed[512] + mouse
    audio/
      audio.hpp                 # MCI-like alias API
      audio_impl.cpp            # cross-platform player (GLFW alias + miniaudio)
  third_party/
    glfw/                       # vendored or fetched
    stb_image.h
    miniaudio.h
  tests/
    smoke.cpp                   # port of reference §8 checklist
```

### Frontend/backend split (the key to "same API, two OSes")

- **Public headers** (`include/igraphics/`): the 29 functions and 5 callbacks as
  declarations only. Zero `#include <windows.h>`, zero `#pragma comment(lib)`, zero
  `__stdcall`, zero GL types in signatures (`unsigned int` for textures is fine).
- **`src/igraphics.cpp`**: owns the documented state and **quirk semantics** — the
  "what it means" logic lives here once, never duplicated per platform.
- **`src/platform/backend.hpp`**: a small interface the state layer calls into. Only
  `backend_glfw.cpp` is OS-aware, so Linux and Windows share every other file.

### backend.hpp (abstract interface)

```cpp
struct Backend {
    virtual ~Backend() = default;
    virtual bool init(int w, int h, const char* title) = 0;   // window + GL context
    virtual void swap() = 0;                                   // double-buffer
    virtual void setClearColor(float r, float g, float b, float a) = 0;
    virtual void* nativeWindow() = 0;                          // if ever needed
    // pumped implicitly by GLFW event loop; backend owns GLFW callbacks
};

struct InputState {   // owned by backend, read by igraphics.cpp
    unsigned int key[512], special[512];
    int mx, my;                 // raw OS y (top-left)
    int button, state;          // current mouse button/state
};

struct BackendEvents {          // implemented in igraphics.cpp
    virtual void onFixedTick() = 0;   // fixedUpdate
    virtual void onTimer(int idx) = 0;// 0..9, skipped if paused
    virtual void onKeyDown(int ascii, int special) = 0;
    virtual void onKeyUp(int ascii, int special) = 0;
    virtual void onMouseMove(int x, int y) = 0;      // maps + flips Y
    virtual void onPassiveMove(int x, int y) = 0;
    virtual void onMouse(int b, int s, int x, int y) = 0;
    virtual void onDraw() = 0;                        // iDraw + swap
};
```

---

## 3. Mapping table (AUST internals -> modern implementation)

| AUST (Windows/GLUT) | Modern (GLFW + GL 3.3 core) | Notes |
|---|---|---|
| `glutInitDisplayMode` / `glutCreateWindow` | `glfwInit` + `glfwCreateWindow` (double-buffered RGBA) | GLFW handles OS coupling itself |
| `glOrtho(0,w,0,h)` | ortho matrix built in GLSL (`projection` uniform) | keeps bottom-left; Y-up |
| `glutDisplayFunc`/`glutSwapBuffers`/`glutPostRedisplay`/idle | render on demand + swap in the main loop | `glfwPollEvents` + render-if-dirty |
| Win32 `SetTimer` for `fixedUpdate` (~16 ms) | a monotonic **clock**; fire `onFixedTick` every `rate` ms | no delta-time passes through |
| `iSetTimer(i)` slice of 10 `SetTimer`s | **timer pool**: 10 slots of `{cb, period, pause}`, driven by one clock | preserve: index 0–9, `-1` past cap, pause = skip flag |
| `keyPressed[]`/`specialKeyPressed[]` | `Backend`-filled GLFW key maps | same arrays, same `isKeyPressed`/`isSpecialKeyPressed` |
| mouse `GLUT_LEFT_BUTTON`/`GLUT_DOWN` + `height-y` flip | GLFW mouse button callbacks -> GLUT constants, flip Y | `iMouseX/Y` set to flipped values |
| GLAUX `iShowBMP`/`iShowBMP2` | standalone BMP decode -> texture + color-key | keep `r\|g<<8\|b<<16`; `-1` = opaque; **no NULL check kept** |
| stb `iLoadImage` (RGBA, texture) | identical stb_image (already portable); upload GL3 texture | `stbi_load(...,4)` unchanged |
| `iShowImage` (GL_REPLACE, neg-V flip) | textured quad via shader; **no color contribution**; neg-V flip kept | replicate "no tint by vertex color" |
| `iSetColor`/primitives (`glBegin`...) | batched vertex buffer + fragment shader | rect/polygon/circle tessellation unchanged (same vertex counts/geometry) |
| `iRotate/iUnRotate` (push/translate/rotate/translate-back) | a 2D transform pushed onto the batch matrix stack | pair semantics preserved |
| `iText` (`glutBitmapCharacter`) | bake ASCII 8x13 glyphs into a texture atlas; draw as quads | same default font impression |
| `glReadPixels` (`iGetPixelColor`) | `glReadPixels` (valid in GL3) | no bounds check kept |
| `glAlphaFunc(GL_GREATER,0)` + `GL_ALPHA_TEST` | fragment shader `if (a == 0.0) discard;` | keep "alpha 0 culled" |
| `iClear()` quirks (next-color, no reset transform) | renderer: remember "clear color" then apply at next clear; don't pop matrix | **preserve exactly** |
| MCI audio (`mciSendString`) | alias-based string API backed by a cross-platform player (miniaudio) | replaces GLAUX-less audio; keeps `mc("open ... alias")` style |
| MSVC-only build / `#pragma comment(lib)` | **CMake** + vendored GLFW/stb/miniaudio; no pragma libs | Linux + Windows identical flow |
| `glut32.lib`/`Glaux.lib`/PE32 | none — all replaced | x64 + ARM supported |

---

## 4. Preserved quirks (explicit decisions)

To keep "identical API + quirks", the port must hard-code these documented behaviors
and unit-test them, not "fix" them:

1. `iSetColor` divides each channel by 255 (hard-coded `mmx=255`).
2. `iClear()` clears, then sets the **next** clear color; does **not** reset transforms.
3. `iShowImage` uses `GL_REPLACE` — draw color does not tint images; no fade-via-color.
4. Alpha-0 culling (from `iStart()`).
5. `iSetTimer` cap of 10; `-1` past cap; `iPauseTimer` resumes skip the slot.
6. `iShowBMP2`/`iLoadImage` have **no NULL check** on decode — bad path = garbage/crash.
7. `iDelay`/`iDelayMS` busy-wait (documented: never in callbacks).
8. `iGetPixelColor` no bounds check.
9. GLAUX color-key is red-low-byte (`r|g<<8|b<<16`); `bitmap_loader.h` path deterministic.

These live in `src/igraphics.cpp` + the renderer, with dedicated smoke assertions
(ported from reference §8).

---

## 5. Public header sketch (what ships — unchanged API)

```cpp
// include/igraphics/igraphics.h  — no OS/GL includes, no #pragma lib
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// 5 callbacks the user implements
void iDraw();
void fixedUpdate();
void iMouseMove(int x, int y);
void iPassiveMouseMove(int x, int y);
void iMouse(int button, int state, int x, int y);

// consts kept so GLUT button codes still work, mapped by the backend
enum IG_MOUSE_BUTTONS { IG_LEFT_BUTTON=0, IG_MIDDLE_BUTTON=1, IG_RIGHT_BUTTON=2 };
enum IG_MOUSE_STATES  { IG_UP=0, IG_DOWN=1 };

void  iInitialize(int width=500, int height=500, const char* title="iGraphics", int rate=16);
void  iStart();
int   iSetTimer(int msec, void (*f)(void));
void  iPauseTimer(int index);
void  iResumeTimer(int index);
int   isKeyPressed(unsigned char key);
int   isSpecialKeyPressed(unsigned char key);

void  iClear();
void  iSetColor(double r, double g, double b);
void  iRotate(double x, double y, double deg);
void  iUnRotate();
void  iText(double x, double y, const char* str, void* font=0);
void  iGetPixelColor(int x, int y, int rgb[3]);
void  iDelay(int sec);
void  iDelayMS(int ms);

void iPoint(double x, double y, int size=0);
void iLine(double x1,double y1,double x2,double y2);
void iPolygon(const double x[], const double y[], int n);
void iFilledPolygon(const double x[], const double y[], int n);
void iRectangle(double l, double b, double dx, double dy);
void iFilledRectangle(double l, double b, double dx, double dy);
void iCircle(double x, double y, double r, int slices=100);
void iFilledCircle(double x, double y, double r, int slices=100);
void iEllipse(double x, double y, double a, double b, int slices=100);
void iFilledEllipse(double x, double y, double a, double b, int slices=100);

void iShowBMP(int x, int y, const char* file);
void iShowBMP2(int x, int y, const char* file, int ignoreColor);
unsigned int iLoadImage(const char* file);
void iShowImage(int x, int y, int w, int h, unsigned int tex);

// audio (MCI-style alias API)
void mc(const char* cmd);

extern int iScreenWidth, iScreenHeight;
extern int iMouseX, iMouseY;

#ifdef __cplusplus
}
#endif
```

> The only signature hardening: `char filename[]` -> `const char*` (source-compatible
> for callers) and GLUT key/font void-pointers mapped internally. Everything else —
> names, arity, defaults, constants, semantics — identical.

---

## 6. Renderer notes (OpenGL 3.3 core)

- **No immediate mode.** One batched dynamic buffer (vertices + color + uv + local
  transform), flushed per draw group.
- **Two shaders:** (1) untextured color, (2) textured with `GL_REPLACE` semantics
  (ignore vertex color, discard alpha==0). This cleanly reproduces AUST behavior.
- **Tessellation:** reuse AUST's geometry loops verbatim (same slices, same
  stop conditions) so vertex counts match the reference's asserted numbers.
- **Transform stack:** `iRotate` pushes the concatenated 2D transform onto the batch
  matrix; `iUnRotate` pops.
- **Text:** bake a small 8x13 ASCII atlas at init, draw glyphs as textured quads at
  the `iText` position in current color.

---

## 7. Timers & fixed update (replacing SetTimer)

- One monotonic clock seeds both `fixedUpdate` and the 10-slot pool.
- `fixedUpdate`: fire every `rate` ms (default 16) — **no elapsed-time arg**, matching AUST.
- Timer slots: `{cb, period, nextFire, paused}`. `iSetTimer` returns the slot index
  (0–9) or `-1` when all full. Paused slots keep loaded but skipped, matching the
  Win32 "keep firing, skip body" behavior.
- All timer callbacks run on the main/GL thread (never race the renderer).

---

## 8. Audio (replacing MCI)

Keep the alias-string shape that AUST users already write:

```cpp
mc("open \"assets/music/background.mp3\" alias bgmusic");
mc("play bgmusic repeat");
mc("play hit from 0");     // retrigger one-shot
```

- **Backend:** miniaudio (CC0, no deps, Linux+Windows+mac) compiled once; `mc()` is a
  tiny parser that turns the MCI-like string into a `play/open/close/stop/pause/resume`
  action on a named sound.
- Same supported-ish formats (wav/mp3/flac — miniaudio handles OGG too, a bonus;
  AUST's "no OGG" limitation disappears but non-breaking).

---

## 9. Build system

- **CMake** (min 3.16), two targets:
  - `igraphics` — the static/shared library.
  - example + `tests/smoke.cpp`.
- Dependencies via `FetchContent` or vendored `third_party/`: **GLFW**, **stb_image**,
  **miniaudio**. All header/source, no system package assumption.
- `-DIGRAPHICS_BUILD_TESTS=ON` runs the reference §8 checklist (headless where the
  renderer can be, windowed otherwise).
- No `#pragma comment(lib)`, no MSVC-only pragmas; works with MSVC, Clang, GCC on
  Windows and Linux.

---

## 10. Implementation order (when code is greenlit)

1. CMake skeleton + vendored deps + blank GLFW window (Windows + Linux both open).
2. backend interface + region setting: ortho, clear, swap, input, timer pool, fixed clock.
3. primitive batch + shaders (untextured color path) with vertex-count smoke asserts.
4. texture path: `iLoadImage` (stb) + `iShowImage` (GL_REPLACE, neg-V) — upright test.
5. `iShowBMP`/`iShowBMP2` via standalone loader + red-low-byte key.
6. `iRotate/iUnRotate`, `iText` atlas, `iGetPixelColor`.
7. audio alias layer via miniaudio.
8. port reference §8 smoke checklist; verify doc §6 quirks with assertions.

---

## 11. Risks / open questions

- **`iText` default font** is `GLUT_BITMAP_8_BY_13` — need a matching baked atlas
  (8x13 pixels/glyph) so old pixel-perfect HUDs look the same. Low risk.
- **GLUT `void* font`** differs per platform in the old lib; port maps any `NULL` to
  the default and supports a couple of known sizes, or exposes an opaque font handle.
- **`glDrawPixels`** (used by `iShowBMP`) has no GL3 equivalent — must go through a
  texture. Same visual result.
- **Wayland** vs X11: GLFW handles both; no code difference.
- AUST's audio had *no* OGG; miniaudio adds it — verify this doesn't clash with any
  "no OGG" assumption in the case-study pipelines (it shouldn't; it only widens support).

*Generated 2026-08-28. Design stage — no code written yet.*
