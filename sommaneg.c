#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define abort(msg) do{printf(msg);exit(1);}while(0)

int fd1;
int fd2;
int size = 5;
int somma1;
int somma2;

void* func(void* param) {
	int fd = *(int*)param;

	int* buffer = malloc(size*sizeof(int));

	int sommalocale = 0;
	int rd = 0;

	while((rd=read(fd,buffer,size*sizeof(int)))>0) {
		sommalocale = 0;

		for(int i=0;i<rd/sizeof(int);i++) {
			if (buffer[i]<0) sommalocale += buffer[i];
		}

		if(fd==fd1) __sync_fetch_and_add(&somma1,sommalocale);
		else if(fd==fd2) __sync_fetch_and_add(&somma2,sommalocale);
	}

	free(buffer);

}


int main(int argc, char *argv[]) {
	
	int lettura = atoi(argv[1]);
	int num_th = atoi(argv[2]);

	int fdu = open("/dev/urandom",O_RDONLY);
	if(fdu < 0) abort("errore apertura urandom\n");

	int* appoggio = malloc(lettura*sizeof(int));

	int rd = read(fdu,appoggio,lettura*sizeof(int));
	if(rd < 0) abort("errore lettura urandom1");

	fd1 = open("file1",O_WRONLY|O_TRUNC|O_CREAT,0660);
	if(fd1 < 0) abort("errore apertura primo file di scrittura\n");

	int wr = write(fd1,appoggio,lettura*sizeof(int));
	if(wr < 0) abort("errore scrittura primo file\n");

	int somma =0;

	for(int i=0;i<lettura;i++) {
		if(appoggio[i]<0) somma+=appoggio[i];
	}

	printf("\nLA SOMMA1 E': %d\n",somma);


	close(fd1);

	rd = read(fdu,appoggio,lettura*sizeof(int));
	if(rd < 0) abort("errore lettura urandom2");

	fd2 = open("file2",O_WRONLY|O_TRUNC|O_CREAT,0660);
	if(fd2 < 0) abort("errore apertura primo file di scrittura\n");

	wr = write(fd2,appoggio,lettura*sizeof(int));
	if(wr < 0) abort("errore scrittura primo file\n");

	somma =0;

	for(int i=0;i<lettura;i++) {
		if(appoggio[i]<0) somma+=appoggio[i];
	}

	printf("\nLA SOMMA2 E': %d\n",somma);

	close(fdu);
	close(fd2);

	fd1 = open("file1",O_RDONLY);
	if(fd1 < 0) abort("errore apertura primo file\n");

	fd2 = open("file2",O_RDONLY);
	if(fd2 < 0) abort("errore apertura secondo file\n");

	pthread_t* tids = malloc(num_th*sizeof(pthread_t));

	int i;
	for(i=0;i<num_th;i++) {

		if(i<num_th/2) {
			pthread_create(tids+i,NULL,func,&fd1);
		}
		else pthread_create(tids+i,NULL,func,&fd2);
	}


	for(i=0;i<num_th;i++) {
		pthread_join(tids[i],NULL);
	}

	int sommamax;
	if(somma1 > 0 && somma2<0) sommamax = somma2;
	if(somma2>0 && somma1<0) sommamax = somma1;
	if(somma1<0 && somma2<0) if(somma1>somma2) sommamax = somma1;
							 else sommamax = somma2;

	printf("la somma maggiore tra le due e': %d\n",sommamax);



	free(tids);
	close(fd1);
	close(fd2);
	free(appoggio);

	return 0;
}