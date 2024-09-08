//#include <bits/stdc++.h>
#include <iostream>
#include <deque>
#include <future>
#include <mutex>

using namespace std;

// Factorial function
int factorial(int N)
{
    int res = 1;
    for (int i = N; i > 1; i--) {
        res *= i;
    }

    cout << "Result is = " << res << "\n";
    return res;
}

// packaged task
deque<packaged_task<int()> > task_q;
mutex mu;
condition_variable cond;

void thread1()
{
    // packaged task
    packaged_task<int()> task;
    {
        unique_lock<mutex> locker(mu);
        cond.wait(locker, []() { return !task_q.empty(); });
        task = move(task_q.front());
        task_q.pop_front();
    }
    task();
}

// Driver Code
int main()
{
    thread thread1(thread1);

    // Create a packaged_task<> that
    // encapsulated the callback i.e. a function
    packaged_task<int()> task(bind(factorial, 5));

    // Fetch the associated future<>
    // from packaged_task<>
    future<int> future = task.get_future();
    {
        lock_guard<mutex> locker(mu);
        task_q.push_back(move(task));
    }
    cond.notify_one();

    // Fetch the result of packaged_task<>
    cout << future.get();

    // Join the thread. Its blocking and
    // returns when thread is finished.
    thread1.join();

    return 0;
}