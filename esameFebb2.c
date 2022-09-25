#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define abort(msg) do{printf(msg);exit(1);}while(0)

char* stringa;
FILE* fp;
int tot=0;

int trova(char*appoggio,int rd) {
	int i=0;
	int ris=1;
	int trovato;
	for(i=0;i<strlen(stringa);i++) {
		trovato = 0;
		for(int j=0; j<rd;j++) {
			printf("%c,%c\n",appoggio[j],stringa[i]);
			printf("trovato1: %d\n",trovato);
			if(appoggio[j] == stringa[i]) trovato = 1;
		}
		printf("trovato2: %d\n",trovato);
		if(trovato==0) {
			ris=0;
			break;
		}
	}
	return ris;
}

void* func(void* p) {
	int righe = *(int*)p;
	int i;
	char* appoggio = NULL;
	size_t n = 0;
	int rd =0;
	int locale = 0;
	for(i=0;i<righe;i++) {
		rd = getline(&appoggio,&n,fp);
		if(trova(appoggio,rd)) {
			locale++;
		}
	}
	__sync_fetch_and_add(&tot,locale);

	free(appoggio);
}

int main(int argc, char *argv[])
{
	stringa = argv[2];
	int num_th = atoi(argv[3]);

	fp = fopen(argv[1],"r");
	if(fp==NULL) abort("no apertura file\n");

	int righe = 0;
	char*appoggio = NULL;
	size_t n = 0;
	while(getline(&appoggio,&n,fp)>0) righe++;

	fseek(fp,0,SEEK_SET);

	int riga = righe/num_th;
	int resto = righe%num_th;
	int rest = riga+resto;

	pthread_t* tids = malloc(num_th*sizeof(pthread_t));

	int i;
	for(i=0;i<num_th;i++) {
		if(i<num_th-1) pthread_create(tids+i,NULL,func,&riga);
		else pthread_create(tids+i,NULL,func,&rest);
	}

	for(i=0;i<num_th;i++) {
		pthread_join(tids[i],NULL);
	}


	fclose(fp);
	free(tids);
	free(appoggio);

	printf("TOTALE: %d\n",tot);

	return 0;
}