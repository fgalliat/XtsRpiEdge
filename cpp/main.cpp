#include <stdio.h>

#include <ncurses.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <wiringPi.h>
#include <wiringPiSPI.h>

#include "./Debug.h"

uint16_t rgb(uint8_t r,uint8_t g,uint8_t b) {return (uint16_t)( (( r *31/255 )<<11) | (( g *63/255 )<<5) | ( b *31/255 ) );}

#include "./arch/desktop/screen/WiredScreen.h"
const uint16_t CLR_GREEN = rgb(0,255,0);
const uint16_t CLR_BLACK = rgb(0,0,0);
const uint16_t CLR_WHITE = rgb(255,255,255);


// if use 2019-06-20-raspbian-buster
// sudo nano /boot/config.txt
// add:
// gpio=6,19,5,26,13,21,20,16=pu

#include "./DEV_Config.h"

bool initGPIO() {
	if(wiringPiSetupGpio() < 0) { //use BCM2835 Pin number table
        DEBUG("set wiringPi lib failed	!!! \r\n");
        return false;
    } else {
        DEBUG("set wiringPi lib success  !!! \r\n");
    }
    // wiringPiSPISetup(0,9000000);
    
    DEV_GPIO_Mode(KEY_UP_PIN, INPUT);
    DEV_GPIO_Mode(KEY_DOWN_PIN, INPUT);
    DEV_GPIO_Mode(KEY_LEFT_PIN, INPUT);
    DEV_GPIO_Mode(KEY_RIGHT_PIN, INPUT);
    
    DEV_GPIO_Mode(KEY1_PIN, INPUT);
    DEV_GPIO_Mode(KEY2_PIN, INPUT);
    DEV_GPIO_Mode(KEY3_PIN, INPUT);
return true;
}

int main(int argc, char** argv) {
	
	
	WiredScreen screen;
	screen.init(1); // here fb1 impl.
	

  // system("clear");
  printf("HelloWorld\n");

	if( !initGPIO() ) {
        DEBUG("set GPIO failed	!!! \r\n");
        return 1;
    }    
    
    // ==================================

	//initscr();
	screen.cls();

#define TTY_LCD 0
#define RESTART_VNC 1
#define STOP_VNC 2
#define SHOW_NET 3
#define REBOOT 4
#define HALT 5
#define DEMO 6
#define EXIT 7

	int itm = 0;
	int nbItems=7;
	int i,cpt=0;
	bool eject = false,runDemoFork=false;
	while(true) {
		
		screen.cls();
		screen.drawAnimatedBackground(1,0,0,0,0); // starfield
		
		
		if ( cpt == 10 ) {
			//clear();
			//screen.cls();
			cpt = 0;
		}
		
		i = 0;
		//move(i,0);
		screen.setCursor(0,0);
		screen.println(" [ tty to HDMI ]\n");
		i++; screen.setCursor(0,i); //move(i,0);
		screen.println(" [ ReStart VNC ]\n");
		i++; screen.setCursor(0,i); //move(i,0);
		screen.println(" [ Stop VNC    ]\n");
		i++; screen.setCursor(0,i); //move(i,0);
		screen.println(" [ Show Network]\n");
		i++; screen.setCursor(0,i); //move(i,0);
		screen.println(" [ Reboot      ]\n");
		i++; screen.setCursor(0,i); //move(i,0);
		screen.println(" [ Halt        ]\n");
		i++; screen.setCursor(0,i); //move(i,0);
		screen.println(" [ -= Menu =-  ]\n");
		i++; screen.setCursor(0,i); //move(i,0);
		screen.println(" [ Exit        ]\n");
		//refresh();
		
		//move(itm, 0);
		screen.setCursor(0,itm);
		screen.println(">");
		//refresh();
		
		
		//move(LINES/2, COLS/2); //cursor
		screen.setCursor(5,5);
		
		if(GET_KEY_UP == 0) {
            while(GET_KEY_UP == 0) {
                DEV_Delay_ms(5);
            }
            screen.println("Up\n");
            itm--; { if ( itm < 0 ) { itm = nbItems+itm; } } itm %= nbItems;

        }
        if(GET_KEY_DOWN == 0) {
            while(GET_KEY_DOWN == 0) {
                DEV_Delay_ms(5);
            }
            screen.println("Down\n");
            itm++; itm %= nbItems;
        }
        if(GET_KEY_LEFT == 0) {
            while(GET_KEY_LEFT == 0) {
                DEV_Delay_ms(5);
            }
            screen.println("Left\n");
        }
        if(GET_KEY_RIGHT == 0) {
            while(GET_KEY_RIGHT == 0) {
                DEV_Delay_ms(5);
            }
            screen.println("Right\n");
        }
        if(GET_KEY1 == 0) {
            while(GET_KEY1 == 0) {
                DEV_Delay_ms(5);
            }
            screen.println("[K1]\n");
            
            if ( itm == RESTART_VNC ) {
            	// system("vncserver -kill :1 ; USER=pi HOME=/home/pi vncserver -depth 8 :1 ");
            	// as root rc.local way
            	system("su - pi -c \"vncserver -kill :1\" ; su - pi -c \"/usr/bin/vncserver -depth 8 :1\" 2>&1 | tee /home/pi/err.txt");
            } else if ( itm == STOP_VNC ) {
            	system("su - pi -c \"vncserver -kill :1\""); // from stdlib.h
            } else if ( itm == TTY_LCD ) {
            	system("chvt 2"); // 1 is hatLcd
            	break; // if not launched as '&' must break to obtain login prompt
            } else if ( itm == SHOW_NET ) {
            	system("ifconfig wlan0 | grep \"inet \"");
            	while(GET_KEY1 != 0) {
	                DEV_Delay_ms(5);
	            }
	            while(GET_KEY1 == 0) {
	                DEV_Delay_ms(5);
	            }
            } else if ( itm == REBOOT ) {
            	system("sudo reboot"); // from stdlib.h
            } else if ( itm == EXIT ) {
            	eject = true;
            	break;
            } else if ( itm == DEMO ) {
            	runDemoFork = true;
            	break;
            } else if ( itm == HALT ) {
            	system("sudo halt"); // from stdlib.h
            }  
            
        }
        if(GET_KEY2 == 0) {
            while(GET_KEY2 == 0) {
                DEV_Delay_ms(5);
            }
            screen.println("[K2]\n");
        }
        if(GET_KEY3 == 0) {
            while(GET_KEY3 == 0) {
                DEV_Delay_ms(5);
            }
            screen.println("[K3]\n");
            
            break;
        }
        
        //refresh();
        DEV_Delay_ms(50);
        cpt++;
	}
	//endwin();
	
	if ( runDemoFork ) {
		system("/home/pi/menu");
	}

  return 0;
}
