#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>

pthread_mutex_t m;
int minprnts,
    pairs=0, 
    chforprnt=0,
    t=5,//сколько раз исполняется каждая нить
    s=15;//время ожидания нити
int action[100],prnt[100],chldrn[100];


int sum(int arr[])
{
  int x=0;
  for(int j=0;j<pairs;j++)
   	if(arr[j]==1)
	  x=x+1;
  return x;
}

void *my_thread(void *arg)
{
  for (int i = 0; i < t; i++)
  {
    sleep(t/2);
    pthread_mutex_lock(&m);
	int a=((int)arg), prntcount = sum(prnt), chldrncount = sum(chldrn);
	long int tt = time (NULL);
	switch(action[a])
	{
	  case 0:
	    action[a]=1;
    	prnt[a]=1;
	    chldrn[a]=1;
		prntcount=prntcount+1;
		chldrncount=chldrncount+1;
		printf("Родитель№ %d Привел ребенка и остался с ним - Родителей:%d - Детей:%d\n",a,prntcount,chldrncount);
        break;
	  case 1:
	    if(prntcount>minprnts)//может
	    {
	      action[a]=2;
	      prnt[a]=0;
		  prntcount=prntcount-1;
	      printf("Родитель № %d Ушел, оставив ребенка - Родителей:%d - Детей:%d\n",a,prntcount,chldrncount);
	    }
		else
		{
	      printf("№ %d Остался с ребенком - Родителей:%d - Детей:%d\n",a,prntcount,chldrncount);
		}
		break;
	  case 2://родитель приходит и забирает ребёнка
	    action[a]=0;
	    chldrn[a]=0;
		chldrncount=chldrncount-1;
        printf("№ %d Забрал ребенка и ушел - Родителей:%d - Детей:%d\n",a,prntcount,chldrncount);
	    break;
	}
	srand(tt);
	int r = (rand()-chldrncount-prntcount-(chldrncount/prntcount))%s;	
    pthread_mutex_unlock(&m);
	
	sleep(r);
  }
}

int main()
{
  while (pairs<1||pairs>100)
  {
    printf("Введите число пар ребенок-родитель :\n");
    scanf("%d", &pairs);
  }
  while (chforprnt<1||chforprnt>pairs)
  {
    printf("Введите максимальное число детей за которыми должен смотреть минимум 1 родитель (меньше или равно %d):\n",pairs);
    scanf("%d", &chforprnt);
  }
  minprnts=pairs/chforprnt;
  if (pairs%chforprnt>0)
    minprnts=minprnts+1;
	
  pthread_t threads[pairs];
  pthread_mutex_init(&m, NULL);
  
  int i;
  for (i = 0; i < pairs; i++)
  {
    if (pthread_create(&threads[i], NULL, my_thread, (void *)i))
    {
      printf("Error");
      return -1;
    }
	else
	{
	  printf("Создан тред № %d\n",i);
	}
  }

  for (i = 0; i < pairs; i++)
  {
    pthread_join(threads[i], NULL);
  }
  for (i = 0; i < pairs; i++)//все родители заабирают своих детей;
  {
	  if(chldrn[i]==1)
	  {
	    action[i]=0;
		prnt[i]=0;
		chldrn[i]=0;
		printf("%d родитель забрал ребенка - %d - %d\n",i,prnt[i],chldrn[i]);
	  }
  }
  printf("Все родители забрали своих детей.");
  return 0;
}
