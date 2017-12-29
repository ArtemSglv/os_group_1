#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h> /*это библиотека потоков*/
#include <errno.h>
#include <unistd.h>
#include <time.h>

#define COUNT 5 /*Кол-во философов и вилочек*/

int t; /*тайминги без еды*/
char currtime[10];
time_t now;

enum philosopher_state { thinking, eating, dead }; /*перечесление для состояни философа*/
enum fork_state { is_free, is_busy }; /*перечесление для состояния вилки*/

struct fork { /*вилка*/
	enum fork_state state;
	pthread_mutex_t mutex;
};

struct philosopher { /*философ*/
	enum philosopher_state state;
	int last_eating_time;
	pthread_mutex_t mutex;
	pthread_t thread;
	pthread_t thread_life;
	struct fork* left_fork;
	struct fork* right_fork;
	int own_left_fork;
	int own_right_fork;
};

struct philosopher philosophers[COUNT]; /*массив философов*/
struct fork forks[COUNT]; /*массив вилок*/

void *philosopher_t(void* num);
void *last_eating_time_tick(void* num);

int main() {
	srand(time(NULL));
	printf("%s\n", "Введите T - время, которое философ может прожить без еды:");
	scanf("%d", &t);
	for (int i = 0; i < COUNT; i++) { /*начальная иницилизация*/
		philosophers[i].state = thinking;
		philosophers[i].last_eating_time = 0;
		philosophers[i].mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER; /*инициализация мьютекса*/

		forks[i].state = is_free;
		forks[i].mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER; /*инициализация мьютекса*/
		/*ну и так далее инициализация крч*/
		if (i == 0)
			philosophers[i].left_fork = &forks[COUNT - 1];
		else
			philosophers[i].left_fork = &forks[i - 1];

		if (i == COUNT - 1)
			philosophers[i].right_fork = &forks[0];
		else
			philosophers[i].right_fork = &forks[i];

		philosophers[i].own_left_fork = 0;
		philosophers[i].own_right_fork = 0;
		/*создание нитей для работы*/
		pthread_create(&(philosophers[i].thread), NULL, philosopher_t, (void*)(intptr_t)i); 
		pthread_create(&(philosophers[i].thread_life), NULL, last_eating_time_tick, (void*)(intptr_t)i);

	}
	for (int i = 0; i < COUNT; i++) {
		pthread_join(philosophers[i].thread, NULL); /*блокируем вызывающий поток до завершенич указанного*/
	}

	return 0;
}

void print_forks_state() { /*состояние вилок*/
	printf("Состояние вилок: ");
	for (int i = 0; i < COUNT; i++) {
		printf("%d", forks[i].state); /*печать состояния*/
	}
	printf("\n");
}

void start_eating(int n) { /*старт*/
	pthread_mutex_lock(&philosophers[n].mutex); /*захватываем для выполнения критической секции*/
	philosophers[n].state = eating; /*изменяем статус философа*/
	now = time(NULL);
	strftime(currtime, 20, "%H:%H:%S", localtime(&now));
	printf("%s Философ %d начал есть. \n", currtime, n + 1);
}

void end_eating(int n) { 
	philosophers[n].state = thinking;
	philosophers[n].last_eating_time = 0;
	now = time(NULL);
	strftime(currtime, 20, "%H:%H:%S", localtime(&now));
	printf("%s Философ %d закончил есть. \n", currtime, n + 1);
	pthread_mutex_unlock(&philosophers[n].mutex); /*отпускаем*/
}

