export module threadpool;

import <thread>;
import <functional>;
import <condition_variable>;
import <queue>;

export class ThreadPool
{
   public:
    ThreadPool(size_t numThreads);
    ~ThreadPool();

    void enqueue(const std::function<void()>& task);
    void wait();  // Wait for all tasks to finish

   private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    std::condition_variable finishedCondition;
    size_t activeTasks = 0;
    bool stop          = false;
};

ThreadPool::ThreadPool(size_t numThreads)
{
    for (size_t i = 0; i < numThreads; ++i)
    {
        workers.emplace_back([this] {
            while (true)
            {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(queueMutex);
                    condition.wait(lock, [this] { return stop || !tasks.empty(); });
                    if (stop && tasks.empty())
                        return;

                    task = std::move(tasks.front());
                    tasks.pop();
                    ++activeTasks;
                }

                // Execute the task
                task();

                {
                    std::unique_lock<std::mutex> lock(queueMutex);
                    --activeTasks;
                    if (tasks.empty() && activeTasks == 0)
                    {
                        finishedCondition.notify_all();
                    }
                }
            }
        });
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread& worker : workers)
    {
        worker.join();
    }
}

void
ThreadPool::enqueue(const std::function<void()>& task)
{
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        tasks.emplace(task);
    }
    condition.notify_one();
}

void
ThreadPool::wait()
{
    std::unique_lock<std::mutex> lock(queueMutex);
    finishedCondition.wait(lock, [this] { return tasks.empty() && activeTasks == 0; });
}