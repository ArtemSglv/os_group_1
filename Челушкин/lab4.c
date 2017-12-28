#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

pthread_mutex_t 
generate_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t space_available = PTHREAD_COND_INITIALIZER;
pthread_cond_t data_available = PTHREAD_COND_INITIALIZER;
pthread_mutex_t santa_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t santa_available = PTHREAD_COND_INITIALIZER;

int N=5;
int K=2;
int M=5;
int T=10;
typedef struct roli {
	char *an;
	int number;
} roli;

typedef struct telo {
    int *o;
    int *e;
    int *t;
	int e_count;
	int o_count;
	int buffer_count;
	int is_back; 
} telo;

roli buffer, elf, olen;
telo _santa;

void print_time() {
	time_t timer;
	static char buffer[26];
	struct tm* tm_info;
	time(&timer);
	tm_info = localtime(&timer);
	strftime(buffer, 26, "%H:%M:%S ", tm_info);
	printf("%s",buffer);
}



int main(int argc, char **argv) {
	int n;

	buffer.an = NULL;
	elf.an = "Эльф";
	olen.an = "Олень";
	void *generate();
	void *coming();
	void *santa();
	pthread_t generate_thread, coming_thread, santa_thread;
    pthread_create(&generate_thread, NULL, generate, NULL);
    pthread_create(&coming_thread, NULL, coming, NULL);
	pthread_create(&santa_thread, NULL, santa, NULL);
	pthread_join(generate_thread, NULL);
	pthread_join(coming_thread, NULL);
	pthread_join(santa_thread, NULL);
	return 0;
}

void *generate() {
	srand(time(0));	
    while (1) {
        pthread_mutex_lock(&generate_mutex);
        if (buffer.number)
            pthread_cond_wait(&space_available, &generate_mutex);	
		int sleepAmount = rand() % 4 + 1;
		sleep(sleepAmount);
		if (sleepAmount % 2) {
			 int c= rand() % M +0;
             
             elf.number=c+1;
	         buffer = elf;
		
}
		else {
			
            int p= rand() % N +0;
        
            olen.number=p+1;
            buffer = olen;


		}
        pthread_cond_signal(&data_available);
        pthread_mutex_unlock(&generate_mutex);
    }
    pthread_exit(NULL);
}

void *coming() {
	while (1) {
		pthread_mutex_lock(&generate_mutex);
        if (!buffer.number)
            pthread_cond_wait(&data_available, &generate_mutex);
        pthread_cond_signal(&space_available);
        pthread_mutex_unlock(&generate_mutex);
		roli rolb = buffer;
		buffer.an = NULL;
		buffer.number = 0;
		
		pthread_mutex_lock(&santa_mutex);
		if (_santa.is_back)
	      	pthread_cond_wait(&data_available, &santa_mutex);
        if (rolb.an == elf.an && _santa.e[rolb.number-1]!=1) {
               _santa.e_count++;
               _santa.e[rolb.number-1]=1;
                print_time();
                if (_santa.e_count>=K && _santa.is_back==0){
                      printf("%s %d подошел к двери. %d эльфа будят Санту. \n", rolb.an, rolb.number, _santa.e_count  );
               }
                else {
               




                printf("%s %d подошел к двери. ", rolb.an, rolb.number);
                printf("Ожидающих эльфов: %d. ", _santa.e_count);
		        printf("Ожидающих оленей: %d.\n", _santa.o_count);
               }
		}	
		if (rolb.an == olen.an && _santa.o[rolb.number-1]!=1) {
          _santa.o_count++;
          _santa.o[rolb.number-1]=1;
           print_time();
           printf("%s %d подошел к двери. ", rolb.an, rolb.number);
           printf("Ожидающих эльфов: %d. ", _santa.e_count);
		   printf("Ожидающих оленей: %d.\n", _santa.o_count);
		}
		pthread_mutex_unlock(&santa_mutex);
    }
    pthread_exit(NULL);
}

void *santa() {
	srand(time(0));
    _santa.o =(int*)malloc(N*sizeof(int));
    _santa.e =(int*)malloc(M*sizeof(int));
    _santa.t =(int*)malloc(M*sizeof(int));
     int l=0;
    print_time();
    printf("Санта спит.\n");
	while(1) {
		if (_santa.o_count == N) {
			pthread_mutex_lock(&santa_mutex);
			print_time();
			printf("Санту разбудили %d оленей.Санта развозит игрушки.\n", N);
			pthread_mutex_unlock(&santa_mutex);
            _santa.is_back = 1;
			_santa.o_count = 0;
			sleep(rand() % 10 + 10);
			pthread_mutex_lock(&santa_mutex);
			print_time();
			printf("Санта с оленями вернулся.\n");
            for (int i=0; i<N; i++){
                 _santa.o[i]=0;
                
            }
            print_time();
            if (_santa.e_count<K) {
                print_time();
                printf("Санта уснул.\n");
             }
			_santa.is_back = 0;
			pthread_cond_signal(&santa_available);
			pthread_mutex_unlock(&santa_mutex);
		}
        else if (_santa.e_count >= K) {
			pthread_mutex_lock(&santa_mutex);
            for (int i=0; i<M; i++){
                 if (_santa.e[i]==1) {
                   print_time();
                   printf("Санта пропускает в кабинет эльфа %d.\n",i+1);
                   _santa.t[l]=i;
                   l=l+1;
                 }

                
            }
            print_time();
			printf("Санта c эльфами начинают совещание.\n");
			pthread_mutex_unlock(&santa_mutex);
            _santa.is_back = 1;
			_santa.e_count = 0;
			sleep(T);
			pthread_mutex_lock(&santa_mutex);
			print_time();
			printf("Санта с эльфами закончил совещание.\n");
             for (int i=0; i<K; i++){
                
                   _santa.e[_santa.t[i]]=0;
                   _santa.t[i]=0;
                   l=0;
                 

                
            }
            
            if (_santa.e_count<K) {
                print_time();
                printf("Санта уснул.\n");
             }
			_santa.is_back = 0;
			pthread_cond_signal(&santa_available);
			pthread_mutex_unlock(&santa_mutex);
		}
	}
	pthread_exit(NULL);
}
