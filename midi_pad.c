#include "fdserial.h"

/* simple utilities */
void    noteOn(fdserial *midiout, int cmd,int data1, int data2);  // send note to midiout
void    msleep(int ms);          // sleep for ms
int     getButton(int n);       // used to get button states
int     getButtons();           // used to get button states

int main()                                    // Main function
{
  // Add startup code here.
  /* start device */
  fdserial *midiout = fdserial_open(31,30,0,115200);
  int note;
  int CHANNEL = 9;
  while(1)
  {
   for (note=30;note<=90;note++){
      //Note on channel , some note value (note), high velocity (120):
      noteOn(midiout, 0x90+CHANNEL, note, 120);
      msleep(100);
      //Note off channel, some note value (note), velocity 0 = noteOff
      noteOn(midiout,0x90+CHANNEL, note, 0);
      msleep(100);
   }      
   msleep(1500);
  }  
}

/*
 * Get all buttons at once
 * Run this function from HUB if compiled for LMM.
 */
int getButtons()
{
    int n = 16;
    int result  = -1;
    int reading =  0;
    int pins = 0xFF;
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
    return result;
}

void msleep(int ms){
  waitcnt(CLKFREQ/1000 * ms + CNT);
}  

void noteOn(fdserial *midiout, int cmd,int data1, int data2){
  writeChar(midiout, cmd);
  writeChar(midiout, data1);
  writeChar(midiout, data2);
}  
