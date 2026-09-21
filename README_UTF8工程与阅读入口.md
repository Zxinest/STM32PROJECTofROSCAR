# L150 LD14 UTF-8 工程入口

这是 L150/C10B、STM32F103RC、LD14 雷达小车源码的可维护 UTF-8 副本。原始资料没有被修改。

- [编码转换报告](<编码转换报告.md>)
- [标准库 Keil 工程](<L150-避障巡线雷达小车-C10B-库函数-2024061/Project/RVMDK（uv5）/MiniBalance.uvprojx>)
- [HAL Keil 工程](<L150-避障巡线雷达小车-C10B-HAL库-20250507/MDK-ARM/miniBlance.uvprojx>)

## 建议先读哪一套

先读标准库工程：

`L150-避障巡线雷达小车-C10B-库函数-2024061`

原因不是它一定比 HAL 更好，而是它的底盘调用关系集中在 `User` 目录，适合先建立整车控制概念。理解之后，再对照 HAL 工程学习 `HAL_GPIO`、`HAL_TIM`、CubeMX 和回调函数。

## 先建立这条主线

```text
main()
  ├─ 初始化 GPIO、编码器、电机 PWM、串口、ADC、MPU6050、舵机
  └─ 启动 5 ms 定时器
       ↓
TIMING_TIM_IRQHandler()            5 ms 实时控制周期
       ↓
Get_Velocity_From_Encoder()        读取左右编码器并换算实际轮速
       ↓
Get_Target_Encoder()               把目标车速/转角换算为左右轮目标速度和舵机 PWM
       ↓
Get_Motor_PWM()
       ↓
Incremental_PI_Left/Right()        左右轮速度闭环
       ↓
Set_Pwm()                          输出左右电机方向/PWM和舵机PWM
```

蓝牙、ROS、手柄、遥控器和雷达负责改变 `Move_X`、`Move_Z`、`Mode` 等上层目标；最终都进入同一条电机控制主线。

## 对应的真实源码

| 阅读顺序 | 文件和函数 | 现在只需要看懂什么 |
|---:|---|---|
| 1 | [`User/main.c`](<L150-避障巡线雷达小车-C10B-库函数-2024061/User/main.c>)：`main()` | 上电后初始化了什么；为什么 `while(1)` 主要负责显示和低速任务 |
| 2 | [`User/CONTROL/control.c`](<L150-避障巡线雷达小车-C10B-库函数-2024061/User/CONTROL/control.c>)：`TIMING_TIM_IRQHandler()` | 真正的 5 ms 底盘控制循环在哪里 |
| 3 | 同一文件：`Get_Velocity_From_Encoder()` | 编码器计数怎样变成 m/s |
| 4 | 同一文件：`Get_Target_Encoder()` | 阿克曼转角怎样变成舵机 PWM 和左右轮目标速度 |
| 5 | [`User/CONTROL/pid.c`](<L150-避障巡线雷达小车-C10B-库函数-2024061/User/CONTROL/pid.c>)：`Incremental_PI_Left/Right()` | “目标速度 − 实际速度”怎样产生电机 PWM |
| 6 | [`User/HARDWARE/Motor/bsp_motor.c`](<L150-避障巡线雷达小车-C10B-库函数-2024061/User/HARDWARE/Motor/bsp_motor.c>)：`Set_Pwm()` | 正负 PWM 怎样控制方向；舵机比较值怎样写入定时器 |
| 7 | [`User/HARDWARE/encoder/encoder.c`](<L150-避障巡线雷达小车-C10B-库函数-2024061/User/HARDWARE/encoder/encoder.c>) | STM32 定时器编码器模式和计数器读取 |
| 8 | [`User/HARDWARE/usartx/usartx.c`](<L150-避障巡线雷达小车-C10B-库函数-2024061/User/HARDWARE/usartx/usartx.c>) | ROS 串口帧怎样产生 `Move_X` 和 `Move_Z` |
| 9 | [`User/CONTROL/bluetooth.c`](<L150-避障巡线雷达小车-C10B-库函数-2024061/User/CONTROL/bluetooth.c>)、[`Lidar.c`](<L150-避障巡线雷达小车-C10B-库函数-2024061/User/CONTROL/Lidar.c>) | 上层控制怎样改变目标，而不是直接驱动电机 |

## 第一条关键理解

`main()` 不是一直亲自控制电机。

它先完成初始化，然后启动一个每 5 ms 触发一次的定时中断。真正要求时间稳定的“读取编码器 → 计算速度 → PI → 输出 PWM”放在这个中断里。主循环 `while(1)` 则主要处理 OLED 显示、按键和电压等不需要每 5 ms 执行的工作。

第一次遇到的几个 C/STM32 术语：

- `void Encoder_Init(void)`：这是一个不接收参数、也不返回结果的 C 函数；在小车里负责配置两个编码器定时器。
- `MotorA.Current_Encoder`：`.` 表示访问结构体成员；这里是左轮当前测得速度。
- `TIM3->CCR1`：`->` 表示通过指针访问寄存器结构体成员；这里最终会改变定时器 PWM 比较值。
- `volatile`：告诉编译器这个变量可能被中断随时改变，不要把它当成永远不变的数据。
- 中断：硬件定时器到点后暂停普通代码，立即执行一次控制函数，再返回原位置。

## 推荐的学习节奏

每次只解决一个具体问题，例如：

1. `main()` 每个初始化函数分别对应哪块硬件？
2. 5 ms 周期是怎样由定时器参数算出来的？
3. 编码器一周期的计数为什么能换算成 m/s？
4. 阿克曼车为什么要同时改变舵机角度和左右轮目标速度？
5. 增量 PI 中 `Kp`、`Ki` 分别改变什么现象？
6. ROS 串口的一帧数据怎样走到 `Set_Pwm()`？

不要一开始深入 CMSIS 或完整 STM32 标准库。先沿上面的真实调用链走通；遇到某个 GPIO、TIM、USART 或 C 语法时，再补刚好够用的背景知识。

## 后续提问方式

可以直接指定文件、函数或现象，例如：

- “从 `main()` 第一行开始带我读，但一次只讲 20 行。”
- “解释 `Get_Velocity_From_Encoder()`，同时复习里面用到的 C 语法。”
- “画出 ROS 串口命令到左右电机 PWM 的变量传递过程。”
- “我认为 `MotorA.Target_Encoder` 是编码器计数，对不对？”

后续讲解应优先检查这份 UTF-8 副本中的实际代码；如果代码与经验不一致，以代码为准并明确指出。

