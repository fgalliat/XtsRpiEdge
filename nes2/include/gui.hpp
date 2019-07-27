#pragma once

#include <stdint.h>

#ifndef XTSCONSOLE
#include <SDL2/SDL.h>
#else
  #define SDL_SCANCODE_UP 82 
  #define SDL_SCANCODE_DOWN 81
  #define SDL_SCANCODE_LEFT 80
  #define SDL_SCANCODE_RIGHT 79
  #define SDL_SCANCODE_RETURN 13 
  uint32_t SDL_GetTicks();
#endif
#include <string>
#include <Nes_Apu.h>
#include "common.hpp"

namespace GUI {


const int TEXT_CENTER  = -1;
const int TEXT_RIGHT   = -2;
const unsigned FONT_SZ = 15;

void init();
void toggle_pause();
#ifndef XTSCONSOLE
SDL_Scancode query_key();
#endif
int query_button();
void run();

#ifndef XTSCONSOLE
SDL_Texture* gen_text(std::string text, SDL_Color color);
void render_texture(SDL_Texture* texture, int x, int y);
#endif

u8 get_joypad_state(int n);
void new_samples(const blip_sample_t* samples, size_t count);
void set_size(int mul);

#ifndef XTSCONSOLE
void new_frame(u32* pixels);
#else
void new_frame(u16* pixels);
#endif

}
