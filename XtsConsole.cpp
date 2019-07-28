/**
 * legacy XtsConsole tiny impl. for Rpi w/ 1.44 LCD/pad Hat
 * 
 * Xtase - fgalliat @Jul2019
 * 
 */

#include <time.h>

#include "./XtsConsole.h"

Pad pad;
WiredScreen fbScreen;


// defined by build script LATER
#define MODE_OUT_SDL 1
#define MODE_OUT_FB 0

#define MODE_IN_SDL 1
#define MODE_IN_NCURSES 0
#define MODE_IN_GPIO 0

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#if MODE_IN_SDL
  #include <SDL2/SDL.h>
  extern int _k_KEYS;


  void pollPad() {
      // TODO : better impl
      pad._left = false; pad._right = false;
      pad._up = false; pad._down = false;
      pad._bt1 = false; pad._bt2 = false;
      pad._start = false;

      if ( _k_KEYS == SDLK_LEFT ) { pad._left = true; }
      if ( _k_KEYS == SDLK_RIGHT ) { pad._right = true; }
      if ( _k_KEYS == SDLK_UP ) { pad._up = true; }
      if ( _k_KEYS == SDLK_DOWN ) { pad._down = true; }

      if ( _k_KEYS == SDLK_s ) { pad._start = true; }
      if ( _k_KEYS == SDLK_d ) { pad._bt1 = true; }
      if ( _k_KEYS == SDLK_f ) { pad._bt2 = true; }
  }
#elif MODE_IN_NCURSES
  void pollPad() {
     // ncurses way
  }
#else
  void pollPad() {
     // GPIO way
  }
#endif


bool Pad::bt1() { return this->_bt1; }
bool Pad::bt2() { return this->_bt2; }
bool Pad::start() { return this->_start; }

bool Pad::left() { return this->_left; }
bool Pad::right() { return this->_right; }
bool Pad::up() { return this->_up; }
bool Pad::down() { return this->_down; }

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

XtsConsole::XtsConsole() {
    this->pad = &pad;
    this->screen = &fbScreen;
}

bool XtsConsole::init() {
    bool ok = this->screen->init(0); // fb0
    // init GPIO / ncurses reading (TODO)
    return ok;
}

void XtsConsole::delay(long time) {
    #if MODE_OUT_SDL
      SDL_Delay(time);
    #else
      ::delay(time);
    #endif
}

Pad* XtsConsole::readPad() {
    pollPad();
    return this->currentPad;
}

WiredScreen* XtsConsole::getScreen() {
    return this->screen;
}

