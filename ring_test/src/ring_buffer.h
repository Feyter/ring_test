/*
 * ring_buffer.h
 *
 *  Created on: 02.08.2016
 *      Author: mintdriver
 */

#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

#include "spinlock.h"

typedef struct{
	char* buffer;	/*the pointer to the start of the ring buffer*/
	int length;		/*the length of the whole ring buffer*/
	char* in;		/*the pointer to the current position to push in*/
	char* out;		/*the pointer to the current position to pop out*/
	spinlock_t lock;	/*the lock for raceconditions*/
}ring_buffer;


int ring_init(ring_buffer* this, int buffer_size){
	this->buffer = malloc(sizeof(char)*buffer_size); /*initialize the Buffer*/
	this->length = buffer_size; /*save the length*/
	this->in = this->buffer;	/*set in pointer to start of buffer*/
	this->out = this->buffer;	/*set out pointer to start of buffer*/
	spin_lock_init(&this->lock);
}

/*Copy byte for byte from in_buffer in this buffer.
 * first check the next field in every step and then copy and bend the in pointer to the right address.
 * If end of buffer is reached pointer will be bend to the beginning of buffer,
 * WARNING: no overwriting of non out popped data!
 * Negative return value indicates errors*/
int ring_push_in(ring_buffer* this, char* in_buffer,int in_length){
	int i;
	char* next_in;

	spin_lock(&this->lock);
	for(i=0; i<in_length;i++){
		/*find out which field would be next in*/
		if(this->in+1 >= this->buffer+this->length){
			next_in = this->buffer;
		}
		else{
			next_in = this->in+1;
		}
		/*Only do something if next is not the out pointer*/
		if(next_in != this->out){
			/*copy one byte in*/
			memcpy(this->in, in_buffer+i, 1);
			/*bend the in pointer to the next position*/
			this->in = next_in;
		}
		else{
			spin_unlock(&this->lock);
			return -1; /*Error: only coped until out. Buffer too small*/
		}
	}
	spin_unlock(&this->lock);
	return 0; /*All data copied in*/
}

/*Copy byte for byte from the ring buffer into the given out_buffer for the length of out_length.
 * Bends the out pointer forward until it is at the in pointer.
 * WARNING: Data will be copied in the out buffer as long as the ring has new buffer to offer or the requested length is reached.
 * Negative return value indicates errors*/
int ring_pop_out(ring_buffer* this, char* out_buffer,int out_length){
	int i;

	spin_lock(&this->lock);
	/*go through byte for byte*/
	for(i=0; i<out_length;i++){
		/*check if the out pointer is at the in pointer*/
		if(this->out != this->in){
			memcpy(out_buffer+i, this->out, 1);
			/*bend out pointer to the next position*/
			if(this->out+1 >= this->buffer+this->length){
				this->out = this->buffer;/*end of buffer reached go to beginning*/
			}
			else{
				this->out++;
			}
		}
		else{
			spin_unlock(&this->lock);
			return -1; /*more data requested than in buffer*/
		}
	}
	spin_unlock(&this->lock);
	return 0; /*All data copied out*/

}

/*Returns the number of bytes waiting to popped out the ring buffer*/
int ring_get_fill_level(ring_buffer* this){
	int count =0;
	char* pos = this->out;

	spin_lock(&this->lock);
	/*runs until pos is at in pointer*/
	while(pos != this->in){
		count ++;
		if(pos+1 >= this->buffer+this->length){
			pos = this->buffer; /*end of buffer reached bend to the beginning*/
		}
		else{
			pos++; /*pos is the next field*/
		}
	}

	spin_unlock(&this->lock);
	return count;
}

#endif /* RING_BUFFER_H_ */
