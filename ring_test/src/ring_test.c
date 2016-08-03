/*
 ============================================================================
 Name        : ring_test.c
 Author      : Jens Fey
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "ring_buffer.h"
#include <pthread.h>

ring_buffer ring;
int er = 0;
int rc;

char* buff;

/*helper function to sleep x milliseconds*/
void sleep(int ms){
	struct timespec ts;

	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000000;

	nanosleep(&ts, NULL);
}

void thread_function(void *threadid){
	int i;

	char* t_buff = malloc(sizeof(char)*9);

	printf("Thread started %i\n", (int)threadid);

	for(i=0; i<10;i++){
		printf("Thread %i doing stuff:\n", (int)threadid);

		/*Push in*/
		if(er = ring_push_in(&ring, "BlubBlub", 8)){
			printf("Error code: %i! Buffer to small!\n" , er);
		}

		/*Pop out*/
		if(er = ring_pop_out(&ring, t_buff, 8)){
			printf("Error code: %i!\n", er);
		}

		printf("Thread %i: ring popped out! ring fill now: %i \n", (int)threadid, ring_get_fill_level(&ring));

		/*Print out buff*/
		t_buff[8] = '\0';
		printf("Thread %i: buffer: %s\n", (int)threadid, t_buff);

		printf("Thread %i finished stuff:\n", (int)threadid);
		sleep(1);
	}
	printf("Thread %i ends:\n", (int)threadid);
}

int main(void) {
	puts("!!!Ring Buffer tester!!!"); /* prints !!!Hello World!!! */

	buff = malloc(sizeof(char)*17);

	ring_init(&ring, 50);
	ring_push_in(&ring, "testtest", 8);

	pthread_t test_thread1, test_thread2;

	puts("after First push in!");
	printf("ring fill: %i \n", ring_get_fill_level(&ring));

	if(er = ring_push_in(&ring, "BlubBlub", 8)){
		printf("Error code: %i! Buffer to small!\n" , er);
	}

	puts("after next push in!");
	printf("ring fill: %i \n", ring_get_fill_level(&ring));

	if(er = ring_pop_out(&ring, buff, 8)){
		printf("Error code: %i!\n", er);
	}

	printf("ring popped out! ring fill now: %i \n", ring_get_fill_level(&ring));
	buff[8] = '\0';

	printf("buffer: %s\n", buff);

	if(er = ring_push_in(&ring, "loollool", 8)){
		printf("Error code: %i! Buffer to small!\n", er);
	}

	puts("after next push in!");
	printf("ring fill: %i \n", ring_get_fill_level(&ring));

	ring_pop_out(&ring, buff, 16);

	printf("ring popped out! ring fill now: %i \n", ring_get_fill_level(&ring));
	buff[16] = '\0';

	printf("buffer: %s\n", buff);

	if(er = ring_pop_out(&ring, buff, 8)){
			printf("Error code: %i!\n", er);
	}


	/*----- multi thread test -----*/

	printf("Now starting thread save test!!!\n\n");


	/*Create the thread that starts sending packets*/
	rc = pthread_create(&test_thread1, NULL, thread_function, (void *)1);
	if (rc){
		printf("error while creating thread\n");
		exit(-1);
	}

	/*Create the thread that starts sending packets*/
	rc = pthread_create(&test_thread2, NULL, thread_function, (void *)2);
	if (rc){
		printf("error while creating thread\n");
		exit(-1);
	}

	pthread_exit(NULL);

	printf("End of Line!\n");
	return EXIT_SUCCESS;
}
