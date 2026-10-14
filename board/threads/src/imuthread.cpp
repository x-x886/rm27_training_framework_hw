#include "main.h"
#include "threads.hpp"

// 在 imuthread.cpp 的顶部
TX_SEMAPHORE imu_semaphore;
TX_THREAD imu_thread;
uint8_t imu_thread_stack[2048];   // 分配 2KB 给 imu（因为 IMU 驱动可能用得多）
void imu_thread_entry(ULONG thread_input) {
    // 初始化 IMU（比如 BMI088 初始化）
    //imu_init();
    
    while(1) {
        // 读数据
        //read_imu_data();
        
        // 把数据存到全局变量，然后用信号量通知别人
        //tx_semaphore_put(&imu_semaphore); 
        
        // 睡 5 个 tick（比如 5ms），让出 CPU
        tx_thread_sleep(5); 
    }
}
