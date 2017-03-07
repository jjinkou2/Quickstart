NAME    = pad
MODEL   = cmm
PROPGCC = /opt/parallax
SIMPLE  = /media/lili/Partage/Programmation/Propeller/SimpleIDE/Learn/Simple\ Libraries

OBJECTS=pad.c
CC = propeller-elf-gcc

# includes
INCLUDE = $(PROPGCC)/propeller-elf/include
TOOLINC = $(SIMPLE)/Utility/libsimpletools
TEXTINC = $(SIMPLE)/TextDevices/libsimpletext
I2CINC  = $(SIMPLE)/Protocol/libsimplei2c


# Libs
I2CLIB  = $(SIMPLE)/Protocol/libsimplei2c/$(MODEL)
LIB     = $(PROPGCC)propeller-elf/lib
TOOLLIB = $(SIMPLE)/Utility/libsimpletools/$(MODEL)
TEXTLIB = $(SIMPLE)/TextDevices/libsimpletext/$(MODEL)

# Loader
LOAD=propeller-load


CFLAGS  = -Wall -Os -m32bit-doubles -m$(MODEL) -fno-exceptions -std=c99 -I. -I$(INCLUDE) -L$(LIB) -I$(TOOLINC) \
	   -L$(TOOLLIB) -I$(TEXTINC) -L$(TEXTLIB)  -I$(I2CINC) -L$(I2CLIB) 
LDFLAGS = -lm -lsimpletools -lsimpletext -lsimplei2c


$(NAME): $(OBJECTS)
	mkdir -p $(MODEL)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(MODEL)/$@.elf $(LDFLAGS)

all:$(NAME)

.PHONY: all clean ram


# load to the board 
BOARD = quickstart
PORT  = /dev/ttyUSB0

ram: $(NAME)
	${LOAD} -b ${BOARD} -I $(PROPGCC)/propeller-load/ -Dreset=dtr -p ${PORT}  -r $(MODEL)/$^.elf

term: $(NAME)
	${LOAD} -b ${BOARD} -Dreset=dtr -p ${PORT} -t -r $(MODEL)/$^.elf

clean:
	rm -f $(MODEL)/*.elf


