LaiNes

- gui.cpp => new_frame(u32* pixels)

- gui.render()

- gui.run()  **<= HERE**

  - SDL poll

  gui::get_joypad_state(int joystickNum) => line 201

  ppu.cpp:272:    else if (s == POST and dot == 0) GUI::new_frame(pixels);

  pixels[256x240]

  ppu.cpp:258  pixels[scanline*256 + x] = nesRgb[rd(0x3F00 + (rendering() ? palette : 0))];

  -> nesRgb -> palette.inc => u32 nesRgb[] => 8x8 (64 colors)

   -> 0x7C7C7C ===> 7c => 124,124,124 => RGB 255 per color

  so convert each colors as 565 & produce an alt. palette include (for XtsConsole)

