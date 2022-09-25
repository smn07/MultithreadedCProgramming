#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define abort(msg) do{printf(msg);exit(1);}while(0)

int ris;
int fd;
int size=26;

void* func(void* p) {

	int rd;
	int letto = 0;
	int counter;
	char* buffer = malloc(size*sizeof(char));
	while((rd=read(fd,buffer,size*sizeof(char)))>0) {
		counter = 0;
		letto = rd; if(letto = 26) letto--;

		for(int i=0; i<(letto/sizeof(char))-1;i++) {
			if((buffer[i] == ' ' && buffer[i+1] != ' ' )|| (i==0 && buffer[i] != ' ')) counter++;
		}

		if(counter == 2) {
			__sync_fetch_and_add(&ris,1);
			printf("%s\n",buffer);
		}

	}

	free(buffer);
}

int main(int argc, char** argv) {

	int num_th = atoi(argv[2]);

	fd = open(argv[1],O_RDONLY);
	if(fd < 0) abort("errore apertura\n");

	pthread_t* tids = malloc(num_th*sizeof(pthread_t));

	int i;
	for(i=0;i<num_th;i++) {
		pthread_create(tids+i,NULL,func,NULL);
	}


	for(i=0;i<num_th;i++) {
		pthread_join(tids[i],NULL);
	}

	printf("il risultato e': %d\n",ris);

	free(tids);
	close(fd);
}