#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define abort(msg) do{printf(msg);exit(1);}while(0)

#define DIM_MAX 20

char* filename;


void* func(void* param) {
	int i_th = *(int*)param;

	char buffer[20];

	printf("sono thread %d, inserisci stringa: ",i_th);
	scanf("%s",buffer);

	FILE* fp = fopen(filename,"r");
	if(fp == NULL) abort("errore apertura");

	ssize_t rd;

	char* string = NULL;
	ssize_t n = 0;
	int stoppato = 0;

	while(getline(&string,&n,fp)>0) {
		printf("getline: %s\n",string);
		if(strstr(string,buffer) == NULL) {
			stoppato = 1;
			break;
		}
	}

	if(stoppato==0) {
		printf("SONO IL THR %d, e la stringa è: %s\n",i_th,buffer);
		free(string);
	}


	fclose(fp);

}


void file_check(char* file_name, int num_theads) {

	filename = file_name;

	pthread_t* tids = malloc(num_theads*sizeof(pthread_t));
	int i;

	int* indici = malloc(num_theads*sizeof(int));

	for(i=0; i<num_theads; i++) {
		indici[i] = i;
		pthread_create(tids+i,NULL,func,indici+i);
	}

	for(i=0; i<num_theads; i++) {
		pthread_join(tids[i],NULL);
	}

	free(tids);
	free(indici);
}

int main(int argc, char const *argv[]) {
	
//prova della funzione con file precedentemente creato
	file_check("fileA.txt",4);


	return 0;
}