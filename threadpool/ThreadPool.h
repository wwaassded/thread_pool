#pragma once

#include "safeque.h"



namespace what::threadpool{

    class ThreadPool {

    public:

///delete掉的默认函数

        ThreadPool() = delete;
        ThreadPool(const ThreadPool &) = delete;
        ThreadPool(ThreadPool &&) = delete;
        ThreadPool &operator=(const ThreadPool &) = delete;
        ThreadPool &operator=(ThreadPool &&) = delete;


        /// 线程池的析构函数以及构造函数

        explicit ThreadPool(size_t thread_num = thread::hardware_concurrency());
        ~ThreadPool();


        /// 线程池的提交接口

        template<class Fn, class... ARGS>
        auto submit(Fn &&func,ARGS &&...args) -> future<decltype(func(args...))> {
//      decltype(auto) submit(Fn &&func,ARGS &&...args) {   //需要 编译器 c++ 14 的支持

            /// future内应该持有的类型，也就是函数func应该返回的类型, 以下两种写法有什么区别呢？？？

            using return_type = decltype(func(args...));
            using another_return_type = typename std::result_of<Fn(ARGS...)>::type;

            std::function<return_type ()> _func = std::bind(std::forward<Fn>(func),std::forward<ARGS>(args)...);

            auto task_ptr = std::make_shared<std::packaged_task<return_type ()>>(_func);

            /// 应该判断一下，线程池是否还在运行？？？

            {
                unique_lock<mutex> tmp_mutex(this->m_mutex);

                if(!this->is_running) {
                    throw std::runtime_error("thread pool is not running now!\n");
                }

                work_queue.enqueue([task_ptr]() {
                    (*task_ptr)();
                });

            }

            ///应该唤醒一个等待中的线程,   类似生产者消费者？？？

            this->conditionVariable.notify_one();

            return task_ptr->get_future();
        }

    private:
        bool is_running { true };

        // 线程池中所持有的线程数
        unsigned long thread_number;

        // 持有所有的线程池
        vector<thread> worker;

        // 任务队列 TODO:: 优化任务队列的效率，考虑无锁队列
        SafeQue<function<void()>> work_queue;

        //同步所需的🔒与条件变量
        condition_variable conditionVariable;
        mutex m_mutex;
    };

}