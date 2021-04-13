// pool of pthreads
#ifndef __PTPOOL_H__
#define __PTPOOL_H__

#include <pthread.h>
#include <stddef.h>
#include <unistd.h>
#include <stdlib.h>
struct pt_pool;
typedef struct pt_pool ptpool_t;

ptpool_t *ptpool_create(size_t num, size_t qs);
void ptpool_destroy(ptpool_t *p);

void ptpool_add_work(ptpool_t *p, void *(*func)(void*), void *arg);
void ptpool_wait(ptpool_t *p);

void *worker(void *job_args);
#endif
