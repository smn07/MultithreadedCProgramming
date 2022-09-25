#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define abort(msg) do{printf(msg);exit(1);}while(0)

int chunk = 10;
int fd;
int ris = -1073741824;
pthread_mutex_t mutex;

void* func(void* p) {
	int rd;
	int* buffer = malloc(chunk*sizeof(int));
	int maxlocale;

	while((rd=read(fd,buffer,chunk*sizeof(int)))>0) {
		maxlocale = -1;
		for(int i=0;i<rd/sizeof(int);i++) {
			if(buffer[i] % 3 == 0) maxlocale = buffer[i];
		}

		if(maxlocale != -1) {
			for(int i=0;i<rd/sizeof(int);i++) {
				if(buffer[i] % 3 == 0 && buffer[i] > maxlocale) maxlocale = buffer[i];
			}

			pthread_mutex_lock(&mutex);
			if(maxlocale>ris) ris=maxlocale;
			pthread_mutex_unlock(&mutex);
		}
	}

	free(buffer);
}


int main(int argc, char const *argv[]){
	
	int num_th = atoi(argv[2]);
	int lettura = atoi(argv[1]);

	int fdu = open("/dev/urandom",O_RDONLY);
	if(fdu < 0) abort("errore apertura urandom\n");

	int* buffer = malloc(lettura*sizeof(int));

	int rdu = read(fdu,buffer,lettura*sizeof(int));
	if(rdu < 0) abort("errore lettura urandom\n");

	close(fdu);

	int fdw = open("filemaxdisp.bin",O_CREAT|O_TRUNC|O_WRONLY,0660);
	if(fdw < 0) abort("errore apertura per scrittura\n");

	int wr = write(fdw,buffer,lettura*sizeof(int));
	if(wr < 0) abort("errore scrittura\n");

	close(fdw);

	fd = open("filemaxdisp.bin",O_RDONLY);
	if(fd < 0) abort("errore apertura\n");

	int filesize = lseek(fd,0,SEEK_END);
	lseek(fd,0,SEEK_SET);

	rdu = read(fd, buffer, filesize);


	int max;
	for(int j=1;j<rdu/sizeof(int);j++) {
		if(buffer[j] % 3 == 0) max=buffer[j];
		break;
	}

	for(int j=0;j<rdu/sizeof(int);j++) {
		if(buffer[j] % 3 == 0 && buffer[j]>max) max=buffer[j];
	}


	printf("\nIL MASSIMO DISPARI DOVREBBE ESSERE: %d\n",max);



	lseek(fd,0,SEEK_SET);


	pthread_mutex_init(&mutex,NULL);

	pthread_t* tids = malloc(num_th*sizeof(pthread_t));

	int i;
	for(i=0;i<num_th;i++) {
		pthread_create(tids+i,NULL,func,NULL);
	}

	for(i=0;i<num_th;i++) {
		pthread_join(tids[i],NULL);
	}

	pthread_mutex_destroy(&mutex);

	printf("IL MASSIMO DISPARI E': %d\n",ris);

	free(tids);
	free(buffer);
	close(fd);
	return 0;
}