#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "Queue.h"
#ifdef _WIN32
#include <Windows.h>
#else
#include <pthread.h>
#endif

struct ThreadJob {
	void* (*job)(void *args);
	void* args;
};

struct ThreadPool {
	int active;
	int noOfThreads;
	struct Queue* jobs;

#ifdef _WIN32
	HANDLE* pool;
	DWORD* threadIds;

	/// <summary>
	/// Synchronizarion:
	/// Use Critical Sections instead of using mutexes for better performance
	/// change to mutexes if decided to use multiple processes instead of threads
	/// </summary>
	CONDITION_VARIABLE lock;
	CRITICAL_SECTION signal;
	
	//HANDLE lock; //Uncomment if using mutexes instead. remember to initialize it in constructor
#else 
	pthread_t* pool;
	pthread_mutex_t lock;
#endif

	void (*addJob)(struct ThreadJob job, struct ThreadPool* threadPool);
};

struct ThreadPool* thread_pool_constructor(int noThreads);
void thread_pool_deconstructor(struct ThreadPool* pool);

struct ThreadJob thread_job_constructor(void* (*job)(void *args), void* args);
#endif