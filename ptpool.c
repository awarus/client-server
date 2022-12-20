// simple pthread pool

#include "ptpool.h"

struct pool_job {
  void *(*routine)(void *);
  void *arg;
};

struct pt_pool {
	pthread_t *worker_threads;
	struct pool_job	*job_queue;
	int head, tail;
	size_t queue_size;
	size_t max_threads;
	size_t scheduled;
	pthread_mutex_t  mut;
	pthread_cond_t   to_work;
	pthread_cond_t   is_done;
};

struct job_args {
  ptpool_t* pool;
  struct pool_job td;
};

void *worker(void *pool_arg) {
	ptpool_t *p = (ptpool_t*)pool_arg;

	//printf("new worker is ready...\n");
	while(1) {
		struct pool_job job;
		pthread_mutex_lock(&p->mut);

		while(p->head == p->tail)
			pthread_cond_wait(&p->to_work, &p->mut);

		job = p->job_queue[p->head % p->queue_size];
		p->head++;
		p->scheduled++;

		pthread_mutex_unlock(&p->mut);

		job.routine(job.arg);
		pthread_mutex_lock(&p->mut);

		p->scheduled--;
		if(p->scheduled == 0)
			pthread_cond_signal(&p->is_done);

		pthread_mutex_unlock(&p->mut);
	}

	return NULL;
}

ptpool_t *ptpool_create(size_t n, size_t qs) {
	ptpool_t *p;
	size_t i;

	p = malloc(sizeof(ptpool_t));

	p->max_threads = n;
	p->scheduled = 0;
	p->head = 0;
	p->tail = 0;
	p->queue_size = qs;

	p->worker_threads = malloc(sizeof(pthread_t) * n);
	p->job_queue = malloc(sizeof(struct pool_job) * qs);

	pthread_mutex_init(&p->mut, NULL);
	pthread_cond_init(&p->to_work, NULL);
	pthread_cond_init(&p->is_done, NULL);

	for (i = 0; i < n; i++) {
		pthread_create(&p->worker_threads[i], NULL, worker, p);
	}

	return p;
}


void ptpool_destroy(ptpool_t *p) {
	ptpool_wait(p);

	for(int i = 0; i < p->max_threads; i++) {
		pthread_detach(p->worker_threads[i]);
	}

	free(p->worker_threads);
	free(p->job_queue);
	free(p);
}

void ptpool_add_work(ptpool_t *p, void *(*func)(void*), void *arg) {
	pthread_mutex_lock(&p->mut);

	struct pool_job job;

	if(p->head == p->tail) {
		pthread_cond_broadcast(&p->to_work);
	}

	job.routine = func;
	job.arg = arg;


	p->job_queue[p->tail % p->queue_size] = job;
	p->tail++;

	pthread_mutex_unlock(&p->mut);

	//printf("%d jobs is scheduled\n", p->tail);
}

void ptpool_wait(ptpool_t *p) {
	pthread_mutex_lock(&p->mut);

	while(p->scheduled > 0) {
		pthread_cond_wait(&p->is_done, &p->mut);
	}

	pthread_mutex_unlock(&p->mut);
}

