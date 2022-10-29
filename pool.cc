#include "pool.h"
#include <queue>
#include <pthread.h>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <set>
#include <iostream>


struct threadParam{
    //all variables in struuct should point to class objects
    std::queue<Task*> *q;
    //the lock for an empty queue
    sem_t *is_empty;
    std::mutex *completed_tasks_lk;
    //a simple lock for the producer
    std::mutex *prod_lk;
    //a simple lock for the consumer
    std::mutex *consumer_lk;
    //the set that will contain all completed _tasks
    std::set<Task*> *completed_tasks;
    std::set<Task*> *running_tasks;
    pthread_cond_t *stop;
    pthread_mutex_t *stop_lk;
    pthread_cond_t *wait;
    pthread_mutex_t *wait_lk;
    bool *stopped;
};

Task::Task() {
}

Task::~Task() {
}

void* RunTask(void* v){
    //unpack pointer
    std::cout<<"running"<<std::endl;
    
    threadParam args = *((threadParam *)v);
    std::cout<<"running2"<<std::endl;
    //loop until stop is called

    //conditon variable set to do work infinitly
    while(pthread_mutex_trylock(args.stop_lk)){
        pthread_mutex_unlock(args.stop_lk);
        sem_trywait(args.is_empty);
        sem_wait(args.is_empty);
        //get first task in queue
        pthread_mutex_lock(args.wait_lk);
        args.consumer_lk->lock();
        Task* task = args.q->front();
        args.q->pop();
        args.consumer_lk->unlock();
        pthread_mutex_unlock(args.wait_lk);
        pthread_cond_broadcast(args.wait);

        
        //run function
        //do not run funtions if there is a wait call
        //while (!args.wait_lk->try_lock()){std::cout<<"infinite"<<std::endl;};
    
        task->Run();
        //put on completed thread queue
        args.completed_tasks_lk->lock();
        args.completed_tasks->insert(task);
        args.completed_tasks_lk->unlock();
        pthread_mutex_unlock(args.wait_lk);
        pthread_cond_broadcast(args.stop);

    }
    return NULL;
}

ThreadPool::ThreadPool(int num_threads) {
    //creates the thread pool
    pool = new pthread_t[num_threads];
    pthread_cond_init(&stop, NULL);
    pthread_cond_init(&wait, NULL);
    pthread_mutex_init(&wait_lk, NULL);
    pthread_mutex_init(&stop_lk, NULL);

    sem_init(&sem, 0, 0);
    //fill threadParam
    threadParam args;
    args.q = &q;
    args.completed_tasks_lk = &completed_tasks_lk;
    args.is_empty = &sem;
    args.prod_lk = &prod_lk;
    args.stop = &stop;
    args.stop_lk = &stop_lk;
    args.wait_lk = &wait_lk;
    args.wait  = &wait;
    args.consumer_lk = &consumer_lk;
    args.completed_tasks = &completed_tasks;


    for (int i = 0; i<num_threads; ++i) {
        pool[i] = i;
        //create pthread
        //make void* for class variables
        pthread_create(&pool[i], NULL, &RunTask, (void*) &args);
    }
}

void ThreadPool::SubmitTask(const std::string &name, Task* task) {
    prod_lk.lock();
    q.push(task);
    sem_post(&sem);
    names[name] = task;
    prod_lk.unlock();
    //signal condition variable thread_empty
}



//wait for task "name" to finish
void ThreadPool::WaitForTask(const std::string &name) {
    pthread_cond_wait(&wait, &wait_lk);
    pthread_mutex_lock(&wait_lk);
    Task* task = names[name];
    while(completed_tasks.find(task)==completed_tasks.end()){
        pthread_cond_wait(&wait, &wait_lk);
    }

    prod_lk.lock();
    names.erase(name);
    prod_lk.unlock();
    
    completed_tasks_lk.lock();
    completed_tasks.erase(task);
    completed_tasks_lk.unlock();
    pthread_mutex_unlock(&wait_lk);
}

void ThreadPool::Stop() {
    pthread_cond_wait(&stop, &stop_lk);
    //signal to threads that they no longer have to wait (another condition var)

    for (int i=0; i < sizeof(pool)/sizeof(int); ++i){
        pthread_join(pool[i], NULL);
    }
}
