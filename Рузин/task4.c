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
pthread_mutex_t parom_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t parom_available = PTHREAD_COND_INITIALIZER;

int n = 4;
typedef struct Employees {
	char *company;
	int number;
} Employees;

typedef struct Parom {
	int capacity;
	int number_of_m_on_parom; 
	int number_of_a_on_parom; 
	int m_waiting; 
	int a_waiting;
	Employees *on_parom_employees; 
	Employees *waiting_employees; 
	Employees *employees_in_queue; 
	int queue_count;
	int arrived; 
	int on_theWay;
} Parom;

Employees queue, microsoft, apple;
Parom _parom;

void print_time() {
	time_t timer;
	static char queue[26];
	struct tm* tm_info;

	time(&timer);
	tm_info = localtime(&timer);

	strftime(queue, 26, "%H:%M:%S ", tm_info);
	printf("%s",queue);
}

void *generate() {
	srand(time(0));	
	while (1) {
        pthread_mutex_lock(&generate_mutex); 
        if (queue.number)
            pthread_cond_wait(&space_available, &generate_mutex);	
		int sleepAmount = rand() % 10 + 1; 
		sleep(sleepAmount);
		if (sleepAmount % 2) { 
			microsoft.number++;
			queue = microsoft;
		}
		else { 
			apple.number++;
			queue = apple;
		}
        pthread_cond_signal(&data_available);
        pthread_mutex_unlock(&generate_mutex); 
    }
    pthread_exit(NULL);
}

void *fill_parom() { 
	while (1) {
		pthread_mutex_lock(&generate_mutex);
        if (!queue.number)
            pthread_cond_wait(&data_available, &generate_mutex);
        pthread_cond_signal(&space_available);
        pthread_mutex_unlock(&generate_mutex);
		Employees employee = queue;  
		queue.company = NULL;
		queue.number = 0;
		
		pthread_mutex_lock(&parom_mutex);
		if (_parom.arrived)
			pthread_cond_wait(&data_available, &parom_mutex);
		if (employee.company == microsoft.company) { 
			if (_parom.number_of_m_on_parom < n && _parom.number_of_a_on_parom <= n || 
			_parom.number_of_m_on_parom >= n && !_parom.number_of_a_on_parom && _parom.number_of_m_on_parom < _parom.capacity)
				_parom.on_parom_employees[_parom.number_of_m_on_parom++ + _parom.number_of_a_on_parom] = employee; 
			else
				_parom.waiting_employees[_parom.m_waiting++ + _parom.a_waiting] = employee; 
		}	
		if (employee.company == apple.company) { 
			if (_parom.number_of_a_on_parom < n && _parom.number_of_m_on_parom <= n || 
			_parom.number_of_a_on_parom >= n && !_parom.number_of_m_on_parom && _parom.number_of_a_on_parom < _parom.capacity)
				_parom.on_parom_employees[_parom.number_of_m_on_parom + _parom.number_of_a_on_parom++] = employee;
			else
				_parom.waiting_employees[_parom.m_waiting + _parom.a_waiting++] = employee;
		}
		print_time();
        printf("Сотрудник %s № %d прибыл на переправу.\n ", employee.company, employee.number);
		if (!_parom.on_theWay) {
			printf("На пароме %d сотрудников %s и ", _parom.number_of_m_on_parom, microsoft.company);
			printf("%d сотрудников %s.\n ", _parom.number_of_a_on_parom, apple.company);
		}
		printf("Ожидает %d сотрудников %s. ", _parom.m_waiting, microsoft.company);
		printf("Ожидает %d сотрудников %s.\n\n", _parom.a_waiting, apple.company);
		pthread_mutex_unlock(&parom_mutex);
    }
    pthread_exit(NULL);
}

