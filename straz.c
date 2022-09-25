#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define abort(msg) do{printf(msg);exit(1);}while(0)

int size = 26;
int fd;
int ris = 0;

void* func(void* param) {

	int rd= 0;
	char* buffer = malloc(size*sizeof(char));
	int sommaA,sommaZ;

	while((rd=read(fd,buffer,size))>0) {
		sommaA = 0;
		sommaZ = 0;

		for(int i=0; i<rd;i++) {
			if(buffer[i] == 'a') sommaA++;
			if(buffer[i] == 'z') sommaZ++;
		}

		if(sommaA > sommaZ) __sync_fetch_and_add(&ris,1);
	}


	free(buffer);
}




int main(int argc, char *argv[]){

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

	printf("NUMERO RIGHE CON NUMERO DI A MAGGIORE DEL NUMERO DI Z: %d\n",ris);


	free(tids);
	close(fd);
	
	return 0;
}