#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define abort(msg) do{printf(msg);exit(1);}while(0)

int posizione = 0;
int* appoggio;
int max;


void* func(void* param) {

	int size = *(int*) param;
	int pos = 0;
	int maxlocale;
	int trovato = 0;

	pos = __sync_fetch_and_add(&posizione,size);

	for(int i=pos;i<pos+size;i++) {
		if(appoggio[i]%2!=0) {
			maxlocale = appoggio[i];
			trovato = 1;
			break;
		}
	}

	if(trovato==0) return NULL;

	for(int i=pos;i<pos+size;i++) {
		if(appoggio[i]%2!=0 && appoggio[i]>=maxlocale) maxlocale = appoggio[i];
	}

	int oldmax;
	do {
		oldmax=max;
	}while(oldmax<maxlocale && !__sync_bool_compare_and_swap(&max,oldmax,maxlocale));


}

int disp(int* appoggio, int l) {
	int trovato = 0;
	int maxlocale;
	for(int i=0;i<l;i++) {
		if(appoggio[i]%2!=0) {
			maxlocale = appoggio[i];
			trovato = 1;
			break;
		}
	}

	if(trovato==0) {
		printf("\nNON CI SONO NUMERI DISPARI\n");
		exit(1);
	}

	for(int i=0;i<l;i++) {
		if(appoggio[i]%2!=0 && appoggio[i] >= maxlocale) maxlocale = appoggio[i];
	}

	return maxlocale;

}


int main(int argc, char*argv[])
{
	int lettura = atoi(argv[1]);
	int num_th = atoi(argv[2]);

	int fdr = open("/dev/urandom",O_RDONLY);
	if(fdr < 0) abort("errore apertura urandom\n");

	appoggio = malloc(lettura*sizeof(int));
	int rdr = read(fdr,appoggio,lettura*sizeof(int));
	if(rdr < 0) abort("errore lettura\n");

	for(int j=0;j<rdr/sizeof(int);j++) {
		printf("%d  ",appoggio[j]);
	}

	printf("\nIL MASSIMO TRA I NUMERI DISPARI DOVREBBE ESSERE: %d\n",disp(appoggio,lettura*sizeof(int)));

	int size = (rdr/sizeof(int))/num_th;
	int resto = (rdr/sizeof(int))%num_th;
	int rest = size+resto;

	pthread_t* tids = malloc(num_th*sizeof(pthread_t));

	int i;
	for(i=0;i<num_th;i++) {
		if(i<num_th-1) pthread_create(tids+i,NULL,func,&size);
		else pthread_create(tids+i,NULL,func,&rest);
	}

	for(i=0;i<num_th;i++) {
		pthread_join(tids[i],NULL);
	}

	printf("\nMASSIMO TRA IN NUMERI DISPARI: %d\n", max);

	close(fdr);
	free(tids);

	free(appoggio);

	return 0;
}