#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define abort(msg) do{printf(msg);exit(1);}while(0)

char stringa[100];
FILE* fp;

void* func(void* param) {
	int righe = *(int*)param;

	char* appoggio = NULL;
	ssize_t n = 0;

	int i;
	for(i=0; i<righe; i++) {
		getline(&appoggio,&n,fp);

		printf("%s\n",appoggio);
		if(strcmp(appoggio,stringa) == 0) {
			printf("TROVATO\n");
			exit(1);
		}

		free(appoggio);
		appoggio = NULL;
		n = 0;
	}
}


int main(int argc, char const *argv[])
{
	fgets(stringa,100,stdin);
	const char* filename = argv[1];
	int num_thr = atoi(argv[2]);

	fp = fopen(filename,"r");
	if(fp == NULL) abort("errore apertura\n");

	int righe=0;

	char* appoggio = NULL;
	size_t n = 0;
	while(getline(&appoggio,&n,fp) > 0) {
		righe++;
	}


	int riga_thr = righe/num_thr;
	int resto = righe%num_thr;
	int rest = riga_thr+resto;

	fseek(fp,0,SEEK_SET);

	pthread_t* tids = malloc(num_thr*sizeof(pthread_t));

	int i;
	for(i=0; i<num_thr; i++) {
		
		if(i<num_thr-1) {
			pthread_create(tids+i,NULL,func,&rest);
		}
		else {
			pthread_create(tids+i,NULL,func,&riga_thr);
		}
	}

	for(i=0; i<num_thr; i++) {
		pthread_join(tids[i],NULL);
	}


	free(tids);
	fclose(fp);

	return 0;
}