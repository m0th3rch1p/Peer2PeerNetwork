#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ThreadPool.h"

void addJob(struct ThreadJob job, struct ThreadPool* threadPool);
void* thread_handler(void* args);

struct ThreadPool* thread_pool_constructor(int noThreads) {
	struct ThreadPool* threadPool = (struct ThreadPool*)calloc(1, sizeof(struct ThreadPool));
	if (!threadPool) {
		fprintf(stderr, "[-] error allocating thread pool memory.\n");
		return NULL;
	}
	
	threadPool->active = 1;
	threadPool->noOfThreads = noThreads;
	threadPool->jobs = queue_constructor(strcmp);
	threadPool->addJob = addJob;
#ifdef _WIN32
	threadPool->threadIds = (DWORD*)calloc(noThreads, sizeof(DWORD));
	if (!threadPool->threadIds) {
		fprintf(stderr, "[-] error allocating memory for thread ids\n");
		free(threadPool);
		return NULL;
	}
	threadPool->pool = (HANDLE*)calloc(noThreads, sizeof(HANDLE));
	InitializeConditionVariable(&threadPool->lock);
	InitializeCriticalSection(&threadPool->signal);
#else
	threadPool->pool = (pthread_t*)calloc(noThreads, sizeof(pthread_t));
	if (pthread_mutex_init(&threadPool->lock)) {
		fprintf(stderr, "[-] error initializing mutex\n");
		if (threadPool->pool) free(threadPool->pool);
		free(threadPool);;
		return NULL;
	}
#endif // _WIN32
	if (!threadPool->pool) {
		fprintf(stderr, "[-] error allocating pool memory.\n");
		free(threadPool);
		return NULL;
	}

	for (int i = 0; i < noThreads; i++)
	{
#ifdef _WIN32
		threadPool->pool[i] = CreateThread(NULL, NULL, thread_handler, (void*)threadPool, NULL, &threadPool->threadIds[i]);
		if (!threadPool->pool[i]) {
			fprintf(stderr, "[-] error creating threads\n");
			free(threadPool->pool);
			free(threadPool);
			exit(1);
		}

		printf("Thread created successfully with id: %lu\n", threadPool->threadIds[i]);
#else
		if (pthread_create(&threadPool->pool[i], NULL, thread_handler, NULL)) {
			fprintf(stderr, "[-] error creating threads\n");
			free(threadPool->pool);
			free(threadPool);
			exit(1);
		}
#endif
	}

	return threadPool;
}

void thread_pool_deconstructor(struct ThreadPool* threadPool) {
#ifdef _WIN32
	WaitForMultipleObjects(threadPool->noOfThreads, threadPool->pool, TRUE, INFINITE);
	free(threadPool->threadIds);

	printf("[+] all threads done running.\n");
#endif

	for (int i = 0; i < threadPool->noOfThreads; i++)
	{
#ifdef _WIN32
		CloseHandle(threadPool->pool[i]);
#endif
	}
	free(threadPool->pool);
	free(threadPool);
}

struct ThreadJob thread_job_constructor(void* (*job)(void* args), void* args) {
	struct ThreadJob threadJob = { 0 };
	threadJob.job = job;
	threadJob.args = args;

	return threadJob;
}

void addJob(struct ThreadJob job, struct ThreadPool* threadPool) {
#ifdef _WIN32
	EnterCriticalSection(&threadPool->signal);
#else
	pthread_lock(&threadPool->lock);
#endif
	threadPool->jobs->insert(&job, sizeof(job), threadPool->jobs);

#ifdef _WIN32
	LeaveCriticalSection(&threadPool->signal);
	WakeConditionVariable(&threadPool->lock);
#else
	pthread_unlock(&threadPool->lock);
#endif
}

void* thread_handler(void* args) {
	struct ThreadPool* threadPool = (struct ThreadPool*)args;
	while (threadPool->active) {
#ifdef _WIN32
		EnterCriticalSection(&threadPool->signal);
		if (!SleepConditionVariableCS(&threadPool->lock, &threadPool->signal, INFINITE)) {
			fprintf(stderr, "[-] error sleeping condition: %d\n", GetLastError());
			exit(9);
		}
#else

#endif
		struct ThreadJob* pThreadJob = (struct ThreadJob*)threadPool->jobs->peek(threadPool->jobs);
		struct ThreadJob threadJob = { 0 };
		if (pThreadJob && pThreadJob->job) {
			threadJob = *pThreadJob;
			threadPool->jobs->pop(threadPool->jobs);
		}
#ifdef _WIN32
		LeaveCriticalSection(&threadPool->signal);
#else
#endif
		if (threadJob.job) {
			threadJob.job(threadJob.args);
		}
	}
	return NULL;
}