MCU=atmega16
CC=avr-gcc
OBJCOPY=avr-objcopy
# optimize for size:
CFLAGS=-g -mmcu=$(MCU) -Wall -Wstrict-prototypes -Os -mcall-prologues
PORT=/dev/ttyUSB0
PART=ATmega16
UISP = uisp -dprog=stk500 -dserial=/dev/ttyUSB0 -dpart=$(PART)

VERSION = 0.1
DISTDIR = avr-midi.${VERSION}

#-------------------
current: basic.hex
#-------------------

BASICSRC = basic.c midi.c 
ECHOSRC = echo.c midi.c 

BASICOBJ = ${BASICSRC:.c=.o}
ECHOOBJ = ${ECHOSRC:.c=.o}

basic.bin : basic.out
	$(OBJCOPY) -R .eeprom -O binary basic.out basic.bin 

basic.hex : basic.out 
	$(OBJCOPY) -R .eeprom -O ihex basic.out basic.hex 

basic.out : $(BASICOBJ)
	$(CC) $(CFLAGS) -o basic.out -Wl,-Map,basic.map $(BASICOBJ)


echo.bin : echo.out
	$(OBJCOPY) -R .eeprom -O binary echo.out echo.bin 

echo.hex : echo.out 
	$(OBJCOPY) -R .eeprom -O ihex echo.out echo.hex 

echo.out : $(ECHOOBJ)
	$(CC) $(CFLAGS) -o echo.out -Wl,-Map,echo.map $(ECHOOBJ)


dist: clean
	mkdir -p ${DISTDIR}
	cp -R COPYING Makefile *.c *.h README ${DISTDIR}
	tar -czf ${DISTDIR}.tar.gz --exclude=".svn" ${DISTDIR}
	rm -rf ${DISTDIR}

post: dist
	scp ${DISTDIR}.tar.gz x37v.info:x37v.info/projects/microcontroller/avr-midi/files/
	scp basic.c x37v.info:x37v.info/projects/microcontroller/avr-midi/

.c.o:
	@echo CC $<
	@$(CC) -c $(CFLAGS) -Os -o $*.o $<

# load (program) the software
load_basic: basic.hex
	$(UISP) --erase
	$(UISP) --upload --verify if=basic.hex -v=3 --hash=32

load_echo: echo.hex
	$(UISP) --erase
	$(UISP) --upload --verify if=echo.hex -v=3 --hash=32

fuse_mega16:
	$(UISP) --wr_fuse_l=0x9f --wr_fuse_h=0xd0

check_fuse:
	$(UISP) --rd_fuses

#-------------------
clean:
	rm -f *.o *.map *.out *.hex *.tar.gz
#-------------------
