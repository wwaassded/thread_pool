//
// Created by crasher on 2023/9/11.
//

#include "ThreadPool.h"
namespace what::threadpool {

    ThreadPool::ThreadPool(size_t thread_num):thread_number(thread_num) {

        PANIC(this->thread_number <= thread::hardware_concurrency(),"The current machine doesn't support this many threads!\n");

        // 创建线程,并不销毁而是不断地执行， 找任务 -> 处理任务 -> 休眠 -> 找任务 -> ...
        for(int i=0; i<thread_number; ++i)
            this->worker.emplace_back(

                    [this] {

                        while(true) { // loop forever

                            optional<function<void()>> task;

                            {
                                unique_lock<mutex> tmp_mutex(this->m_mutex);

                                this->conditionVariable.wait(tmp_mutex,[this] {return !this->is_running || !work_queue.empty();});

                                if(!this->is_running && work_queue.empty())
                                    return ;
                                task = std::move(this->work_queue.dequeue());
                            }

                            if(task.has_value())
                                (task.value()) ();
                        }

                    }

                    );
    }


    ThreadPool::~ThreadPool() {

        {
            unique_lock<mutex>  tmp_lock (m_mutex);
            this->is_running = false;
        }

        this->conditionVariable.notify_all();

        for(auto &item : worker)
            if(item.joinable())
                item.join();

    }
}

