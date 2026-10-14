#ifndef THREADS_HPP
#define THREADS_HPP

#ifdef __cplusplus
extern "C" {
#endif

#include "tx_api.h" 

// 1. 声明信号量
extern TX_SEMAPHORE imu_semaphore;

// 2. 声明线程控制块和栈
extern TX_THREAD imu_thread;
extern uint8_t imu_thread_stack[2048];

extern TX_THREAD control_thread;
extern uint8_t control_thread_stack[2048];

extern TX_THREAD alive_thread;
extern uint8_t alive_thread_stack[1024];

// 3. 声明入口函数
void imu_thread_entry(ULONG thread_input);
void control_thread_entry(ULONG thread_input);
void alive_thread_entry(ULONG thread_input);

#ifdef __cplusplus
}
#endif

#endif