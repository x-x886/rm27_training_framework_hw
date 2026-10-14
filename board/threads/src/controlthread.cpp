
#include "main.h"
#include "threads.hpp"
// 在 controlthread.cpp 的顶部
TX_THREAD control_thread;
uint8_t control_thread_stack[2048]; // 分配 2KB 给 control
void control_thread_entry(ULONG thread_input) {
    // 初始化电机或 PID 参数
    //motor_init();
    
    while(1) {
        // 等 IMU 发来的信号量（等不到我就一直睡）
        tx_semaphore_get(&imu_semaphore, TX_WAIT_FOREVER);
        
        // 拿到信号量了，说明数据来了，开始算 PID
        //calc_pid();
        
        // 睡 5 个 tick
        tx_thread_sleep(5); 
    }
}