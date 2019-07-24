#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>



pthread_t *produtor;
pthread_t *consumidor;

sem_t buf_mutex,empty_count,fill_count;

int *buf,buf_pos=-1,prod_count,con_count,buf_len;


int produce(pthread_t self){
	int i = 0;
	int p = 1 + rand()%40;
	while(!pthread_equal(*(produtor+i),self) && i < prod_count){
		i++;
	}
	printf("Produtor %d produziu %d \n",i+1,p);
	return p;
}


void consume(int p,pthread_t self){
	int i = 0;
	while(!pthread_equal(*(consumidor+i),self) && i < con_count){
		i++;
	}

	printf("Buffer:");
	for(i=0;i<=buf_pos;++i)
		printf("%d ",*(buf+i));
	printf("\nConsumidor %d consumiu %d \n buffer : %d\n",i+1,p,buf_pos);
	
}


void* producer(void *args){

	while(1){
		int p = produce(pthread_self());
		sem_wait(&empty_count);
		sem_wait(&buf_mutex);
		++buf_pos;			// Seção critica
		*(buf + buf_pos) = p; 
		sem_post(&buf_mutex);
		sem_post(&fill_count);
		sleep(1 + rand()%3);
	}
	
	return NULL;
}


void* consumer(void *args){
	int c;
	while(1){
		sem_wait(&fill_count);
		sem_wait(&buf_mutex);
		c = *(buf+buf_pos);
		consume(c,pthread_self());
		--buf_pos;
		sem_post(&buf_mutex);
		sem_post(&empty_count);
		sleep(1+rand()%5);
	}

	return NULL;
}

int main(void){
	
	int i,err;

	srand(time(NULL));

	sem_init(&buf_mutex,0,1);
	sem_init(&fill_count,0,0);

	printf("Entre com o numero de Produtores:");
	scanf("%d",&prod_count);
	produtor = (pthread_t*) malloc(prod_count*sizeof(pthread_t));

	printf("Entre com o numero de consumidores:");
	scanf("%d",&con_count);
	consumidor = (pthread_t*) malloc(con_count*sizeof(pthread_t));

	printf("Entre com a capacidade do Buffer:");
	scanf("%d",&buf_len);
	buf = (int*) malloc(buf_len*sizeof(int));

	sem_init(&empty_count,0,buf_len);

	for(i=0;i<prod_count;i++){
		err = pthread_create(produtor+i,NULL,&producer,NULL);
		if(err != 0){
			printf("Erro criando produtor %d: %s\n",i+1,strerror(err));
		}else{
			printf("Successo criando produtor %d\n",i+1);
		}
	}

	for(i=0;i<con_count;i++){
		err = pthread_create(consumidor+i,NULL,&consumer,NULL);
		if(err != 0){
			printf("Erro criando consumidor %d: %s\n",i+1,strerror(err));
		}else{
			printf("Successo criando  %d\n",i+1);
		}
	}

	for(i=0;i<prod_count;i++){
		pthread_join(*(produtor+i),NULL);
	}
	for(i=0;i<con_count;i++){
		pthread_join(*(consumidor+i),NULL);
	}


	return 0;
}

