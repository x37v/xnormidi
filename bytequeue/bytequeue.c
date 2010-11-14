//this is a single reader [maybe multiple writer?] byte queue
//Copyright 2008 Alex Norman
//writen by Alex Norman 
//
//This file is part of avr-bytequeue.
//
//avr-bytequeue is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//avr-bytequeue is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with avr-bytequeue.  If not, see <http://www.gnu.org/licenses/>.

#include "bytequeue.h"
#include <avr/interrupt.h>

void bytequeue_init(byteQueue_t * queue, uint8_t * dataArray, byteQueueIndex_t arrayLen){
	queue->length = arrayLen;
	queue->data = dataArray;
	queue->start = queue->end = 0;
}

bool bytequeue_enqueue(byteQueue_t * queue, uint8_t item){
	uint8_t sreg = SREG;
	cli();
	//full
	if(((queue->end + 1) % queue->length) == queue->start){
		SREG = sreg;
		return false;
	} else {
		queue->data[queue->end] = item;
		queue->end = (queue->end + 1) % queue->length;
		SREG = sreg;
		return true;
	}
}

byteQueueIndex_t bytequeue_length(byteQueue_t * queue){
	byteQueueIndex_t len;
	uint8_t sreg = SREG;
	cli();
	if(queue->end >= queue->start)
		len = queue->end - queue->start;
	else
		len = (queue->length - queue->start) + queue->end;
	SREG = sreg;
	return len;
}

//we don't need to avoid interrupts if there is only one reader
uint8_t bytequeue_get(byteQueue_t * queue, byteQueueIndex_t index){
	return queue->data[(queue->start + index) % queue->length];
}

//we just update the start index to remove elements
void bytequeue_remove(byteQueue_t * queue, byteQueueIndex_t numToRemove){
	uint8_t sreg = SREG;
	cli();
	queue->start = (queue->start + numToRemove) % queue->length;
	SREG = sreg;
}

