#include "pool.h"
#include <queue>
#include <pthread.h>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <set>

struct threadParam{
    int y_start; 
    int isFirstThread;
    int y_end;
    LifeBoard state;
    LifeBoard next_state;
    pthread_barrier_t barrier;
};

Task::Task() {
}

Task::~Task() {
}

ThreadPool::ThreadPool(int num_threads) {
    //creates the thread pool
    pool = new pthread_t[num_threads];
    for (int i = 0; i<num_threads; ++i) {
        pool[i] = i;
        task_q.append(i);
        //create pthread

        //make void* for class variables

        pthread_t_create(currThread, NULL, &RunTask, (void*) NULL);
    }
}

void ThreadPool::SubmitTask(const std::string &name, Task* task) {
    prod_lk.lock();
    q.append(task);
    names[name] = task;
    prod_lk.unlock();
    //Run()
}

void* RunTask(void* v){
    consumer_lk.lock();
    Task task = q.pop();
    pthread_t* currThread = pool.pop();
    consumer_lk.unlock();

    completed_tasks.insert(task);
    //pthread_join?
}

//wait for task "name" to finish
void ThreadPool::WaitForTask(const std::string &name) {
    consumer_lk.lock();
    while(q.empty){
        is_empty.Wait()
    }
}

void ThreadPool::Stop() {
    //signal to threads that they no longer have to wait (another condition var)
    //call pthread_join
}
