#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define STDOUT 1

#define abort(msg) do{printf(msg);exit(1);}while(0)

//pthread_mutex_t ptmutex;

void* func(void* param) {
	
	int fd = *(int*)param; //fd associato al thread.

	int filesize = lseek(fd,0,SEEK_END); //calcolo filesize
	if(filesize == -1) abort("errore dimensione file\n");

	lseek(fd,0,SEEK_SET); //mi riposiziono all'inizio del file


/*Adesso, divido il file in 4 porzioni.
  Potrei leggere anche un byte alla volta ma questo 
  implicherebbe l'acquisizione di lock e il suo rilascio
  ad ogni scrittura di un singolo byte, quindi credo sia meglio che quando un thread
  acquisisca un lock faccia più scritture (quindi in questo caso 4 porzioni)
  sullo STDOUT.
  Viceversa, utilizzare un buffer che salvi l'intero file
  potrebbe essere non efficiente dal punto di vista della memoria.
  Queste affermazioni sono giuste?*/

	int chunk = filesize/4;
	int remainder = filesize%4;

	int start = 1;
	int count = 1;
	int rd;
	int lettura;
	char* buffer = NULL;

	while(count <= 4) {
		lettura=0;
		if(start) {
			lettura = chunk + remainder;
			start = 0;
		}
		else lettura = chunk;

		buffer = malloc(lettura*sizeof(char));
		if((rd = read(fd,buffer,lettura*sizeof(char))) < 0) abort("errore lettura\n");
		
		/*protezione di scrittura su STDOUT*/

		//pthread_mutex_lock(&ptmutex);
		int scrivo = 0;
		while(scrivo != rd) {
			int wr = write(STDOUT,buffer,lettura);
			if (wr == -1) abort("errore scrittura\n");
			scrivo += wr;
		}
		//pthread_mutex_unlock(&ptmutex);

		free(buffer);

		count++;
	}

}


void tunnel(int descriptors[], int count) {
	
	pthread_t* tids = malloc(count*sizeof(pthread_t));

	
	//pthread_mutex_init(&ptmutex,NULL);
	int i;

	for(i=0; i<count; i++) {
		pthread_create(tids+i,NULL,func,descriptors+i); //associo ad ogni thread un fd.
	}

	for(i=0; i<count; i++) {
		pthread_join(tids[i],NULL);
	}

	//pthread_mutex_destroy(&ptmutex);

	free(tids);
}

int main(int argc, char const *argv[])
{

	//è un test della funzione tunnel
	//con dei file precedentemente  creati.

	int fd1,fd2,fd3;

	fd1 = open("fileA.txt",O_RDONLY);
	fd2 = open("fileB.txt",O_RDONLY);
	fd3 = open("fileC.txt",O_RDONLY);

	if(fd1<0 || fd2<0 || fd3<0) abort("errore apertura canale\n");

	int* descriptors = malloc(3*sizeof(int));
	descriptors[0] = fd1;
	descriptors[1] = fd2;
	descriptors[2] = fd3;

	tunnel(descriptors,3);

	free(descriptors);

	return 0;
}
