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
   
# BMI088 传感器驱动调试日志 (logs.md)

## 一、 工程环境搭建与编译
**日期**：2026-10-16
**调试目标**：让 `board/` 工程能成功编译，并为后续外设驱动做准备。
   - 在 `board/CMakeLists.txt` 中正确引入 `bsp/`、`libs/` 和 `modules/` 的源文件与头文件路径。
3. **编译结果**：工程编译成功，退出代码 0。
   - **Memory 记录**：RAM 使用 9616B (7.34%)，FLASH 使用 25092B (2.39%)。
   - 符合外设单独验证前的工程编译要求。

## 二、 BMI088 驱动底层 SPI 通信实现
**调试目标**：实现通过 SPI 协议读写 BMI088 寄存器，并处理片选与通信故障识别。

1. **片选与引脚配置**：
   - 查阅《RoboMaster 开发板 C 型用户手册》，确定硬件接线。
   - 加速度计 CS 引脚：PA4；陀螺仪 CS 引脚：PB0；SPI1_CLK：PB3；SPI1_MOSI：PA7；SPI1_MISO：PB4。
   - 在 `WriteReg` 与 `ReadReg` 中，根据 `cs` 入参，通过 `HAL_GPIO_WritePin` 拉低/拉高对应片选引脚，实现了硬件片选逻辑。

2. **`WriteReg` 逻辑**：
   - 拉低片选 -> 发送地址（最高位为0） -> 发送数据 -> 拉高片选。

3. **`ReadReg` 逻辑（关键踩坑点）**：
   - 拉低片选 -> 发送地址（最高位置1，即 `addr | 0x80`） -> **丢弃加速度计返回的第一个无效字节 (Dummy Byte)** -> 读取 `len` 个字节 -> 拉高片选。
   - **调试记录**：通过阅读 BMI088 数据手册发现，加速度计在接收读指令后，第一个字节是无效的，必须先读取并丢弃，否则数据会错位。而陀螺仪不需要丢弃字节。

4. **通信失败识别**：
   - 实现了 `VerifyAccChipID` 和 `VerifyGyroChipID`。
   - 加速度计 ID 寄存器地址 `0x00`，预期值 `0x1E`。
   - 陀螺仪 ID 寄存器地址 `0x00`，预期值 `0x0F`。
   - 若读取值不符，将 `self_test.ACC_CHIP_ID_ERR` 或 `self_test.GYRO_CHIP_ID_ERR` 置为 `true`，标记初始化失败。

## 三、 数据读取与单位换算
**调试目标**：读取三轴加速度、三轴角速度及温度，并转换为带物理单位的浮点数。

1. **三轴数据拼接（小端序）**：
   - BMI088 的 16 位数据采用小端模式（低位在前，高位在后）。
   - 拼接方式：`int16_t raw_x = (int16_t)((raw[1] << 8) | raw[0]);`，同理拼接 Y 轴与 Z 轴。

2. **单位换算（明确量程）**：
   - 在 `Config()` 中配置加速度计量程为 ±6G，对应换算系数为 `IMU_ACCEL_6G_SEN`，最终单位转换为 m/s²。
   - 配置陀螺仪量程为 ±2000dps，对应换算系数为 `IMU_GYRO_2000_SEN`，最终单位转换为 rad/s。
   - **零漂处理**：陀螺仪在 `ReadGyroData` 中减去 `Gyro_offset`（由 `Calibrate` 函数采样 4000 次计算得出），提高静态精度。

3. **温度读取**：
   - 温度数据在 MSB 的高 11 位，先读 2 字节，合并后右移 5 位。
   - 按公式计算：`*temp = raw_temp * TEMP_UNIT + TEMP_BIAS;`（TEMP_UNIT = 0.125，TEMP_BIAS = 23.0）。

4. **数据有效性检查**：
   - `VerifyAccData` 与 `VerifyGyroData` 中，读取三轴数据，若三轴全为 0，则判定 SPI 通信可能断开，将 `ACC_DATA_ERR` / `GYRO_DATA_ERR` 置为 `true`。

## 四、 线程按需启用与休眠机制
**调试目标**：结合 ThreadX 操作系统，实现合理的线程调度与休眠。

1. **线程创建**：在 `threads/` 目录下设计了独立的 IMU 线程。
2. **休眠与同步**：
   - 在任务主循环中调用 `tx_thread_sleep(1)` 或固定周期进行休眠，避免 CPU 空转。

## 五、 总结与遗留问题
- 能够在无硬件条件下通过编译。
-PID逻辑还没补全