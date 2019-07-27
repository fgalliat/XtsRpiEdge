#include <csignal>
#ifndef XTSCONSOLE
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#endif
#include "Sound_Queue.h"
#include "apu.hpp"
#include "cartridge.hpp"
#include "cpu.hpp"
#include "menu.hpp"
#include "gui.hpp"
#include "config.hpp"

#ifdef XTSCONSOLE
 #include "../XtsConsole.h"
 extern XtsConsole console;
 extern Pad* pad;
 static u32 __tickNb = 0;
 uint32_t SDL_GetTicks() {
      //return (u32)console.now();
      __tickNb++;
      return __tickNb;
 }
#endif

namespace GUI {

// Screen size:
const unsigned WIDTH  = 256;
const unsigned HEIGHT = 240;

#ifndef XTSCONSOLE
// SDL structures:
SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* gameTexture;
SDL_Texture* background;
SDL_Joystick* joystick[] = { nullptr, nullptr };
TTF_Font* font;
#endif
u8 const* keys;
Sound_Queue* soundQueue;

// Menus:
Menu* menu;
Menu* mainMenu;
Menu* settingsMenu;
Menu* videoMenu;
// Menu* keyboardMenu[2];
// Menu* joystickMenu[2];
FileMenu* fileMenu;

bool pause = true;

/* Set the window size multiplier */
void set_size(int mul)
{
    last_window_size = mul;
    #ifndef XTSCONSOLE
    SDL_SetWindowSize(window, WIDTH * mul, HEIGHT * mul);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    #endif
}


#ifdef XTSCONSOLE 
u16 pixels_copy[256*240]; 
void *videoThread(void *argument) {
  static const int w = 256;
  static const int h = 240;
  static const int x = ( 320-w ) / 2;
  static const int y = ( 240-h ) / 2;
  while( true ) {
  	if ( !pause ) {
		console.getScreen()->drawRGB16(x,y,w,h,pixels_copy);
  	}
	console.delay(50);
  }
  return NULL;
}
#endif


/* Initialize GUI */
void init()
{
// printf("GUI::init 1\n");
    // Initialize graphics system:
    #ifndef XTSCONSOLE
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    TTF_Init();

    for (int i = 0; i < SDL_NumJoysticks(); i++)
        joystick[i] = SDL_JoystickOpen(i);
    APU::init();
    soundQueue = new Sound_Queue;
    soundQueue->init(96000);
    #else
    pthread_t thread1, thread2, thread3;
	int i1,i2,i3;
	// i1 = pthread_create( &thread1, NULL, keyThread, (void*) NULL);
	i2 = pthread_create( &thread2, NULL, videoThread, (void*) NULL);
    #endif

// printf("GUI::init (5)\n");

#ifndef XTSCONSOLE
    // Initialize graphics structures:
    window      = SDL_CreateWindow  ("LaiNES",
                                     SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                     WIDTH * last_window_size, HEIGHT * last_window_size, 0);
printf("GUI::init (5.1)\n");
    renderer    = SDL_CreateRenderer(window, -1,
                                     SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
printf("GUI::init (5.2)\n");
    SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);
printf("GUI::init (5.3)\n");
    gameTexture = SDL_CreateTexture (renderer,
                                     SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
                                     WIDTH, HEIGHT);

printf("GUI::init (6)\n");
    font = TTF_OpenFont("res/font.ttf", FONT_SZ);
    keys = SDL_GetKeyboardState(0);

printf("GUI::init (7)\n");
    // Initial background:
    SDL_Surface* backSurface  = IMG_Load("res/init.png");
    background = SDL_CreateTextureFromSurface(renderer, backSurface);
    SDL_SetTextureColorMod(background, 60, 60, 60);
    SDL_FreeSurface(backSurface);

#endif
printf("GUI::init (8)\n");
    // Menus:
    mainMenu = new Menu;
    mainMenu->add(new Entry("Load ROM", []{ menu = fileMenu; }));
#ifndef XTSCONSOLE
    mainMenu->add(new Entry("Settings", []{ menu = settingsMenu; }));
#endif
    mainMenu->add(new Entry("Exit",     []{ exit(0); }));

    settingsMenu = new Menu;
    settingsMenu->add(new Entry("<",            []{ menu = mainMenu; }));
    settingsMenu->add(new Entry("Video",        []{ menu = videoMenu; }));
#ifndef XTSCONSOLE
    settingsMenu->add(new Entry("Controller 1", []{ menu = useJoystick[0] ? joystickMenu[0] : keyboardMenu[0]; }));
    settingsMenu->add(new Entry("Controller 2", []{ menu = useJoystick[1] ? joystickMenu[1] : keyboardMenu[1]; }));
#endif
    settingsMenu->add(new Entry("Save Settings", []{ save_settings(); menu = mainMenu; }));

    videoMenu = new Menu;
    videoMenu->add(new Entry("<",       []{ menu = settingsMenu; }));
    videoMenu->add(new Entry("Size 1x", []{ set_size(1); }));
    videoMenu->add(new Entry("Size 2x", []{ set_size(2); }));
    videoMenu->add(new Entry("Size 3x", []{ set_size(3); }));
    videoMenu->add(new Entry("Size 4x", []{ set_size(4); }));
#ifndef XTSCONSOLE
    for (int i = 0; i < 2; i++)
    {
        keyboardMenu[i] = new Menu;
        keyboardMenu[i]->add(new Entry("<", []{ menu = settingsMenu; }));
        #ifndef XTSCONSOLE
        if (joystick[i] != nullptr)
            keyboardMenu[i]->add(new Entry("Joystick >", [=]{ menu = joystickMenu[i]; useJoystick[i] = true; }));
        keyboardMenu[i]->add(new ControlEntry("Up",     &KEY_UP[i]));
        keyboardMenu[i]->add(new ControlEntry("Down",   &KEY_DOWN[i]));
        keyboardMenu[i]->add(new ControlEntry("Left",   &KEY_LEFT[i]));
        keyboardMenu[i]->add(new ControlEntry("Right",  &KEY_RIGHT[i]));
        keyboardMenu[i]->add(new ControlEntry("A",      &KEY_A[i]));
        keyboardMenu[i]->add(new ControlEntry("B",      &KEY_B[i]));
        keyboardMenu[i]->add(new ControlEntry("Start",  &KEY_START[i]));
        keyboardMenu[i]->add(new ControlEntry("Select", &KEY_SELECT[i]));

        if (joystick[i] != nullptr)
        {
            joystickMenu[i] = new Menu;
            joystickMenu[i]->add(new Entry("<", []{ menu = settingsMenu; }));
            joystickMenu[i]->add(new Entry("< Keyboard", [=]{ menu = keyboardMenu[i]; useJoystick[i] = false; }));
            joystickMenu[i]->add(new ControlEntry("Up",     &BTN_UP[i]));
            joystickMenu[i]->add(new ControlEntry("Down",   &BTN_DOWN[i]));
            joystickMenu[i]->add(new ControlEntry("Left",   &BTN_LEFT[i]));
            joystickMenu[i]->add(new ControlEntry("Right",  &BTN_RIGHT[i]));
            joystickMenu[i]->add(new ControlEntry("A",      &BTN_A[i]));
            joystickMenu[i]->add(new ControlEntry("B",      &BTN_B[i]));
            joystickMenu[i]->add(new ControlEntry("Start",  &BTN_START[i]));
            joystickMenu[i]->add(new ControlEntry("Select", &BTN_SELECT[i]));
        }
        #endif
    }
#endif
    fileMenu = new FileMenu;

    menu = mainMenu;
}

#ifndef XTSCONSOLE
/* Render a texture on screen */
void render_texture(SDL_Texture* texture, int x, int y)
{
    int w, h;
    SDL_Rect dest;

    SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);
    if (x == TEXT_CENTER)
        dest.x = WIDTH/2 - dest.w/2;
    else if (x == TEXT_RIGHT)
        dest.x = WIDTH - dest.w - 10;
    else
        dest.x = x + 10;
    dest.y = y + 5;

