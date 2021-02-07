targets   = SendI2C ReceiveI2C I2CScan LEDgame LEDmsg LEDclock VPS Dial Play
objects   = LED.o LEDmsg.o LEDclock.o VPS.o
CC        = gcc
CFLAGS    = -O2 -Wall
LDFLAGS   = -noixemul

all : $(targets) ;
         
LED.o : LED.c LED.h

LEDmsg : LEDmsg.o LED.o
LEDmsg.o : LEDmsg.c LED.h
         
LEDclock : LEDclock.o LED.o
LEDclock.o : LEDclock.c LED.h

VPS : VPS.o LED.o
VPS.o : VPS.c LED.h
                        
clean :
	delete $(targets) $(objects)

install :
	copy $(targets) c:

