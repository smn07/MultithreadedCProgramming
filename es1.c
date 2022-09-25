#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char const *argv[])
{
	int sizerd,sizewr;

	int fdev = open("/dev/urandom",O_RDONLY);
	if(fdev==-1) {
		printf("errore apertura file\n");
		exit(1);
	}

	int fd = open(argv[2],O_CREAT | O_TRUNC | O_WRONLY,0660);
	if(fd==-1) {
		printf("errore apertura file\n");
		exit(1);
	}

	int n = atoi(argv[1]);
	int* buffer = (int*) malloc(n*sizeof(int));


	sizerd = read(fdev,buffer,n);
	if(sizerd==-1) {
		printf("errore lettura file\n");
		exit(1);
	}

	close(fdev);

	sizewr = write(fd,buffer,sizerd);
	if(sizewr==-1) {
		printf("errore scrittura file\n");
		exit(1);
	}

	close(fd);

}