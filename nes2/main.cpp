#include "gui.hpp"
#include "config.hpp"

#ifdef XTSCONSOLE
 #include "../XtsConsole.h"
 XtsConsole console;
 Pad* pad;
#endif

void *runThread(void *argument) {
	 GUI::run();
	 return NULL;
}



int main(int argc, char *argv[])
{
        #ifdef XTSCONSOLE
          if ( ! console.init() ) {
              return -1;
          }
          console.delay(500);
        //   return 1;
        #endif

    GUI::load_settings();
    GUI::init();
    // GUI::run();

	pthread_t thread1;
	int i1;
	i1 = pthread_create( &thread1, NULL, runThread, (void*) NULL);

while(true) {
	console.delay(1000);
}

    return 0;
}
