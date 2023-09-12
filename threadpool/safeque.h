#pragma once

#include <queue>
#include <mutex>
#include <optional>
#include <vector>
#include <thread>
#include <future>
#include <functional>
#include <exception>
#include <stdexcept>
#include <cassert>
#include <condition_variable>
#include <iostream>

#define PANIC(con,str)  do {                 \
     if(!(con)) {                              \
            std::cout << str << std::endl;   \
            assert(false);                   \
    }                                        \
}                                            \
while(0)                                     \

    namespace what::threadpool{

        using std::vector;
        using std::thread;
        using std::future;
        using std::queue;
        using std::mutex;
        using std::function;
        using std::optional;
        using std::unique_lock;
        using std::condition_variable;


/// 线程池的内部数据结构，只是简单的封装 queue，dequeue会导致出队列，并返回一个option

        template <class T>
        class SafeQue{
        public:

            bool empty() {
                return _que.empty();
            }

            size_t size() {
                return _que.size();
            }

            optional<T> dequeue() {
                if(empty())
                    return std::nullopt;
                auto it = std::move(_que.front());
                _que.pop();
                return it;
            }

            void enqueue(T &item) {
                _que.emplace(item);
            }

            void enqueue(T item) {
                _que.emplace(item);
            }

        private:
            queue<T> _que;
        };
    }
