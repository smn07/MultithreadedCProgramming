#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define abort(msg) do{printf(msg);exit(1);}while(0)

int fd;
int somma = 0;
int numeripari = 0;
int chunk = 10;
//pthread_mutex_t ptmutex;


void* func(void* param) {

	int* buffer = malloc(chunk*sizeof(int));
	int sommalocale=0;
	int parilocale=0;
	int rd;
	int i_th = *(int*)param;
	
	while((rd = read(fd,buffer,chunk*sizeof(int)))>0) {
		sommalocale=0;
		parilocale=0;

		for(int i=0;i<rd/sizeof(int);i++) {
			if(buffer[i]%2 == 0) {
				sommalocale += buffer[i];
				parilocale++;
			}
		}

		__sync_fetch_and_add(&somma,sommalocale);
		__sync_fetch_and_add(&numeripari,parilocale);

		/*pthread_mutex_lock(&ptmutex);
		somma+=sommalocale;
		numeripari+=parilocale;
		pthread_mutex_unlock(&ptmutex);*/

		printf("SONO IL THREAD: %d\n",i_th);
	}

	free(buffer);
}

int avg(int* appoggio,int l) {
	int somma=0;
	int conta=0;
	for(int i=0;i<l;i++) {
		if(appoggio[i]%2==0) {
			somma += appoggio[i];
			conta++;
		}
	}

	return somma/conta;
}


int main(int argc, char** argv) {

	int lettura = atoi(argv[1]);
	int num_th = atoi(argv[2]);

	int fdu = open("/dev/urandom",O_RDONLY);
	if(fdu < 0) abort("errore apertura\n");

	int* appoggio = malloc(lettura*sizeof(int));

	int rdu = read(fdu,appoggio,lettura*sizeof(int));
	if(rdu < 0) abort("errore lettura da dev urandom\n");

	close(fdu);

	for(int j=0;j<rdu/sizeof(int);j++) {
		printf("%d  ",appoggio[j]);
	}

	printf("\n");

	printf("LA MEDIA DOVREBBE ESSERE: %d\n",avg(appoggio,rdu/sizeof(int)));

	int fdw = open("fileMedia.txt",O_CREAT | O_TRUNC | O_WRONLY, 0660);
	if(fdw <0) abort("errore creazione file\n");

	int wr = write(fdw,appoggio,lettura*sizeof(int));
	if(wr < 0) abort("errore scrittura\n");

	close(fdw);

	fd = open("fileMedia.txt",O_RDONLY);
	if(fd < 0) abort("errore lettura\n");
	//pthread_mutex_init(&ptmutex,NULL);

	pthread_t* tids = malloc(num_th*sizeof(pthread_t));
	int i;
	int*indici = malloc(num_th*sizeof(int));

	for(i=0;i<num_th;i++) {
		indici[i]=i;
		pthread_create(tids+i,NULL,func,indici+i);
	}


	for(i=0;i<num_th;i++) {
		pthread_join(tids[i],NULL);
	}
	//pthread_mutex_destroy(&ptmutex);

	printf("La media dei numeri pari nel file e': %d\n",somma/numeripari );

	free(tids);
	free(indici);
	free(appoggio);
	close(fd);
	return 0;
}

