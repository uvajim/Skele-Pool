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

    std::condition_variable *stop;


    std::unique_lock<std::mutex> *wait_lk;

    bool *stopped;
};

Task::Task() {
}

Task::~Task() {
}

void* RunTask(void* v){
    //unpack pointer
    threadParam args = *((threadParam *)v);
    //loop until stop is called

    //conditon variable set to do work infinitly
    std::cout<< args.stopped <<std::endl;
    while(!*args.stopped){
        
        sem_trywait(args.is_empty);
        if (*args.stopped){
            return NULL;
        }
        sem_wait(args.is_empty);
        //get first task in queue
        args.wait_lk->lock();
        args.consumer_lk->lock();
        Task* task = args.q->front();
        args.q->pop();
        args.wait_lk->unlock();
        args.consumer_lk->unlock();
        
        //run function
        //do not run funtions if there is a wait call
        //while (!args.wait_lk->try_lock()){std::cout<<"infinite"<<std::endl;};
        args.wait_lk->unlock();

        task->Run();
        //put on completed thread queue
        args.completed_tasks_lk->lock();
        args.completed_tasks->insert(task);
        args.completed_tasks_lk->unlock(); 
    }
    return NULL;
}

ThreadPool::ThreadPool(int num_threads) {
    //creates the thread pool
    pool = new pthread_t[num_threads];
    sem_init(&sem, 0, 0);
    //fill threadParam
    threadParam args;
    args.q = &q;
    args.completed_tasks_lk = &completed_tasks_lk;
    args.is_empty = &sem;
    args.prod_lk = &prod_lk;
    args.wait_lk = &wait_lk;
    args.stop = &stop;
    args.consumer_lk = &consumer_lk;
    args.running_tasks = &running_tasks;
    args.completed_tasks = &completed_tasks;
    args.stopped = &stopped;

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
    wait_lk.lock();
    Task* task = names[name];
    while(completed_tasks.find(task)==completed_tasks.end()){stop.wait(wait_lk);}

    prod_lk.lock();
    names.erase(name);
    prod_lk.unlock();
    
    completed_tasks_lk.lock();
    completed_tasks.erase(task);
    completed_tasks_lk.unlock();
    wait_lk.unlock();
}

void ThreadPool::Stop() {
    //signal to threads that they no longer have to wait (another condition var)
    stopped = true;

    for (int i=0; i < sizeof(pool)/sizeof(int); ++i){
        pthread_join(pool[i], NULL);
    }
}
