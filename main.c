#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

pthread_mutex_t mutex[5];
pthread_mutex_t pmutex;
pthread_cond_t cond[5];
enum {THINKING, HUNGRY, EATING} state[5];
int forks[5] = {0, 0, 0, 0, 0};

void *run(void *);
void pickup_forks(int);
void putdown_forks(int);
void test(int);

int main(int argc, char *argv[]) {
	srand(0);
	pthread_t tid[5];
	pthread_attr_t attr[5];
	int idx[5] = {0, 1, 2, 3, 4};

	/* inititialize threads */
	pthread_mutex_init(&pmutex, NULL);
	for (int i = 0; i < 5; ++i) {
		pthread_attr_init(&attr[i]);
		pthread_create(&tid[i], &attr[i], run, &idx[i]);
		pthread_mutex_init(&mutex[i], NULL);
		state[i] = THINKING;
	}
	
	for (int i = 0; i < 5; ++i)
			pthread_join(tid[i], NULL);

	return 0;
}

void *run(void *p)
{
	int i = *((int*) p);
//	printf("Thread %d is running", i);
	while(1) {
		// int dur = (rand() % 3) + 1;
		int dur = 1;
		pthread_mutex_lock(&pmutex);
		printf("Philo %d is attempting to eat\n", i);
		fflush(stdout);
		pthread_mutex_unlock(&pmutex);

		pickup_forks(i);

		pthread_mutex_lock(&pmutex);
		printf("Philo %d is eating\n", i);
		fflush(stdout);
		pthread_mutex_unlock(&pmutex);

		sleep(dur);

		putdown_forks(i);

		pthread_mutex_lock(&pmutex);
		printf("Philo %d is thinking\n", i);
		fflush(stdout);
		pthread_mutex_unlock(&pmutex);

		sleep(dur);
	}
}

void pickup_forks(int i)
{
	int r = i;
	int l = (i + 4) % 5;

	pthread_mutex_lock(&mutex[r]);
	while (forks[l] == 1) /* leave the fork if other one not available */
		pthread_cond_wait(&(cond[r]), (&mutex[r]));
	pthread_mutex_lock(&mutex[l]);
	forks[l] = 1;
	forks[r] = 1;
}

void putdown_forks(int i)
{
	int r = i;
	int l = (i + 4) % 5;

	pthread_cond_signal(&cond[r]);
	forks[l] = 0;
	forks[r] = 0;
	pthread_mutex_unlock(&mutex[l]);
	pthread_mutex_unlock(&mutex[r]);
}
