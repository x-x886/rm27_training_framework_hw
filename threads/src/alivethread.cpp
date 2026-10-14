
#include "main.h"
#include "tx_api.h"
// 在 controlthread.cpp 的顶部
TX_THREAD control_thread;
uint8_t control_thread_stack[2048]; // 分配 2KB 给 control
void alive_thread_entry(ULONG thread_input) {
    while(1) {
        // 监控逻辑：如果超时没等到信号量，就报警
        check_alive_status();
        
        // 睡 200 个 tick（比如 200ms，监控不需要跑那么快）
        tx_thread_sleep(200); 
    }
}