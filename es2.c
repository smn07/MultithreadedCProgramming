#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define FILE_SIZE (32768*sizeof(short))
#define MEM_CORE (256*sizeof(short))
#define CORES 8

#define CUT 512  //max allocabile per ogni processo

volatile short min = 0x7fff , max = 0xffff;

int main(int argc, char const *argv[])
{
	pthread_t threads[CORES];

	int i;
	for(i=0; i<CORES; i++) {
		pthread_create(threads[i],calcolo,(void *) i);
	}
	
}