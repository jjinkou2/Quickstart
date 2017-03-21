/* 
 * QuickStart midi keyboard
 * 
 * How to use :
 * First select a bank sound with the pad
 * (pad 0 is the drums) wait until the red led is switched on
 * then play
 * 
 * How to install: 
 * http://forums.parallax.com/discussion/166294/quickstart-midiout-drums-pad
 * 
 */




//#include "simpletools.h"                      // Include simple tools
//#include "fdserial.h"
#include "midiserial.h"
//#include "serial.h"

// BANK
#define DRUMCHANNEL    9
#define CHANNEL0       0

// Notes For Drum
#define BASSDRUM  0x23
#define SNARE     0x26
#define CLAP      0x27
#define TOM1      0x29
#define CLOSEDHH  0x2A
#define TOM2      0x2D
#define OPENHH    0x2E
#define CYMBAL    0x34

// Notes
#define C4_Note   0x3C
#define D_Note    0x3E
#define E_Note    0x40
#define F_Note    0x41
#define G_Note    0x43
#define A_Note    0x45
#define B_Note    0x47
#define C5_Note   0x48


void    get_index_pad(int *pad_set, int pad_state);
int     get_bit_value(int value, int index);
void    send_midi(int touch, int previous_touch);
void    msleep(int ms);          // sleep for ms
int     getButtons();           // used to get button states
void    changePrg(serial *midiout, char PatchNb);

void    noteOn(serial *midiout, int data1, int data2);


serial *midiout;
int      note;
int      CHANNEL = DRUMCHANNEL;

int drums_notes[8]     = {CYMBAL,OPENHH,TOM2,TOM1,
                          CLOSEDHH,CLAP,SNARE,BASSDRUM};
int chromatic_scale[8] = {C4_Note,D_Note,E_Note,F_Note,
                          G_Note,A_Note,B_Note,C5_Note};
                        
int main()                                    
{
  int pad_index[8];          // array of touched pad 
  DIRA |= 0x00ff0000;           // set LEDs for output

  /* start device */
  midiout = serial_open(31,30,0,115200);


  int programSelect=0;
  
  // First loop: 
  // Selection of a Patch (bank of sound)
  while(!programSelect) {
    programSelect = getButtons();
    if (programSelect == 1) { // right button => play drums at channel 10
      CHANNEL = DRUMCHANNEL;
    }
    else {
      CHANNEL = CHANNEL0;
      changePrg(midiout, programSelect);
    }      
    msleep(1000);
  }    
  
  int previous_touch ;
  int next_touch ;  // return from getbuttons
  int changed_touch;  // detects if next touch is <> previous  
  // Main Loop to play the music
  while (1){
    previous_touch = next_touch;   // Save State
    next_touch = getButtons();
    changed_touch = next_touch ^ previous_touch;
    
    if (changed_touch)  send_midi(next_touch,changed_touch); 
   
  }    
}


void send_midi(int touch, int changed_touch){
  int pad_index[8];          // array of touched pad
  
    get_index_pad(pad_index, changed_touch); // identify which note has to be played on /off
    for (int i=0;i<=7;i++){
      if (pad_index[i]==1) {  // Only send a midi message for note which states has changed
        int velocity = get_bit_value(touch,i) *100;  // swith on or off
        if (CHANNEL == DRUMCHANNEL)   
          noteOn(midiout, drums_notes[i], velocity);
        else 
          noteOn(midiout, chromatic_scale[7-i], velocity);
      } 
    }
}

int get_bit_value(int value, int index){
  int mask = 1;
  return value >> index & mask; // shift value to the right for n=index 
                                // mask with 1 to get bit value
}  

void get_index_pad(int *pad_index, int pad_state){
  for (int i=0;i<=7;i++){
      pad_index[i]=get_bit_value(pad_state,i) ? 1 : 0;
  }      
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

void changePrg(serial *midiout, char PatchNb){
  serial_txChar(midiout, 0xC0+CHANNEL);
  serial_txChar(midiout, PatchNb);
}

void noteOn(serial *midiout, int data1, int data2){
  serial_txChar(midiout, 0x90+CHANNEL);
  serial_txChar(midiout, data1);
  serial_txChar(midiout, data2);
}  

