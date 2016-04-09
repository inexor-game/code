#ifndef _CEF_CONTEXT_BINDINGS_H
#define _CEF_CONTEXT_BINDINGS_H

#include "inexor/engine/engine.hpp"
#include "inexor/fpsgame/game.hpp"

#include "inexor/rpc/SharedVar.hpp"

extern void quit();
extern void getfps(int &fps, int &bestdiff, int &worstdiff);

extern SharedVar<int> fullscreen, scr_w, scr_h, vsync;

namespace game {
    extern fpsent *player1;
    extern void switchname(const char *name, const char *tag);
}

#endif  // _CEF_CONTEXT_BINDINGS_H
