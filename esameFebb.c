#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define abort(msg) do{printf(msg);exit(1);}while(0)

FILE* fp;

void* func(void* param) {
	int righe = *(int*) param;
	char* appoggio = NULL;
	size_t n=0;
	int rd=0;
	int i;
	int counter=0;
	
	for(i=0; i<righe; i++) {
		rd=0;
		counter=0;
		rd=getline(&appoggio,&n,fp);
		if(rd<0) abort("getline non andata a buon fine\n");
		int j;
		for(j=0; j<rd-1; j++) {
			if(appoggio[j] == ' ') {
				for(int k=j; k<rd-1; k++) {
					appoggio[k]=appoggio[k+1];
				}
				counter++;
			}
		}
		
		appoggio[strlen(appoggio)-1-counter] = '\0';
		
		int ok=1;
		for(int j=0; j<strlen(appoggio); j++) {
			printf("%c,%c\n",appoggio[j],appoggio[strlen(appoggio)-1-j]);
			if(appoggio[j]!=appoggio[strlen(appoggio)-1-j]) {
				ok=0;
				break;
			}
		}
		 printf("%s\n",appoggio);
	}
}


int main(int argc, char const *argv[]) {
	
	const char* filename = argv[1];
	int num_thr = atoi(argv[2]);

	fp = fopen(filename,"r");
	if(fp == NULL) abort("errore apertura canale\n");

	int righe = 0;

	char* appoggio = NULL;
	size_t n = 0;

	while(getline(&appoggio,&n,fp)>0) {
		righe++;
	}

	fseek(fp,0,SEEK_SET);

	int riga = righe/num_thr;
	int resto = righe%num_thr;
	int rest = riga+resto;

	pthread_t* tids = malloc(num_thr*sizeof(pthread_t));
	int i;
	for(i=0; i<num_thr; i++) {
		if(i<num_thr-1) {
			pthread_create(tids+i,NULL,func,&rest);
		}
		else {
			pthread_create(tids+i,NULL,func,&riga);
		}
	}

	for(i=0; i<num_thr; i++) {
		pthread_join(tids[i],NULL);
	}



	free(tids);
	fclose(fp);
	return 0;
}