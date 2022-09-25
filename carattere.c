#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define abort(msg) do{printf(msg);exit(1);}while(0)

char carattere = 'c';
int fd;
int chunk = 5;
int risultato = 0;

void* func(void* func) {


	char* buffer = malloc(chunk*sizeof(char));
	int locale;
	int rd;
	while((rd=(read(fd,buffer,chunk*sizeof(char))))>0) {
		locale = 0;
		for(int i=0;i<rd/sizeof(char);i++) {
			if(buffer[i] == carattere) locale++;
		}

		__sync_fetch_and_add(&risultato,locale);
	}

	free(buffer);
}


int main(int argc, char const *argv[])
{
	int num_th = atoi(argv[2]);
	
	fd = open(argv[1],O_RDONLY);
	if(fd < 0) abort("errore apertura\n");

	pthread_t* tids = malloc(num_th*sizeof(pthread_t));

	int i;
	for(i=0; i<num_th; i++) {
		pthread_create(tids+i,NULL,func,NULL);
	}

	for(i=0; i<num_th; i++) {
		pthread_join(tids[i],NULL);
	}

	printf("RISULTATO = %d\n",risultato);

	free(tids);
	close(fd);

	return 0;
}