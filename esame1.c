/*#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#define FILENAME "dataset.bin"
#define NUM_THREADS 4
#define NUM_THRESHOLDS 3

#define abort(msg) do{printf(msg);exit(1);}while(0)

int thresholds[NUM_THRESHOLDS] = {0x0, 0x40000000, 0xc0000000};
int counter = 0;
int fd;
int thresh;


void* func(void* param) {
  int size = *(int*) param;

  int i;
  int rd;
  int locale = 0;
  int* buffer = malloc(size*sizeof(int));
  rd = read(fd,buffer,size*sizeof(int));
  if(rd < 0) abort("errore lettura");

  for(i=0;i<size;i++) {
    if(buffer[i]>thresh) locale++;
  }

    
    if(locale != 0)
    __sync_fetch_and_add(&counter,locale);

  free(buffer);
}


int above_threshold(char *filename, int count, int threshold){
   int total_count = 0;
   thresh = threshold;

   fd = open(filename,O_RDONLY);
   if(fd < 0) abort("errore apertura");

   int filesize = lseek(fd,0,SEEK_END);
   if(filesize < 0) abort("errore filesize\n");

   lseek(fd,0,SEEK_SET);

   int size = (filesize/sizeof(int))/count;
   int resto = (filesize/sizeof(int))%count;
   int rest = size + resto;

   pthread_t* tids = malloc(count*sizeof(pthread_t));

   int i;
   for(i=0; i<count; i++) {
      if(i<(count-1)) {
        pthread_create(tids+i,NULL,func,&size);
      }
      else {
        pthread_create(tids+i,NULL,func,&rest);
      }
   }

    for(i=0; i<count; i++) {
      pthread_join(tids[i],NULL);
   }

   free(tids);
   total_count = counter;
   counter=0;
   close(fd);
   printf("%d\n",total_count);
   return total_count;
}


int main(){
  int res;
  int i;
  for(i=0;i<NUM_THRESHOLDS;i++){
    res = above_threshold(FILENAME, NUM_THREADS, thresholds[i]);
    printf("The correct answer for %d is %d\n", thresholds[i], res);
  }
}
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#define FILENAME "dataset.bin"
#define NUM_THREADS 4
#define NUM_THRESHOLDS 3

#define abort(msg) do{printf(msg);exit(1);}while(0)

int thresholds[NUM_THRESHOLDS] = {0x0, 0x40000000, 0xc0000000};

int fd;
int chunk = 1024;
int thresh;
int counter=0;

void* func(void* param) {
    int rd = 0;
    int* buffer = malloc(chunk*sizeof(int));
    int locale = 0;

    while((rd=(read(fd,buffer,chunk*sizeof(int)))) > 0) {
        locale = 0;
        for(int i=0; i<rd/sizeof(int);i++) {
            if(buffer[i]>thresh) locale++;
        }

        if (locale != 0) __sync_fetch_and_add(&counter,locale);
    }

    free(buffer);
}



int above_threshold(char *filename, int count, int threshold){
   int total_count = 0;
   counter=0;
   
   thresh = threshold;

   fd = open(filename,O_RDONLY);
   if(fd < 0) abort("errore apertura\n");

   pthread_t* tids = malloc(count*sizeof(pthread_t));

   int i;
   for(i=0;i<count;i++) {
      pthread_create(tids+i,NULL,func,NULL);
   }

    for(i=0;i<count;i++) {
      pthread_join(tids[i],NULL);
   }

   free(tids);
   close(fd);
   total_count = counter;
   printf("%d\n",total_count);
   
   return total_count;
}


int main(){
  int res;
  int i;
  for(i=0;i<NUM_THRESHOLDS;i++){
    res = above_threshold(FILENAME, NUM_THREADS, thresholds[i]);
    printf("The correct answer for %d is %d\n", thresholds[i], res);
  }
}
