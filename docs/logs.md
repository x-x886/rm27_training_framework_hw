#PWM口有多个。该配那个。SPI和I2C也不会选，暂时选了IMU（BM1088）的SPI1和可配置的PWM2,I2C2
#如何完成CAN的驱动？https://blog.csdn.net/weixin_54448108/article/details/128570593
         1. 重新开了中断
         2.16进制转二进制易读
         3.那么收到的信息应该分发给谁 Motor_measure_t
#https://www.bilibili.com/video/BV1Yx4y1x7xY/?spm_id_from=333.337.search-card.all.click&vd_source=e6d0004d5540f23a14aeb61fd26b0d27
     pwm功能由TIM实现。
#ThreadX 多线程作业 - Debug 日志

一、 环境配置与工程搭建

1. CubeMX 配置：
   · 启用了 Azure RTOS ThreadX 组件。
   · 关键步骤：将 HAL 库基准时钟源从 SysTick 修改为 TIM2，防止 HAL 库与 ThreadX 内核抢占 SysTick 导致调度冲突。
2. 工程化代码结构：
   · 建立 threads/include/threads.hpp 与 threads/src/*.cpp 分离目录，实现线程与业务逻辑的模块化封装。
   · 配置 CMakeLists.txt：
     · 添加头文件搜索路径 threads/include 以及 ThreadX 内核路径。
     · 将 imuthread.cpp、controlthread.cpp、alivethread.cpp 加入 target_sources 编译列表。

二、 核心问题排查记录（Debug 踩坑）

问题 1：编译报错“未定义标识符 TX_SEMAPHORE / TX_THREAD”

· 现象：VS Code 智能感知能按 F12 跳转到定义，但 GCC 编译报错未定义。
· 排查过程：发现是“C/C++ 混合编程”引起的 Name Mangling 冲突。在 C++ 文件中直接包含纯 C 语言的 tx_api.h 时，编译器按 C++ 规则解析，导致底层结构体识别失败。
· 解决方案：在 threads.hpp 中使用 extern "C" { ... } 包裹 #include "tx_api.h"，强制 C++ 编译按 C 语言规则链接。
· 教训：VS Code 红线仅作参考，编译器的真实反馈才是最终结果。

问题 2：链接报错“undefined reference to 'imu_thread'”

· 现象：CMake 编译通过，但链接阶段报错找不到线程控制块和线程栈。
· 排查过程：main.c（C文件）与 threads/*.cpp（C++文件）对变量的命名规则不一致，导致链接器无法匹配。
· 解决方案：在 main.c 的 User Code 区域，采用“暴力声明”法，直接手动添加 extern 声明（包含 TX_THREAD、TX_SEMAPHORE 等），绕过 C/C++ 头文件包含的编译器细节，强制链接。

问题 3：最后一次编译失败

· 现象：提示 [build] ninja: build stopped: subcommand failed。
· 排查过程：确认是拼写错误。检查代码发现 TX_SEMAPHORE 拼写成了 TX_SEMAPHORE_，导致链接器找不到符号。
· 解决方案：修正拼写，随后编译成功，输出 [build] 生成已完成，退出代码为 0。

三、 当前进度与成果

· 成功建立基于 ThreadX 的三线程基础框架：imu、control、alive。
· 成功实现跨线程同步的基础通信机制（信号量 tx_semaphore_put/get）。
· 代码占用资源：RAM 9616 B (7.34%)，FLASH 24816 B (2.37%)。
· 下一步计划：将硬件外设驱动（I2C读取BMI088、CAN电机通信）填入线程业务逻辑，并完善 alive 线程的状态监控逻辑。
   
