#include "pool.h"
#include <queue>
#include <pthread.h>

Task::Task() {
}

Task::~Task() {
}

ThreadPool::ThreadPool(int num_threads) {
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
}

void ThreadPool::WaitForTask(const std::string &name) {
}

void ThreadPool::Stop() {
}
