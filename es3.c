#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define abort(msg) do{printf(msg);exit(1);}while(0)

char* mem;
int posizione = 0;
pthread_mutex_t ptmutex;
int filesizeA;

void* invert(void* param) {

	int* appoggio = (int*)param;
	int size = appoggio[0];
	int i_th = appoggio[1];

	int count=1;
	
	int pos;

	/*ogni thread a turno salva l'attuale posizione
	  e la incrementa size volte. Successivamente esegue
	  size volte swap.*/

	pthread_mutex_lock(&ptmutex);
	pos = posizione;
	while(posizione < pos+size)
		posizione++;
	pthread_mutex_unlock(&ptmutex);

	char app;
	while(pos < (filesizeA/2) && count<=size){
		app = mem[pos];
		mem[pos] = mem[filesizeA-1-pos];
		mem[filesizeA-1-pos]=app;
		count++;
		pos++;
	}

	printf("sono il thread: %d\n",i_th);
}


int main(int argc, char const *argv[])
{
	int fdA = open(argv[1],O_RDONLY);
	if(fdA == -1) abort("errore apertura canale A\n");

	int fdB = open(argv[2],O_WRONLY | O_TRUNC | O_CREAT,0660);
	if(fdB == -1) abort("errore apertura canale B\n");

	filesizeA = lseek(fdA,0,SEEK_END);
	if(filesizeA == -1) abort("errore calcolo filesizeA\n");

	lseek(fdA,0,SEEK_SET);

	mem= malloc(filesizeA*sizeof(char));

	int r = read(fdA,mem,filesizeA);
	if(r != filesizeA) abort("errore lettura\n"); //file salvato in memoria

	
	int n = atoi(argv[3]);
	int size = filesizeA/n;
	int resto = filesizeA%n;
	int rest=size+resto;

	pthread_t thread[n];

	pthread_mutex_init(&ptmutex,NULL);

	int* appoggio;
	for (int i=0; i<n; i++) {
		
		appoggio = malloc(2*sizeof(int));

		if(i < (n-1)){
			appoggio[0] = size;
			appoggio[1] = i;
			pthread_create(thread+i,NULL,invert,appoggio);
		}
		else {
			appoggio[0] = rest;
			appoggio[1] = i;
			pthread_create(thread+i,NULL,invert,appoggio);
		}
	}

	for(int i=0; i<n; i++) {
		pthread_join(thread[i],NULL);
	}

	pthread_mutex_destroy(&ptmutex);

	int w = write(fdB,mem,filesizeA);
	if(w == -1) abort("errore scrittura\n");
	
	close(fdA);
	close(fdB);
	free(mem);
	free(appoggio);
	return 0;
}