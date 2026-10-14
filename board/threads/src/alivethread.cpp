
#include "main.h"
#include "threads.hpp"
// 在 controlthread.cpp 的顶部
TX_THREAD alive_thread;
uint8_t alive_thread_stack[1024]; // 分配 2KB 给 control
void alive_thread_entry(ULONG thread_input) {
    while(1) {
        // 监控逻辑：如果超时没等到信号量，就报警
        //check_alive_status();
        
        // 睡 200 个 tick（比如 200ms，监控不需要跑那么快）
        tx_thread_sleep(200); 
    }
}