MCU=atmega8
CC=avr-gcc
OBJCOPY=avr-objcopy
# optimize for size:
CFLAGS=-g -mmcu=$(MCU) -Wall -Wstrict-prototypes -Os -mcall-prologues
PORT=/dev/ttyUSB0
PART=ATmega8
UISP = uisp -dprog=stk500 -dserial=/dev/ttyUSB0 -dpart=$(PART)

VERSION = 0.2.develop
DISTDIR = avr-midi.${VERSION}

#-------------------
current: basic.hex
#-------------------

BASICSRC = basic.c midi.c 

BASICOBJ = ${BASICSRC:.c=.o}
ECHOOBJ = ${ECHOSRC:.c=.o}

.c.o:
	@echo CC $<
	@$(CC) -c $(CFLAGS) -Os -o $*.o $<


basic.bin : basic.out
	$(OBJCOPY) -R .eeprom -O binary basic.out basic.bin 

basic.hex : basic.out 
	$(OBJCOPY) -R .eeprom -O ihex basic.out basic.hex 

basic.out : $(BASICOBJ)
	$(CC) $(CFLAGS) -o basic.out -Wl,-Map,basic.map $(BASICOBJ)


dist: clean
	mkdir -p ${DISTDIR}
	cp -R COPYING Makefile *.c *.h README ${DISTDIR}
	tar -czf ${DISTDIR}.tar.gz ${DISTDIR}
	rm -rf ${DISTDIR}

post: dist
	scp ${DISTDIR}.tar.gz x37v.info:x37v.info/projects/microcontroller/avr-midi/files/
	scp basic.c x37v.info:x37v.info/projects/microcontroller/avr-midi/

# load (program) the software
load_basic: basic.hex
	$(UISP) --erase
	$(UISP) --upload --verify if=basic.hex -v=3 --hash=32

fuse_mega16:
	$(UISP) --wr_fuse_l=0x9f --wr_fuse_h=0xd0

fuse_mega8:
	$(UISP) --wr_fuse_l=0xef --wr_fuse_h=0xc9

check_fuse:
	$(UISP) --rd_fuses

#-------------------
clean:
	rm -f *.o *.map *.out *.hex *.tar.gz
#-------------------