void *philosopher_t(void* num) {
	int n = (int)(intptr_t)num;
	while (philosophers[n].state != dead) { /*пока живы*/
		int f = rand() % 2;
		switch (f) {
		case 0:
			pthread_mutex_lock(&philosophers[n].left_fork->mutex); /*захватили*/
			if (philosophers[n].own_left_fork) { /*чекаем вилку в левой руке*/
				philosophers[n].left_fork->state = is_free; /*меняем состояние вилки*/
				philosophers[n].own_left_fork = 0;
				now = time(NULL);
				strftime(currtime, 20, "%H:%H:%S", localtime(&now));
				printf("%s Философ %d положил левую вилку. ", currtime, n + 1);
				print_forks_state(); /*поясняем за текущее состояние вилок*/

				if (philosophers[n].state == eating) { /*если кушает*/
					end_eating(n);
				}
			}
			else {
				if (philosophers[n].left_fork->state == is_free) { /*если вилочка левая свободна*/
					philosophers[n].left_fork->state = is_busy; /*меняем состояние*/
					philosophers[n].own_left_fork = 1;
					now = time(NULL);
					strftime(currtime, 20, "%H:%H:%S", localtime(&now));
					printf("%s Философ %d взял левую вилку. ", currtime, n + 1);
					print_forks_state(); /*печатаем вилочек статус*/

					if (philosophers[n].own_right_fork) { /*если с вилками всё хорошо, то стартуем кушать*/
						start_eating(n);
					}
				}
				else { /*если с вилками не заладилось*/
					now = time(NULL);
					strftime(currtime, 20, "%H:%H:%S", localtime(&now));
					printf("%s Философ %d не смог взять левую вилку. ", currtime, n + 1);
					print_forks_state();
				}
			}
			pthread_mutex_unlock(&philosophers[n].left_fork->mutex); /*отпустили*/
			break;
		case 1:
			pthread_mutex_lock(&philosophers[n].right_fork->mutex); /*захватили*/
			if (philosophers[n].own_right_fork) { /*чекаем вилку в правой руке*/
				philosophers[n].right_fork->state = is_free; /*меняем состояние вилочки*/
				philosophers[n].own_right_fork = 0;
				now = time(NULL);
				strftime(currtime, 20, "%H:%H:%S", localtime(&now));
				printf("%s Философ %d положил правую вилку. ", currtime, n + 1);
				print_forks_state();

				if (philosophers[n].state == eating) { /*если он уже кушает, то очень хорошо*/
					end_eating(n); /*продолжаем поедание*/
				}
			}
			else {
				if (philosophers[n].right_fork->state == is_free) { /*если вилочка правая свободна*/
					philosophers[n].right_fork->state = is_busy; /*состояние меняем */
					philosophers[n].own_right_fork = 1;
					now = time(NULL);
					strftime(currtime, 20, "%H:%H:%S", localtime(&now));
					printf("%s Философ %d взял правую вилку. ", currtime, n + 1);
					print_forks_state(); /*текущее состояние вилок*/

					if (philosophers[n].own_left_fork) {
						start_eating(n); /*кушаем*/
					}
				}
				else { /*неудача (9((99(((9((9*/
					now = time(NULL);
					strftime(currtime, 20, "%H:%H:%S", localtime(&now));
					printf("%s Философ %d не смог взять правую вилку. ", currtime, n + 1);
					print_forks_state();
				}
			}
			pthread_mutex_unlock(&philosophers[n].right_fork->mutex); /*отпустили*/
			break;
		}

		int wait = rand() % 10;
		sleep(wait);
	}
	pthread_exit(NULL);
}

void *last_eating_time_tick(void* num) {
	int n = (int)(intptr_t)num;
	while (philosophers[n].state != dead) { /*пока не мёртвый )))0)))0))00*/
		while (philosophers[n].last_eating_time <= t) { /*чекаем, а может ли он жить ещё???!!! тип время того, как он может быть без еды*/
			sleep(1); /*задержка, но не месячных ))0)0)))0)*/
			philosophers[n].last_eating_time++; /*собсна, увеличиваем время без еды*/
		};
		if (!pthread_mutex_trylock(&philosophers[n].mutex)) { /*удалось ли захватить*/
			philosophers[n].state = dead; /*он погиб за тебя, а ты даже не знаешь его имени*/
			philosophers[n].left_fork->state = is_free; /*освобождаем вилочки*/
			philosophers[n].right_fork->state = is_free;
			now = time(NULL);
			strftime(currtime, 20, "%H:%H:%S", localtime(&now));
			printf("%s Философ %d умер от голода :(\n", currtime, n + 1);

			pthread_mutex_unlock(&philosophers[n].mutex); /*отпустили*/
		}
	}
	pthread_exit(NULL); /*завершение нити*/
}