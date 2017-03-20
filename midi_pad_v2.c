#include "simpletools.h"                      // Include simple tools
#include "fdserial.h"


#define BASSDRUM  0x23
#define SNARE     0x26
#define CLAP      0x27
#define TOM1      0x29
#define CLOSEDHH  0x2A
#define TOM2      0x2D
#define OPENHH    0x2E
#define CYMBAL    0x34

void    get_index_pad(int *pad_set, int pad_state);
int     get_bit_value(int value, int index);
void    send_midi(int touch, int previous_touch);
void    msleep(int ms);          // sleep for ms
int     getButtons();           // used to get button states

#ifdef __DEBUG
void debug_touch(int touch, int previous_touch);
#else 
void    noteOn(fdserial *midiout, int cmd,int data1, int data2);  // send note to midiout
#endif

#ifdef __DEBUG
char *drums_names[8] = {"CYMBAL","OPENHH","TOM2","TOM1",
                        "CLOSEDHH","CLAP","SNARE","BASSDRUM"};
#else 
fdserial *midiout;
int note;
int CHANNEL = 9;
#endif
int drums_notes[8]   = {CYMBAL,OPENHH,TOM2,TOM1,
                        CLOSEDHH,CLAP,SNARE,BASSDRUM};
                        
int main()                                    
{
  int pad_index[8];          // array of touched pad 
  int first_touch = 0xA;    // return from getbuttons
  DIRA |= 0x00ff0000;           // set LEDs for output

  /* start device */
#ifndef __DEBUG
  midiout = fdserial_open(31,30,0,115200);
#endif

  get_index_pad( pad_index,  first_touch); 

#ifdef __DEBUG
  for (int i=0;i<=7;i++){
    if (pad_index[i]==1) print ("index of touched pads: %d\n",i);
  }    
  
  // init
  for (int i=0;i<=7;i++){
    if (pad_index[i]==1) print ("Play note: %x name: %s\n",drums_notes[i],drums_names[i]);
  } 
#endif

  // next touch
  int previous_touch = first_touch;
  int next_touch  = 0xC;  // return from getbuttons
#ifdef __DEBUG
  debug_touch(next_touch, previous_touch);
#endif

  send_midi(next_touch,previous_touch);
  
  // next touch
  previous_touch = next_touch;
  next_touch  = 0xA4;  // return from getbuttons
#ifdef __DEBUG
  debug_touch(next_touch, previous_touch);
#endif
  send_midi(next_touch,previous_touch); 
  
  while (1){
    previous_touch = next_touch;
    next_touch = getButtons();
    int changed_touch = next_touch ^ previous_touch;
    if (changed_touch) {
#ifdef __DEBUG
      print ("\nButton: %08b",next_touch);
#endif
      send_midi(next_touch,previous_touch); 
    }      
  }    
}


void send_midi(int touch, int previous_touch){
  int pad_index[8];          // array of touched pad
  
  get_index_pad(pad_index, touch); 
  int changed_touch = touch ^ previous_touch;
  get_index_pad(pad_index, changed_touch); 
  for (int i=0;i<=7;i++){
    if (pad_index[i]==1) {
      int velocity = get_bit_value(touch,i) *120;

#ifdef __DEBUG
      print ("\nSend velocity %03d for drum: %x name: %s",velocity,drums_notes[i],drums_names[i]);     
    }
  }  
    print("\n");      
#else
      noteOn(midiout, 0x90+CHANNEL, drums_notes[i], velocity);
    }
  }    
#endif

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




#ifdef __DEBUG
void debug_touch(int touch, int previous_touch){
  int pad_index[8];          // array of touched pad
  int changed_touch = touch ^ previous_touch;
  print ("\nfirst_touch:\t%08b\n", previous_touch);
  print ("next_touch:\t%08b\n", touch);
  print ("------------------------\n");
  print ("changed:\t%08b\n\n", changed_touch);    
  for (int i=0;i<=7;i++){
    if (pad_index[i]==1) print ("Play note: %x name: %s\n",drums_notes[i],drums_names[i]);
  }  
}  

#else

void noteOn(fdserial *midiout, int cmd,int data1, int data2){
  writeChar(midiout, cmd);
  writeChar(midiout, data1);
  writeChar(midiout, data2);
}  

#endif