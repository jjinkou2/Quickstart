#include "simpletools.h"                      // Include simple tools
#include "fdserial.h"

// Midi Files
// Use  bin2c to convert .mid file 
// example : ./bin2c.exe F5.mid F5.h

#include "bridge.h"
#include "F2.h"
#include "intro.h"
#include "Ride.h"
#include "Open.h"
#include "Hat.h"
#include "F1.h"
#include "F3.h"
#include "F5.h"

void    msleep(int ms);          // sleep for ms
int     getButtons();           // used to get button states
void    noteOn(fdserial *midiout, int cmd,int data1, int data2);  // send note to midiout
    
void main()
{
  int first_touch = 0xA;    // return from getbuttons
  int previous_touch = first_touch;
  DIRA |= 0x00ff0000;           // set LEDs for output
  fdserial *midiout;
  
#define __DEBUG
  /* start device */
#ifndef __DEBUG
  midiout = fdserial_open(31,30,0,115200);
#endif

  
  print ("Intro size : %d\n",Intro_size);
  print ("F2 Size : %d\n",F2_size);
  print ("Bridge size : %d\n\nData:\n-----\n",bridge_size);

  for (int i = 0 ;i<bridge_size; i++){ 
     print ("0x%x, ",bridge_array[i]);
     if ((i%8==0)&&(i!=0)) print("\n");
  }    
  int touch;
  while(1) {
    touch = getButtons();
    print("touched: %08b\n",touch);
    pause(500);
  }  

}



void noteOn(fdserial *midiout, int cmd,int data1, int data2){
  writeChar(midiout, cmd);
  writeChar(midiout, data1);
  writeChar(midiout, data2);
}  


int getButtons()
{
    int n = 16;
    int result  = -1;
    int reading =  0;
    int pins = 0xFF;
    int leds;
    
    OUTA |= pins;               //output high to buttons
    
    while(n--)
    {
        DIRA |= pins;           //output high to buttons
        DIRA ^= pins;           //switch to input
        reading = pins;
        msleep(2);              //wait for RC time
        reading &= INA;          //return button states
        result  &= reading;
    }
    result = ~result & 0xFF;
    leds = result << 16;      // copy pads state to led
    OUTA &= 0xff00ffff;       // clear LED
    OUTA |= leds;             // switch them on
    
    return result;
}
void msleep(int ms){
  waitcnt(CLKFREQ/1000 * ms + CNT);
}  

