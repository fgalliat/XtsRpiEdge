/**
 * legacy XtsConsole tiny impl. for Rpi w/ 1.44 LCD/pad Hat
 * 
 * Xtase - fgalliat @Jul2019
 * 
 */

#include <time.h>
#include <stdint.h>
#include <unistd.h>

#include "./XtsConsole.h"

Pad _pad;
WiredScreen fbScreen;

#ifndef MY_PC
 // RpiMode
 #define MODE_OUT_SDL 0
 #define MODE_OUT_FB 1

 #define MODE_IN_SDL 0
 #define MODE_IN_NCURSES 1
 #define MODE_IN_GPIO 0
#else
 // Desktop
 #define MODE_OUT_SDL 1
 #define MODE_OUT_FB 0

 #define MODE_IN_SDL 1
 #define MODE_IN_NCURSES 0
 #define MODE_IN_GPIO 0
#endif

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

uint16_t rgb(uint8_t r,uint8_t g,uint8_t b) {return (uint16_t)( (( r *31/255 )<<11) | (( g *63/255 )<<5) | ( b *31/255 ) );}

#include "./cpp/arch/desktop/screen/WiredScreen.h"
const uint16_t CLR_GREEN = rgb(0,255,0);
const uint16_t CLR_BLACK = rgb(0,0,0);
const uint16_t CLR_WHITE = rgb(255,255,255);

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#if MODE_IN_SDL
  #include <SDL2/SDL.h>
  extern int _k_KEYS;

  int lastK = 0;
  void pollPad() {
      // TODO : better impl
      _pad._left = false; _pad._right = false;
      _pad._up = false; _pad._down = false;
      _pad._bt1 = false; _pad._bt2 = false;
      _pad._start = false;

      _pad._atLeastOne = _k_KEYS != 0;
      _pad._hasChanged = lastK != _k_KEYS;
      lastK = _k_KEYS;

      if ( _k_KEYS == SDLK_LEFT ) { _pad._left = true; }
      if ( _k_KEYS == SDLK_RIGHT ) { _pad._right = true; }
      if ( _k_KEYS == SDLK_UP ) { _pad._up = true; }
      if ( _k_KEYS == SDLK_DOWN ) { _pad._down = true; }

      if ( _k_KEYS == SDLK_s ) { _pad._start = true; }
      if ( _k_KEYS == SDLK_d ) { _pad._bt1 = true; }
      if ( _k_KEYS == SDLK_f ) { _pad._bt2 = true; }
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

bool Pad::atLeastOne() { return this->_atLeastOne; }
bool Pad::hasChanged() { return this->_hasChanged; }

bool Pad::bt1() { return this->_bt1; }
bool Pad::bt2() { return this->_bt2; }
bool Pad::start() { return this->_start; }

bool Pad::left() { return this->_left; }
bool Pad::right() { return this->_right; }
bool Pad::up() { return this->_up; }
bool Pad::down() { return this->_down; }

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

XtsConsole::XtsConsole() {
    this->currentPad = &_pad;
    this->screen = &fbScreen;
}

XtsConsole::~XtsConsole() {

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
      usleep(time*1000);
    #endif
}

Pad* XtsConsole::readPad() {
    pollPad();
    return this->currentPad;
}

WiredScreen* XtsConsole::getScreen() {
    return this->screen;
}

