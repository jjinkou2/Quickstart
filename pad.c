//
// file = QS_padscan.c
// propgcc conversion of JonnyMac's SPIN version of Quickstart Button Demo
// simple propgcc tutorial demo
//
// copyright 2011 by Rick Post
// terms of use below
//
#include <propeller.h>

int scan_pads(long int delay)
{
      OUTA |= 0x000000ff;                        // "charge" the pads - force them HIGH
      DIRA |= 0x000000ff;                        // set them as output
      DIRA &= 0xffffff00;                        // release touch pads back to input
    waitcnt(delay+CNT);                     // delay for a "touch"
                                            // a touched pad goes to LOW
                                            // so if you invert the input bits you can mask them out
                                            // and return the "touched" pads as HIGHS
    return ((~INA) & 0x000000ff);             // return the touched pads
}

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
        waitcnt (CLKFREQ / 500 + CNT); //pause
    //    msleep(2);              //wait for RC time
        reading &= INA;          //return button states
        result  &= reading;
    }
    result = ~result & 0xFF;
    return result;
}


int main(void)
{
    int pads = 0;
    DIRA |= 0x00ff0000;                        // set LEDs for output and pads for input
    DIRA &= 0xffffff00;
    while (1)
    {
        //pads = scan_pads(CLKFREQ / 100);    // scan the pads passing delay to wait for user touch
        pads = getButtons();
        pads = pads << 16;                    // shift the pad pins (0..7) to the LED pins (16..23)
        OUTA &= 0xff00ffff;                    // clear out the scanned pads from last time
        OUTA |= pads;                        // set the most recent scanned pads
    }
    return(0);
}