void *parom() {
	srand(time(0));
	while(1) {
		if (_parom.number_of_m_on_parom + _parom.number_of_a_on_parom == _parom.capacity) { 
			Employees employee = _parom.on_parom_employees[rand() % (_parom.number_of_m_on_parom + _parom.number_of_a_on_parom)];
			pthread_mutex_lock(&parom_mutex);
			print_time();
			_parom.on_theWay=1;
			printf("Сотрудник %s под номером %d сказал \"Стартуем!\"\n\n", employee.company, employee.number);
			pthread_mutex_unlock(&parom_mutex);
			
			sleep(rand() % 10 + 10); 
			pthread_mutex_lock(&parom_mutex); 
			_parom.on_theWay=0;
			_parom.arrived = 1;
			_parom.number_of_m_on_parom = 0;
			_parom.number_of_a_on_parom = 0;
			for (int i = 0; i < _parom.capacity; i++) { 
				Employees employee = _parom.waiting_employees[i];
				if (employee.number) {
					if (employee.company == microsoft.company) {
						if (_parom.number_of_m_on_parom < n && _parom.number_of_a_on_parom <= n || 
			_parom.number_of_m_on_parom >= n && !_parom.number_of_a_on_parom && _parom.number_of_m_on_parom < _parom.capacity) {
							_parom.m_waiting--;
							_parom.number_of_m_on_parom++;
							_parom.on_parom_employees[i] = employee;
						}
						else
							_parom.employees_in_queue[_parom.queue_count++] = employee;
					}
						
					if (employee.company == apple.company) {
						if (_parom.number_of_a_on_parom < n && _parom.number_of_m_on_parom <= n || 
			_parom.number_of_a_on_parom >= n && !_parom.number_of_m_on_parom && _parom.number_of_a_on_parom < _parom.capacity) {
							_parom.a_waiting--;
							_parom.number_of_a_on_parom++;
							_parom.on_parom_employees[i] = employee;
						}
						else
							_parom.employees_in_queue[_parom.queue_count++] = employee;
					}
				}
				_parom.waiting_employees[i].company = NULL;
				_parom.waiting_employees[i].number = 0;
			}
			
			for (int i = 0; i < _parom.queue_count; i++) {
				_parom.waiting_employees[i] = _parom.employees_in_queue[i];
				_parom.employees_in_queue[i].company = NULL;
				_parom.employees_in_queue[i].number = 0;
			} 
			_parom.queue_count = 0;
			
			print_time();
			printf("Паром пришел обратно. ");
			printf("На пароме %d сотрудников %s и ", _parom.number_of_m_on_parom, microsoft.company);
			printf("%d сотрудников %s.\n ", _parom.number_of_a_on_parom, apple.company);
			printf("Ожидает %d сотрудников %s. ", _parom.m_waiting, microsoft.company);
			printf("Ожидает %d сотрудников %s.\n\n",_parom.a_waiting, apple.company);
			_parom.arrived = 0;
			pthread_cond_signal(&parom_available);
			pthread_mutex_unlock(&parom_mutex);
		}
	}
	pthread_exit(NULL);
}

int main(int argc, char **argv) {
	_parom.capacity = n*2;
	_parom.on_parom_employees = (Employees*)calloc(_parom.capacity, sizeof(Employees));
	_parom.waiting_employees = (Employees*)calloc(_parom.capacity, sizeof(Employees));
	_parom.employees_in_queue = (Employees*)calloc(_parom.capacity, sizeof(Employees));
	queue.company = NULL;
	microsoft.company = "мелкомягких";
	apple.company = "яблочников";
	pthread_t generate_thread, fill_parom_thread, parom_thread; 
    pthread_create(&generate_thread, NULL, generate, NULL);
    pthread_create(&fill_parom_thread, NULL, fill_parom, NULL);
	pthread_create(&parom_thread, NULL, parom, NULL);
	pthread_join(generate_thread, NULL);
	pthread_join(fill_parom_thread, NULL);
	pthread_join(parom_thread, NULL);
	return 0;
}
