MCU=atmega16
CC=avr-gcc
OBJCOPY=avr-objcopy
# optimize for size:
CFLAGS=-g -mmcu=$(MCU) -Wall -Wstrict-prototypes -Os -mcall-prologues
PORT=/dev/ttyUSB0
PART=ATmega16
UISP = uisp -dprog=stk500 -dserial=/dev/ttyUSB0 -dpart=$(PART)

#-------------------
current: main.hex
#-------------------

SRC = main.c \
		midi.c 

OBJ = ${SRC:.c=.o}

main.bin : main.out
	$(OBJCOPY) -R .eeprom -O binary main.out main.bin 

main.hex : main.out 
	$(OBJCOPY) -R .eeprom -O ihex main.out main.hex 

main.out : $(OBJ)
	$(CC) $(CFLAGS) -o main.out -Wl,-Map,main.map $(OBJ)

.c.o:
	@echo CC $<
	@$(CC) -c $(CFLAGS) -Os -o $*.o $<

# you need to erase first before loading the program.
# load (program) the software into the eeprom:
load: main.hex
	$(UISP) --erase
	$(UISP) --upload --verify if=main.hex -v=3 --hash=32

fuse_mega16:
	$(UISP) --wr_fuse_l=0x9f --wr_fuse_h=0xd0

check_fuse:
	$(UISP) --rd_fuses

#-------------------
clean:
	rm -f *.o *.map *.out *.hex
#-------------------
