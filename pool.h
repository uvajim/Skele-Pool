#ifndef POOL_H_
#include <string>
#include <queue>
#include <pthread.h>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <set>

class Task {
public:
    Task();
    virtual ~Task();

    virtual void Run() = 0;  // implemented by subclass
};

class ThreadPool {
public:
    ThreadPool(int num_threads);

    // Submit a task with a particular name.
    void SubmitTask(const std::string &name, Task *task);
 
    // Wait for a task by name, if it hasn't been waited for yet. Only returns after the task is completed.
    void WaitForTask(const std::string &name);

    void Run();

    // Stop all threads. All tasks must have been waited for before calling this.
    // You may assume that SubmitTask() is not caled after this is called.
    void Stop();

    std::queue<Task*> q;

    std::queue<int> thread_q;
    //the set that will contain all completed _tasks
    std::set<Task*> completed_tasks;
    //keeps the name as a key, and the TaskID as a value
    std::unordered_map<std::string, Task*> names;  
    //creates the thread pool
     pthread_t *pool;
    //locks
    //the lock for an empty queue
    std::condition_variable is_empty;
    //a simple lock for the producer
    std::mutex prod_lk;
    //a simple lock for the consumer
    std::mutex consumer_lk;

};
#endif
