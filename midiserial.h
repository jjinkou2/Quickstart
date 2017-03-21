#include <propeller.h>

#define TERM_COG_LEN  7
#define SERIAL_MIN_PIN 0
#define SERIAL_MAX_PIN 31

typedef struct text_struct
{
  int  (*txChar)(struct text_struct *p, int ch); 
  int  cogid[TERM_COG_LEN];                      
  volatile void *devst;                          
} text_t;

typedef struct serial_info
{
   int rx_pin;
   int tx_pin;
   int mode;
   int baud;
   int ticks;
} Serial_t;

typedef text_t serial;
typedef text_t terminal;

int  serial_txChar(serial *device, int txbyte);
serial *serial_open(int rxpin, int txpin, int mode, int baudrate);
void serial_close(serial *device);