    SDL_RenderCopy(renderer, texture, NULL, &dest);
}

/* Generate a texture from text */
SDL_Texture* gen_text(std::string text, SDL_Color color)
{
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_FreeSurface(surface);
    return texture;
}
#endif

/* Get the joypad state from SDL */
u8 get_joypad_state(int n)
{

    const int DEAD_ZONE = 8000;
    u8 j = 0;

#ifdef XTSCONSOLE

    if (pad == NULL) { return 0; }
    if (n == 1) { return 0; } // 2nd joypad

    // polled inside run()
        j |= (pad->bt1())      << 0; // A
        j |= (pad->bt2())      << 1; // B
        // j |= (keys[KEY_SELECT[n]]) << 2; // keys[...] not defined w/o SDLs
        j |= (pad->start())  << 3;
        j |= (pad->up())     << 4;
        j |= (pad->down())   << 5;
        j |= (pad->left())   << 6;
        j |= (pad->right())  << 7;

#else
    if (useJoystick[n])
    {
        j |= (SDL_JoystickGetButton(joystick[n], BTN_A[n]))      << 0;  // A.
        j |= (SDL_JoystickGetButton(joystick[n], BTN_B[n]))      << 1;  // B.
        j |= (SDL_JoystickGetButton(joystick[n], BTN_SELECT[n])) << 2;  // Select.
        j |= (SDL_JoystickGetButton(joystick[n], BTN_START[n]))  << 3;  // Start.

        j |= (SDL_JoystickGetButton(joystick[n], BTN_UP[n]))     << 4;  // Up.
        j |= (SDL_JoystickGetAxis(joystick[n], 1) < -DEAD_ZONE)  << 4;
        j |= (SDL_JoystickGetButton(joystick[n], BTN_DOWN[n]))   << 5;  // Down.
        j |= (SDL_JoystickGetAxis(joystick[n], 1) >  DEAD_ZONE)  << 5;
        j |= (SDL_JoystickGetButton(joystick[n], BTN_LEFT[n]))   << 6;  // Left.
        j |= (SDL_JoystickGetAxis(joystick[n], 0) < -DEAD_ZONE)  << 6;
        j |= (SDL_JoystickGetButton(joystick[n], BTN_RIGHT[n]))  << 7;  // Right.
        j |= (SDL_JoystickGetAxis(joystick[n], 0) >  DEAD_ZONE)  << 7;
    }
    else
    {
        j |= (keys[KEY_A[n]])      << 0;
        j |= (keys[KEY_B[n]])      << 1;
        j |= (keys[KEY_SELECT[n]]) << 2;
        j |= (keys[KEY_START[n]])  << 3;
        j |= (keys[KEY_UP[n]])     << 4;
        j |= (keys[KEY_DOWN[n]])   << 5;
        j |= (keys[KEY_LEFT[n]])   << 6;
        j |= (keys[KEY_RIGHT[n]])  << 7;
    }
#endif

    return j;
}

