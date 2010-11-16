CFLAGS += -I. -I../ -g -Wall -DDEBUG 
SRC = dummy_device.c ../midi.c ../midi_device.c ../bytequeue/bytequeue.c
OBJ = ${SRC:.c=.o}

.c.o:
	@echo CC $<
	@$(CC) -c $(CFLAGS) -o $*.o $<

test: clean $(OBJ)
	@$(CC) -o test $(OBJ)

#-------------------
clean:
	rm -f *.o *.map *.out *.hex *.tar.gz ../*.o ../bytequeue/*.o test
#-------------------

