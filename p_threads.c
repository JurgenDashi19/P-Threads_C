#define _POSIX_SOURCE
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#undef _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

//Here initialize the arrays a pointer p which i will use to know where I'm looking in the graph and the start to write to the output file
int InpArray[16000];
int sum[16];
double ar_avg[16];
double geo[16];
int art[16];
int p=0;
int fp;
char t[200];
int n;

//just return the sum 
void *getsum(void *vargp){
  int c=0;
  for(int i=0; i<1000; i++){
    c=c+InpArray[p+i];
  }
  return (void *) c;
}
//just return the geometric answer
void *go(void *vargp){
  int c=0;
  for(int i=0; i<1000; i++){
    c=c+InpArray[p+i];
  }
  float x=pow(c,(float) 1/1000);
  float *returnf = malloc(sizeof(returnf));
  *returnf = x;
  return returnf;
}
//just return the arithmetic answer
void *pro(void *vargp){
  int c=0;
  for(int i=0; i<1000; i++){
    c=c+InpArray[p+i];
  }
  float x=c;
  x=x/1000;
  float *returnf = malloc(sizeof(returnf));
  *returnf = x;
  return returnf;

}

//this is the child which call the workers here is where the answers from the previus function are sent to and then stores them into to the array and writes to the output file
void *running(void *vargp){
	// sum pthread start and will ed before the next starts
    pthread_t thread_id;
    void *x;
    pthread_create(&thread_id, NULL, getsum, NULL);
    pthread_join(thread_id, &x);
    sum[p/1000]=(int)x;
        n=sprintf(t,"Worker Child Pthread Number = THREAD_NUMBER : %d Sum = %d\n", p/1000, (int)x);
        write(fp,t,n);
//geometric pthread start and whill end before the next starts
    pthread_t thread_id1;
    float *j = NULL;
    pthread_create(&thread_id1, NULL, go, NULL);
    pthread_join(thread_id1, (void **)&j);
    float f = *j;
    geo[p/1000]=f;
        n=sprintf(t,"Worker Child Pthread Number = TREAD_NUMBER : %d Geometric average = %f\n", p/1000, f);
        write(fp,t,n);
//arithmetic pthread start
    pthread_t thread_id2;
    float *j2 = NULL;
    pthread_create(&thread_id2, NULL, pro, NULL);
    pthread_join(thread_id2, (void **)&j2);
    float f2 = *j2;
    ar_avg[p/1000]=f2;
        n=sprintf(t,"Worker Child Pthread Number = THREAD_NUMBER : %d Arithmetic average = %f\n",p/1000, f2);
        write(fp,t,n);
}

int main(int argc, char *argv[]){
    mode_t mm= S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH; //initiate the mode for the out file
    fp=creat("file8.txt",mm);//initiate  out file
    FILE *myFile;
    myFile = fopen(argv[1], "r");//opne file
    for (int i = 0; i < 16000; i++){
        fscanf(myFile, "%d", &InpArray[i]);//fill array
    }
//creationg of the child pthreads
    pthread_t thread_id;
    for (int i=0; i<16; i++){
        p=(i*1000);
        pthread_create(&thread_id, NULL, running, NULL);
        pthread_join(thread_id, NULL);
    }
	//start of output and the part in which i will find the max for each of the sum, geo, and arth they are all the same but just looking though different arrays
    int s=0; float g=0; float a=0;
        n=sprintf(t,"THREAD_NUMBER - ");
        write(fp,t,n);
    for(int i=0; i<16; i++){
        n=sprintf(t," %d -",i);
        write(fp,t,n);
    }
     n=sprintf(t,"\nGEOMETRIC_AVERAGE ");
        write(fp,t,n);
    for(int i=0; i<16; i++){
        n=sprintf(t," -  %f",geo[i]);
        write(fp,t,n);
        if(g<geo[i])g=geo[i];
    }
   n=sprintf(t,"\nARITHMETIC_AVERAGE ");
        write(fp,t,n);
    for(int i=0; i<16; i++){
        n=sprintf(t," - %f", ar_avg[i]);
        write(fp,t,n);
        if(a<ar_avg[i])a=ar_avg[i];
    }
  n=sprintf(t,"\nSUM ");
        write(fp,t,n);
    for(int i=0; i<16; i++){
        n=sprintf(t," -  %d",(int)sum[i]);
        write(fp,t,n);
        if(s<sum[i])s=sum[i];
    }
	//final output
        n=sprintf(t,"\nMAX_OF_GEOM_AVG - %f\n MAX_OF_ARITH_AVG - %f\n MAX_OF_SUM - %d\n", g,a,s);
        write(fp,t,n);
  n=sprintf(t,"Main program thread: Max of Geometric Average = %f\n Main prgram thread: Max of Arithmetic avergae = %f\n Main program thread: Max of Sum = %d\n Main program thread: Terminating",g,a,s);
        write(fp,t,n);
   close(fp);
  exit(0);
}
