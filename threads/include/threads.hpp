#ifndef THREADS_HPP
#define THREADS_HPP

#include "tx_api.h" // 假设你的 ThreadX 头文件叫这个，如果不是，换成你工程里的名字

// 1. 声明信号量（这就是作业要求的通信工具）
extern TX_SEMAPHORE imu_semaphore; 
extern TX_SEMAPHORE control_semaphore;

// 2. 声明线程函数入口（告诉系统，这几个函数会出现在别的 .cpp 文件里）
void alive_thread_entry(ULONG thread_input);
void imu_thread_entry(ULONG thread_input);
void control_thread_entry(ULONG thread_input);

#endif // THREADS_HPP