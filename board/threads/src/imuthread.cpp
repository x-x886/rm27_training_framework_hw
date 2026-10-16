#include "main.h"
#include "threads.hpp"
#include "../../modules/include/BMI088.hpp"

BMI088::cBMI088 imu

// 在 imuthread.cpp 的顶部
TX_SEMAPHORE imu_semaphore;
TX_THREAD imu_thread;
uint8_t imu_thread_stack[2048];   // 分配 2KB 给 imu（因为 IMU 驱动可能用得多）
void imu_thread_entry(ULONG thread_input) {
    // 初始化 比如 BMI088 初始化）
        imu.Config();
        imu.VerifyAccChipID();
        imu.Calibrate();
    
    while(1) {
         // 1 读三轴加速度和温度
        imu.ReadAccData(&imu.acc_data);
        imu.ReadAccTemperature(&imu.temp_data); // 假设你有 temp_data 变量
        
        // 2 读取三轴角速度
        imu.ReadGyroData(&imu.gyro_data);
        
        // 3 数据有效性检查（识别通信失败）
        imu.VerifyAccData();
        imu.VerifyGyroData();
        
        // 4 温度控制（可选，如果要交作业中的温度控制）
        // imu.TemperatureControl(40.0f); // 目标温度 40 度

        // 5 把数据丢给全局变量或发送到队列，供其他线程使用
        // ... (这部分视你的项目需求而定)

        // ==========================================
        // 3. 核心：按需休眠！
        // ==========================================
        // 作业要求：“含适当的休眠或同步”
        // 100Hz 的 IMU 数据更新率，意味着 10ms 读一次
        // ThreadX 的 tick 通常是 1ms，所以 sleep 10 个 tick
        tx_thread_sleep(10); 

    }
}