/* Send the rendered frame to the GUI */
#ifndef XTSCONSOLE
void new_frame(u32* pixels)
{
    SDL_UpdateTexture(gameTexture, NULL, pixels, WIDTH * sizeof(u32));
}
#else
int frameCpt =  0;
void new_frame(u16* pixels)
{
	// if ( frameCpt++ % 10 > 0 ) { return; }
 //   static const int w = 256;
 //   static const int h = 240;
 //   static const int x = ( 320-w ) / 2;
 //   static const int y = ( 240-h ) / 2;
 //   console.getScreen()->drawRGB16(x,y,w,h,pixels);
 memcpy(pixels_copy, pixels, 256*240*2);
}
#endif

void new_samples(const blip_sample_t* samples, size_t count)
{
#ifndef XTSCONSOLE
    soundQueue->write(samples, count);
#endif
}

/* Render the screen */
#ifndef XTSCONSOLE
void render()
{
    SDL_RenderClear(renderer);

    // Draw the NES screen:
    if (Cartridge::loaded())
        SDL_RenderCopy(renderer, gameTexture, NULL, NULL);
    else
        SDL_RenderCopy(renderer, background, NULL, NULL);

    // Draw the menu:
    if (pause) menu->render();

    SDL_RenderPresent(renderer);
}
#else
void render() {
    // Draw the menu:
    if (pause) menu->render();  
}
#endif

