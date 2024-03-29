#ifndef __LEGACY_XTSCONSOLE_H__
#define __LEGACY_XTSCONSOLE_H__ 1

/**
 * legacy XtsConsole tiny impl. for Rpi w/ 1.44 LCD/pad Hat
 * 
 * Xtase - fgalliat @Jul2019
 * 
 */

  #include <string.h>

  #include "./cpp/arch/desktop/screen/WiredScreen.h"

  class Pad {
      public:
        bool up();
        bool down();
        bool left();
        bool right();

        bool bt1();
        bool bt2();
        bool start();

        bool atLeastOne();
        bool hasChanged();

        bool _atLeastOne = false;
        bool _hasChanged = false;

        bool _up = false;
        bool _down = false;
        bool _left = false;
        bool _right = false;

        bool _bt1 = false;
        bool _bt2 = false;
        bool _start = false;
  };

  class XtsConsole {
      private:
        Pad* currentPad;
        WiredScreen* screen;
      public:
        XtsConsole();
        ~XtsConsole();
        bool init();
        WiredScreen* getScreen();
        void delay(long ms);
        Pad* readPad();
  };

#endif