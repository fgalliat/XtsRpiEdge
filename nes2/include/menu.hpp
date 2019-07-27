#pragma once
#include <functional>
#ifndef XTSCONSOLE
#include <SDL2/SDL.h>
#else
  #define SDL_SCANCODE_UP 82 
  #define SDL_SCANCODE_DOWN 81
  #define SDL_SCANCODE_RETURN 13 
#endif
#include <string>
#include <vector>
#include "gui.hpp"

namespace GUI {


class Entry
{
    int x, y;

    std::string label;
    std::function<void()> callback;

    bool selected = false;
#ifndef XTSCONSOLE
    SDL_Texture* whiteTexture = nullptr;
    SDL_Texture* redTexture   = nullptr;
#endif
  public:
    Entry(std::string label, std::function<void()> callback = []{}, int x = TEXT_CENTER, int y = 0);
    ~Entry();

    virtual void setX(int x) { this->x = x; }
    virtual void setY(int y) { this->y = y; }
    int getX() { return x; }
    int getY() { return y; }
    void setLabel(std::string label);

    virtual void select()   { selected = true;  };
    virtual void unselect() { selected = false; };
    void trigger() { callback(); };
    virtual void render();
};

class ControlEntry : public Entry
{
  #ifndef XTSCONSOLE
    SDL_Scancode* key;
  #endif
    int* button;
    Entry* keyEntry;

  public:
  #ifndef XTSCONSOLE
    ControlEntry(std::string action, SDL_Scancode* key, int x = 0, int y = 0);
  #endif  
    ControlEntry(std::string action, int* button, int x = 0, int y = 0);
    void setY(int y) { Entry::setY(y);    keyEntry->setY(y);    }
    void select()    { Entry::select();   keyEntry->select();   }
    void unselect()  { Entry::unselect(); keyEntry->unselect(); }
    void render()    { Entry::render();   keyEntry->render();   }
};

class Menu
{
    std::vector<Entry*> entries;
    int cursor = 0;

  public:
    Menu();
    void add(Entry* entry);
    void clear();
    void update(u8 const* keys);
    void render();
};

class FileMenu : public Menu
{
    void change_dir(std::string dir);

  public:
    FileMenu();
};


}
