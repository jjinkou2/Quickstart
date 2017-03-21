/* 
 * just a copy paste of tx functions from serial 
 *
 */
#include "propeller.h"
#include "midiserial.h"
#include <stdlib.h>


void serial_close(serial *device)
{
  if(!device)
    return;
  Serial_t *sp = (Serial_t*) device->devst;
  OUTA &= ~(1 << sp->tx_pin);
  DIRA &= ~(1 << sp->tx_pin);
  DIRA &= ~(1 << sp->rx_pin);
  free((void*)device->devst);
  free(device);
  device = 0;
}

__attribute__((fcache)) static void _outbyte(int bitcycles, int txmask, int value)
{
  int j = 10;
  int waitcycles;

  waitcycles = CNT + bitcycles;
  while(j-- > 0) {
    /* C code is too big and not fast enough for all memory models.
    // waitcycles = waitcnt2(waitcycles, bitcycles); */
    __asm__ volatile("waitcnt %[_waitcycles], %[_bitcycles]"
                     : [_waitcycles] "+r" (waitcycles)
                     : [_bitcycles] "r" (bitcycles));

    /* if (value & 1) OUTA |= txmask else OUTA &= ~txmask; value = value >> 1; */
    __asm__ volatile("shr %[_value],#1 wc \n\t"
                     "muxc outa, %[_mask]"
                     : [_value] "+r" (value)
                     : [_mask] "r" (txmask));
  }
}

serial *serial_open(int rxpin, int txpin, int mode, int baudrate)
{
  Serial_t *serptr;

  /* can't use array instead of malloc because it would go out of scope. */
  text_t* text = (text_t*) malloc(sizeof(text_t));

  /* set pins first for boards that can misbehave intentionally like the Quickstart */
  if(txpin >= SERIAL_MIN_PIN && txpin <= SERIAL_MAX_PIN) {
    OUTA |=  (1<<txpin);
    DIRA |=  (1<<txpin);
  }
  if(rxpin >= SERIAL_MIN_PIN && rxpin <= SERIAL_MAX_PIN) {
    DIRA &= ~(1<<rxpin);
  }

  //memset(text, 0, sizeof(text_t));
  serptr = (Serial_t*) malloc(sizeof(Serial_t));
  text->devst = serptr;
  memset((char*)serptr, 0, sizeof(Serial_t));
  
  text->txChar    = serial_txChar;     /* required for terminal to work */

  serptr->rx_pin  = rxpin; /* recieve pin */
  serptr->tx_pin  = txpin; /* transmit pin*/
  serptr->mode    = mode;
  serptr->baud    = baudrate;
  serptr->ticks   = CLKFREQ/baudrate; /* baud from clkfreq (cpu clock typically 80000000 for 5M*pll16x) */
     
  return text;
}

int serial_txChar(serial *device, int value)
{
  Serial_t *sp = (Serial_t*) device->devst;
  int txmask = (1 << sp->tx_pin);

  if(sp->tx_pin < SERIAL_MIN_PIN && sp->tx_pin > SERIAL_MAX_PIN)
    return 0; /* don't transmit on pins out of range */

  DIRA |= txmask;

  _outbyte(sp->ticks, txmask, (value | 0x100) << 1);

  return value;
}