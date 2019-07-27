/**
 * legacy XtsConsole tiny impl. for Rpi w/ 1.44 LCD/pad Hat
 * 
 * Xtase - fgalliat @Jul2019
 * 
 */

#include "./XtsConsole.h"

bool Pad::bt1() { return false; }
bool Pad::bt2() { return false; }
bool Pad::start() { return false; }

bool Pad::left() { return false; }
bool Pad::right() { return false; }
bool Pad::up() { return false; }
bool Pad::down() { return false; }


Pad pad;
WiredScreen fbScreen;

XtsConsole::XtsConsole() {
    this->pad = &pad;
    this->screen = &fbScreen;
}

bool XtsConsole::init() {
    bool ok = this->screen->init(0); // fb0
    // init GPIO
    return ok;
}

void XtsConsole::delay(long time) {
    // ....
    // see if SDL or not
}

Pad* XtsConsole::readPad() {
    return this->currentPad;
}

WiredScreen* XtsConsole::getScreen() {
    return this->screen;
}

