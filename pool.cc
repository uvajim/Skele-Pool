#include "pool.h"
#include <queue>
#include <pthread.h>
#include <mutex>
#include <condition_variable>
#include <unordered_map>

Task::Task() {
}

Task::~Task() {
}

ThreadPool::ThreadPool(int num_threads) {
    //creates a conditional variable
    //init to false cuz queue should start off false
    std::condition_variable is_empty = true;
    std::mutex lock;
    //creates maps to keep track of the name
    unordered_map<std::string, Task> names;
    //creates the usuable threads
    pthread_t *pool = new pthread_t[num_threads];
    for (int i = 0; i<num_threads; ++i) {
        pool[i] = i;
    }

    //initializes the task queue
    std::queue<Task> task_queue;
    
    //keeps track of free threads
    std::queue<Task> free_threads;
}

void ThreadPool::SubmitTask(const std::string &name, Task* task) {

    task_queue.push(task);
    names[name] = task;
    is_empty.notify_all();
}

void ThreadPool::WaitForTask(const std::string &name) {
    lk(&lock);
    while(queue.empty()){
        is_empty.wait(lock);
    }
}

void ThreadPool::Stop() {
    if (free_threads.size() != pool.size()){
        delete(is_empty);
        delete(lock);
        delete(names);
        delete(pool);
        delete(task_queue);
        delete(free_threads);
    }
}
