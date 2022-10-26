#include "pool.h"
#include <queue>
#include <pthread.h>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <set>

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
    }
}

void ThreadPool::SubmitTask(const std::string &name, Task* task) {
    prod_lk.lock();
    q.append(task);
    names[name] = task;
    prod_lk.unlock();
    Run()
}

void Run(){
    consumer_lk.lock();
    Task task = q.pop()
    consumer_lk.unlock();
    pthread_t* currThread = pool.pop();
    pthread_t_create(currThread, NULL, task->Run, NULL);
    completed_tasks.insert(task);
}

void ThreadPool::WaitForTask(const std::string &name) {

}

void ThreadPool::Stop() {
}
