
*c++ 实现线程池*

submit函数对传进来的函数以及参数通过std::bind绑定之后包装，最终包装成
std::function<void()>类型，并存储在work_queue中，等待空闲线程的执行
并返回future用于获取提交函数的返回值
线程池的运行逻辑部分全部在 线程池的构造函数中实现

*复刻自：https://github.com/progschj/ThreadPool*