/* Play/stop the game */
void toggle_pause()
{
// printf("toggle_pause.1\n");
    pause = not pause;
    menu  = mainMenu;
#ifndef XTSCONSOLE
    if (pause)
        SDL_SetTextureColorMod(gameTexture,  60,  60,  60);
    else
        SDL_SetTextureColorMod(gameTexture, 255, 255, 255);
#else
if (pause) {
console.getScreen()->cls();
console.getScreen()->dispStr("PAUSE", 60, 60, 1);
}
#endif
// printf("toggle_pause.2\n");
}

/* Prompt for a key, return the scancode */
#ifndef XTSCONSOLE
SDL_Scancode query_key()
{
    SDL_Texture* prompt = gen_text("Press a key...", { 255, 255, 255 });
    render_texture(prompt, TEXT_CENTER, HEIGHT - FONT_SZ*4);
    SDL_RenderPresent(renderer);

    SDL_Event e;
    while (true)
    {
        SDL_PollEvent(&e);
        if (e.type == SDL_KEYDOWN)
            return e.key.keysym.scancode;
    }
}

int query_button()
{
    SDL_Texture* prompt = gen_text("Press a button...", { 255, 255, 255 });
    render_texture(prompt, TEXT_CENTER, HEIGHT - FONT_SZ*4);
    SDL_RenderPresent(renderer);

    SDL_Event e;
    while (true)
    {
        SDL_PollEvent(&e);
        if (e.type == SDL_JOYBUTTONDOWN)
            return e.jbutton.button;
    }
}
#endif

/* Run the emulator */
void run()
{
    #ifndef XTSCONSOLE
    SDL_Event e;
    #endif

    // Framerate control:
    u32 frameStart, frameTime;
    const int FPS   = 60;
    const int DELAY = 1000.0f / FPS;

    while (true)
    {
        frameStart = SDL_GetTicks();

#ifndef XTSCONSOLE
        // Handle events:
        while (SDL_PollEvent(&e))
            switch (e.type)
            {
                case SDL_QUIT: return;
                case SDL_KEYDOWN:
                    if (keys[SDL_SCANCODE_ESCAPE] and Cartridge::loaded())
                        toggle_pause();
                    else if (pause)
                        menu->update(keys);
            }
#else
//    printf("poll console \n"); // it always do...
// console.delay(30); // TMP slow down keypad

if (pause) { // TMP : remove keypad listening while play...

  pad = console.readPad();
//   printf("polled console \n");
  if (pad == NULL) printf("null pad \n");
  u8 _keys[128];
  _keys[SDL_SCANCODE_RETURN] = pad->start();
  _keys[SDL_SCANCODE_DOWN] = pad->down();
  _keys[SDL_SCANCODE_UP] = pad->up();
  _keys[SDL_SCANCODE_LEFT] = pad->left();
  _keys[SDL_SCANCODE_RIGHT] = pad->right();
//   if ( pad->hasChanged() && pause ) {
  if ( pad->atLeastOne() && pause ) {
      if ( pad->hasChanged() ) {
        //   printf("run.A \n");
        menu->update(_keys);
      }
  }
  
}  
#endif
// printf("run.B \n");
        if (not pause) CPU::run_frame();
// printf("run.C \n");
        render();

        // Wait to mantain framerate:
        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < DELAY) {
            #ifndef XTSCONSOLE
            SDL_Delay((int)(DELAY - frameTime));
            #else
           // console.delay( (int)(DELAY - frameTime) );
            #endif
        }
    }
}


}
