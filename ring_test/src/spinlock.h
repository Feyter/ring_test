/*
 * spinlock.h
 *
 *  Created on: 23.07.2016
 *      Author: mintdriver
 */

#ifndef SPINLOCK_H_
#define SPINLOCK_H_

#include <time.h>

/*Fake spinlock for the simulation*/
typedef struct {
	int lock;
} spinlock_t;

void spin_lock_init(spinlock_t *lock){
	lock->lock = 0;
}

void spin_lock(spinlock_t *lock){
	struct timespec ts;
	int ms = 10;

	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000000;
	nanosleep(&ts, NULL);
	while(lock->lock!=0){
		printf("Wating for spinlock!!!\n");
		nanosleep(&ts, NULL);
	}
	lock->lock = 1;
}

void spin_unlock(spinlock_t *lock){
	lock->lock = 0;
}

#endif /* SPINLOCK_H_ */
