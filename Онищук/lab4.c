#include <sys/time.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t m_mutex;

int count_add = 0;
int count_find = 0;
int count_del = 0;

void *find();
void *add();
void *delete();

struct node{
	int data;
	struct node* next;
}

main(){
	printf("M=4, N=4, K=3 \n");
	int M, N, K;
	M = 4;
	N = 4;
	K = 3;
	struct node* head = NULL;
	add(&head);
	pthread_t addThread[M];
	pthread_t findThread[N];
	pthread_t deleteThread[K];
	struct tm *tm_ptr;
 	time_t theTime;
 	(void)time(&theTime);
 	tm_ptr = gmtime(&theTime);
	pthread_mutex_init(&m_mutex, NULL);
	for(int i=1; i<M; i++){	
		pthread_create(&addThread[i], NULL, add, &head);
	}
	for(int j=1; j<N; j++){	
		pthread_create(&findThread[j], NULL, find, &head);
		
	}
	for(int l=1; l<K; l++){
		pthread_create(&deleteThread[l], NULL, delete, &head);	
	}
	
	for(int i=1; i<M; i++){	
		pthread_join(addThread[i], NULL);
	}

	for(int j=1; j<N; j++){	
		pthread_join(findThread[j], NULL);
	}

	for(int l=1; l<K; l++){ 
		pthread_join(deleteThread[l], NULL);
	}   
}

void *find(struct node** head){
 	sleep(rand()%(4));
	struct tm *tm_ptr;
 	time_t theTime;
 	(void)time(&theTime);
 	tm_ptr = gmtime(&theTime);
	count_find++;
	printf("%02d:%02d:%02d Поисковик #%d хочет начать поиск в списке. \n",tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec,count_find);
	printf("%02d:%02d:%02d Поисковик #%d закончил поиск в списке. \n",tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec,count_find);
}

void *add(struct node** head){
	struct tm *tm_ptr;
	sleep(rand()%(7));
 	time_t theTime;
 	(void)time(&theTime);
 	tm_ptr = gmtime(&theTime);
	count_add++;
	printf("%02d:%02d:%02d Добавлятель #%d хочет начать добавление в список. \n",tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec, count_add);		
	pthread_mutex_lock(&m_mutex);
	printf("%02d:%02d:%02d Добавлятель #%d начал добавление в список. \n",tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec, count_add);
	struct node* newNode = malloc(sizeof(struct node));
	newNode->data = rand();
	newNode->next = *head;
	*head = newNode;
	pthread_mutex_unlock(&m_mutex);
	printf("%02d:%02d:%02d Добавлятель #%d закончил добавление в список. \n",tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec, count_add);
}

void *delete(struct node** head){
	struct tm *tm_ptr;
 	time_t theTime;
 	(void)time(&theTime);
 	tm_ptr = gmtime(&theTime);
	sleep(1);
	sleep(rand()%(7));
	count_del = count_del+1;
	printf("%02d:%02d:%02d Удалятель #%d хочет начать удаление из списка. \n",tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec, count_del);
	pthread_mutex_lock(&m_mutex);	
	struct node * temp = *head;
  	int data=temp->data;
	if(temp->next != NULL){
		printf("%02d:%02d:%02d Удалятель #%d начал удаление из списка. \n",tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec, count_del);
	  	*head = temp->next;
  		free(temp);
		printf("%02d:%02d:%02d Удалятель #%d закончил удаление из списка. \n",tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec, count_del);
	} else{
		printf("Список пуст\n");
	}
	pthread_mutex_unlock(&m_mutex);
	

	}

