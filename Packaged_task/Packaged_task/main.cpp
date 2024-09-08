//#include <bits/stdc++.h>
#include <iostream>
#include <deque>
#include <future>
#include <mutex>

// Factorial function
int factorial(int N)
{
    int res = 1;
    for (int i = N; i > 1; i--) {
        res *= i;
    }

    std::cout << "Result is = " << res << "\n";
    return res;
}

// packaged task
std::deque<std::packaged_task<int()> > task_q;
std::mutex mu;
std::condition_variable cond;

void thread1()
{
    // packaged task
    std::packaged_task<int()> task;
    {
        std::unique_lock<std::mutex> locker(mu);
        cond.wait(locker, []() { return !task_q.empty(); });
        task = move(task_q.front());
        task_q.pop_front();
    }
    task();
}

// Driver Code
int main()
{
    std::thread t1(thread1);

    // Create a packaged_task<> that
    // encapsulated the callback i.e. a function
    std::packaged_task<int()> task(std::bind(factorial, 5));

    // Fetch the associated future<>
    // from packaged_task<>
    std::future<int> future = task.get_future();
    {
        std::lock_guard<std::mutex> locker(mu);
        task_q.push_back(move(task));
    }
    cond.notify_one();

    // Fetch the result of packaged_task<>
    std::cout << future.get();

    // Join the thread. Its blocking and
    // returns when thread is finished.
    t1.join();

    return 0;
}