# fx-CG50 compatibility patch (untested)

Upstream: https://gitlab.com/balping/chess-prism (GPLv3, balping, v1.01).
Reported problem: runs on fx-CG50 but board and pieces are not displayed.

Change (src/chess.c, just before `#include "bstring/bstrlib.c"`):
- `GetKey(k)` now calls `Bdisp_PutDisp_DD()` first, so drawn VRAM reaches the screen.
- `SaveVRAM_1()` / `LoadVRAM_1()` replaced with a memcpy of `GetVRAMAddress()`
  (384*216*2 bytes) into a static buffer, in case the old syscalls misbehave on the CG50 OS.

Build (libfxcg / Prizm toolchain, `sh3eb-elf`):
    export FXCGSDK=/path/to/libfxcg   # the Makefile expects $(FXCGSDK)/common/prizm_rules
    make                              # produces cg50-chess.g3a (named after the directory; rename the folder to "chess" if you want that name)
Copy the .g3a to the calculator over USB.

Not compiled or run on hardware. If the board is still blank, the cause is elsewhere.
