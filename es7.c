#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define abort(msg) do{printf(msg);exit(1);}while(0)
#define NUM_THR 4
char* buffer;
int risultato = -1073741824;
int posizione = 0;
//pthread_mutex_t mutex;

void* func(void* param) {
	int size = *(int*) param;
	if(size < 2) abort("DEVO AVERE ALMENO DUE NUMERI PER CALCOLARE LA DIVISIONE.\n");
	int pos;
	int max_locale;
	int oldmax;

	pos = __sync_fetch_and_add(&posizione,size);

	max_locale = risultato;

	for(int i=pos; i<(pos+size-1); i++) {
		if(buffer[i]/buffer[i+1] >= max_locale) max_locale = buffer[i]/buffer[i+1];
		i++;
	}

	do {oldmax = risultato;}
	while(oldmax < max_locale && !(__sync_bool_compare_and_swap(&risultato,oldmax,max_locale)));

	/*pthread_mutex_lock(&mutex);
	if(max_locale>risultato) risultato = max_locale;
	pthread_mutex_unlock(&mutex);*/

}


int main(int argc, char** argv) {

	int fd = open("/dev/urandom",O_RDONLY);
	if(fd < 0) abort("errore apertura\n");

	int numero = atoi(argv[1]);
	buffer = malloc(numero*sizeof(int));

	int rd = read(fd,buffer,numero*sizeof(int));
	if(rd < 0) abort("errore lettura\n");

	for(int j=0;j<numero;j++) {
		printf("%d ",buffer[j]);
	}

	printf("\nDIVISIONI: \n");

	for(int j=0;j<numero-1;j++) {
		printf("%d  ",buffer[j]/buffer[j+1]);
		j++;
	}


	pthread_t* tids = malloc(NUM_THR*sizeof(pthread_t));
	//pthread_mutex_init(&mutex,NULL);

	int i;
	int size = numero/NUM_THR;
	int resto = numero%NUM_THR;
	int rest = size+resto;

	for(i=0;i<NUM_THR;i++) {
		if(i<NUM_THR-1) pthread_create(tids+i,NULL,func,&size);
		else pthread_create(tids+i,NULL,func,&rest);
	}

	for(i=0;i<NUM_THR;i++) {
		pthread_join(tids[i],NULL);
	}

	//pthread_mutex_destroy(&mutex);

	printf("VALORE DIVISIONE MAX: %d\n",risultato);
	free(tids);
	free(buffer);
	close(fd);
}