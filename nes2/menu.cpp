#include <dirent.h>
#include <unistd.h>
#include "cartridge.hpp"
#include "menu.hpp"

#ifdef XTSCONSOLE
 #include "../XtsConsole.h"
 extern XtsConsole console;
#endif

namespace GUI {

using namespace std;

Menu::Menu()
{
    #ifdef XTSCONSOLE
    console.getScreen()->cls();
    #endif
}

Entry::Entry(string label, function<void()> callback, int x, int y) : callback(callback), x(x), y(y)
{
    setLabel(label);
}

Entry::~Entry()
{
#ifndef XTSCONSOLE
    SDL_DestroyTexture(whiteTexture);
    SDL_DestroyTexture(redTexture);
#endif
}

void Entry::setLabel(string label)
{
    this->label = label;
#ifndef XTSCONSOLE
    if (whiteTexture != nullptr) SDL_DestroyTexture(whiteTexture);
    if (redTexture   != nullptr) SDL_DestroyTexture(redTexture);

    whiteTexture = gen_text(label, { 255, 255, 255 });
    redTexture   = gen_text(label, { 255,   0,   0 });
#endif
}

void Entry::render()
{
#ifndef XTSCONSOLE
    render_texture(selected ? redTexture : whiteTexture, getX(), getY());
#else
    // console.getScreen()->dispStr(this->label, getX(), getY(), selected ? CLR_RED : CLR_WHITE );
    const char* str = this->label.c_str();
    int x = getX(); if (x < 0) { x = 5; } // x may be -1 !!
    int y = getY(); 
    static const u16 CL_RED = 10;
    static const u16 CL_WHITE = 1;
    console.getScreen()->dispStr((char*)str, x, y, selected ? CL_RED : CL_WHITE );
#endif
}

#ifndef XTSCONSOLE
ControlEntry::ControlEntry(string action, SDL_Scancode* key, int x, int y) : key(key),
    Entry::Entry(
        action,
        [&]{ keyEntry->setLabel(SDL_GetScancodeName(*(this->key) = query_key())); },
        x,
        y)
{
    this->keyEntry = new Entry(SDL_GetScancodeName(*key), []{}, TEXT_RIGHT, y);
}

ControlEntry::ControlEntry(string action, int* button, int x, int y) : button(button),
    Entry::Entry(
        action,
        [&]{ keyEntry->setLabel(to_string(*(this->button) = query_button())); },
        x,
        y)
{
    this->keyEntry = new Entry(to_string(*button), []{}, TEXT_RIGHT, y);
}
#endif


void Menu::add(Entry* entry)
{
    if (entries.empty()) {
        entry->select();
    }
    entry->setY(entries.size() * FONT_SZ);
    entries.push_back(entry);
}

void Menu::clear()
{
    for (auto entry : entries)
        delete entry;
    entries.clear();
    cursor = 0;
#ifdef XTSCONSOLE
  console.getScreen()->cls();
#endif
}

void Menu::update(u8 const* keys)
{
    int oldCursor = cursor;

    if (keys[SDL_SCANCODE_DOWN] and cursor < entries.size() - 1)
        cursor++;
    else if (keys[SDL_SCANCODE_UP] and cursor > 0)
        cursor--;

    entries[oldCursor]->unselect();
    entries[cursor]->select();

    if (keys[SDL_SCANCODE_RETURN]) {
        #ifdef XTSCONSOLE
          console.getScreen()->cls();
        #endif
        entries[cursor]->trigger();
    }
}

void Menu::render()
{
    for (auto entry : entries)
        entry->render();
}


void FileMenu::change_dir(string dir)
{
    clear();

    struct dirent* dirp;
    DIR* dp = opendir(dir.c_str());

    while ((dirp = readdir(dp)) != NULL)
    {
        string name = dirp->d_name;
        string path = dir + "/" + name;

        if (name[0] == '.' and name != "..") continue;

        if (dirp->d_type == DT_DIR)
        {
            add(new Entry(name + "/",
                          [=]{ change_dir(path); },
                          0));
        }
        else if (name.size() > 4 and name.substr(name.size() - 4) == ".nes")
        {
            add(new Entry(name,
                          [=]{ Cartridge::load(path.c_str()); toggle_pause(); },
                          0));
        }
    }
    closedir(dp);
}

FileMenu::FileMenu()
{
    char cwd[512];

    change_dir(getcwd(cwd, 512));
}


}
