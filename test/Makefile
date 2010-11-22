CFLAGS += -I. -I../ -g -Wall -DDEBUG 
DUMMY_SRC = dummy_device.c ../midi.c ../midi_device.c ../bytequeue/bytequeue.c ../bytequeue/interrupt_setting.c
DUMMY_OBJ = ${DUMMY_SRC:.c=.o}

SYSEX_SRC = sysex_test.c ../sysex_tools.c
SYSEX_OBJ = ${SYSEX_SRC:.c=.o}

.c.o:
	@echo CC $<
	@$(CC) -c $(CFLAGS) -o $*.o $<

dummy_test: clean $(DUMMY_OBJ)
	@$(CC) -o dummy_test $(DUMMY_OBJ)

sysex_test: clean $(SYSEX_OBJ)
	@$(CC) -o sysex_test $(SYSEX_OBJ)

run_tests: sysex_test dummy_test
	./dummy_test
	./sysex_test

all: run_tests

#-------------------
clean:
	rm -f *.o *.map *.out *.hex *.tar.gz ../*.o ../bytequeue/*.o dummy_test sysex_test
#-------------------

