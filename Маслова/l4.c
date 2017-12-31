#define _CRT_SECURE_NO_WARNINGS
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t way_mutex;

pthread_cond_t too_many_people, box_start_way, box_end_way;

int box_count, people_count, people_id, N, P, T;

int randtime()
{
    return 10*1000 + rand() % (1000*1000);
}

void printtime()
{
    printf("Now - ");
}
box_count++;
void main()
{
    printf("Введите задаваемые числа задачи (N P T): ");
    scanf("%d %d %d", &N, &P, &T);

    printf("Программа запустится с параметрами: %d тележек, %d людей, %d единиц времени проходит поездка.\n\n", N, P, T*1000*1000);

    box_count = 0;
    people_count = 0;
    people_id=0;

    pthread_mutex_init(&way_mutex, NULL);

    pthread_cond_init(&too_many_people, NULL);
    pthread_cond_init(&box_start_way, NULL);
    pthread_cond_init(&box_end_way, NULL);

    pthread_t box_thread[N]; //control_thread, box_thread[N];
    void* box();//,// control();
    //pthread_create(&control_thread, NULL, control, NULL);
    for(int i = 0; i < N; i++)
        pthread_create(&box_thread, NULL, box, i);

    set_people();
    //pthread_join(control_thread, NULL);
}

void tryGo()
{
    if(people_count>=P)
    {
        //printtime();
        //printf("Попытка поехать, people_count = %d\n", people_count);

        pthread_cond_signal(&too_many_people);
    }
}

void set_people()
{
    while(1)
    {
        usleep(randtime()*3);

        //pthread people_thread;
        //pthread_create(&people_thread, NULL, people, NULL);
        //pthread_join(&people_thread, NULL);

        pthread_mutex_lock(&way_mutex);
        people_count++;
        printtime();
        printf("Человек %d подходит в очередь.  Тележек: %d. Людей в очереди: %d\n", people_id, box_count, people_count);
        people_id++;
        pthread_mutex_unlock(&way_mutex);
        //printtime();
        //printf("Тутууу\n");
        tryGo();
    }
}


void* box(int id)
{
    while(1)
    {
        pthread_mutex_lock(&way_mutex);
        box_count++;
        printtime();
        printf("Тележка %d подъезжает на посадку. Тележек: %d. Людей в очереди: %d\n", id, box_count, people_count);
        pthread_mutex_unlock(&way_mutex);

        //pthread_cond_wait(&box_start_way, &way_mutex);
        pthread_cond_wait(&too_many_people, &way_mutex);
        people_count-=P;
        box_count--;
        printtime();
        printf("Тележка %d начинает поезку. Тележек: %d. Людей в очереди: %d\n", id, box_count, people_count);
        pthread_mutex_unlock(&way_mutex);

        usleep(T*10*1000*1000);

        pthread_mutex_lock(&way_mutex);
        printtime();
        printf("Тележка %d закснчивает поезку. Тележек: %d. Людей в очереди: %d\n", id, box_count, people_count);
        pthread_mutex_unlock(&way_mutex);

        pthread_cond_broadcast(&box_end_way);
        //pthread_mutex_unlock(&way_mutex);
    }
}

//void* people(int id)
//{
  //  pthread_mutex_lock(&way_mutex);
    //people_count++;
    //printtime();
    //printf("Человек %d подходит в очередь. Людей в очереди: %d\n", id, people_count);
    //pthread_mutex_unlock(&way_mutex);

    //pthread_cond_wait(&box_start_go, &way_mutex);
//
    //printtime();
    //printf("Человек %d садится в тележку. Людей в очереди: %d\n", id, people_count);
//

    //pthread_cond_wait(&box_end_go, &way_mutex);

//}

void* control()
{
    //while(1)
    //{
     //   pthread_cond_wait(&too_many_people, &way_mutex);
      //  while()

       // pthread_mutex_unlock(&way_mutex);
    //}
}
