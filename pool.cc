#include "pool.h"
#include <queue>
#include <pthread.h>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <set>

struct threadParam{
    //all variables in struuct should point to class objects
    std::queue<Task*> *q;
    //the lock for an empty queue
    std::condition_variable *is_empty;
    //a simple lock for the producer
    std::mutex *prod_lk;
    //a simple lock for the consumer
    std::mutex *consumer_lk;
    //the set that will contain all completed _tasks
    std::set<Task*> *completed_tasks;

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


    //use threads condvar
    while(!args.stopped){
        args.consumer_lk->lock();
        while(args.q->empty()){
            //use thread_empty condvar here

        }
        //get first task in queue
        Task* task = args.q->front();
        args.q->pop();
        args.consumer_lk->unlock();
        //run function
        task->Run();
        //put on completed thread queue
        args.prod_lk->lock();
        args.completed_tasks->insert(task);
        args.prod_lk->unlock();
    }

    return NULL;
}

ThreadPool::ThreadPool(int num_threads) {
    //creates the thread pool
    pool = new pthread_t[num_threads];
    stopped = false;
    //fill threadParam
    threadParam args;
    args.q = &q;
    args.is_empty = &is_empty;
    args.prod_lk = &prod_lk;
    args.consumer_lk = &consumer_lk;
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
    names[name] = task;
    prod_lk.unlock();
    //signal condition variable thread_empty
}



//wait for task "name" to finish
void ThreadPool::WaitForTask(const std::string &name) {
    Task* task = names[name];
    wait_lk.lock();
    while(completed_tasks.find(task)==completed_tasks.end()){
        // infinite loop
    }
    wait_lk.unlock();
}

void ThreadPool::Stop() {
    //signal to threads that they no longer have to wait (another condition var)
    stopped = true;
    //call pthread_join
    /*
    for (int thread = 0; thread < threads; ++thread){
        pthread_join(pool[thread],NULL);
    }
    */
    
    //deallocate memory for everything
}
