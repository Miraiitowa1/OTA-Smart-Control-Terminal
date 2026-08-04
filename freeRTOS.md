## FreeRTOS

### FreeRTOS的核⼼思想是将系统划分为多个独⽴的任务，每个任务都有⾃⼰的代码和堆栈空间，可以独⽴运⾏。通过任务管理器，可以创建、删除、挂起、恢复和切换任务。任务的调度是由FreeRTOS内核完成的，它使⽤优先级和时间⽚轮转等调度算法，确保⾼优先级任务得到及时执⾏。除了任务管理和调度，FreeRTOS还提供了内存管理功能，可以动态分配和释放内存，以满⾜任务的需求。此外，FreeRTOS还⽀持信号量、消息队列和互斥量等同步机制，⽤于任务间的通信和资源共享。FreeRTOS 是⽤ C 和汇编来写的，其中绝⼤部分都是⽤ C 语⾔编写的，只有极少数的与处理器密切相关的部分代码才是⽤汇编写的,FreeRTOS 结构简洁，可读性很强

### 1. 任务管理task
#### 在FreeRTOS中，任务是系统的基本执⾏单位。每个任务都有⾃⼰的代码和堆栈空间，可以独⽴运⾏。通过任务管理器，可以创建、删除、挂起、恢复和切换任务 任务的调度是由FreeRTOS内核完成的。它使⽤优先级和时间⽚轮转等调度算法，确保⾼优先级任务优先执⾏，并且每个任务都有公平的执⾏机会

#### 任务创建与启动 
**osThreadld**: 任务id
```c
TaskHandle_t osThreadId;
```
**osThreadCreate**: 使⽤动态/静态内存的⽅法创建⼀个任务
```c
osThreadId osThreadCreate (const osThreadDef_t *thread_def, void *argument)
```
**osThreadTerminate**: 删除任务 任务被删除后就不复存在，也不会再进⼊运⾏态
```c
osStatus osThreadTerminate (osThreadId thread_id)
```
**osKernelStart**: 删除任务 任务被删除后就不复存在，也不会再进⼊运⾏态
- 在创建完任务的时候，我们需要开启调度器，因为创建仅仅是把任务添加到系统中，还没真正调度，并且空闲任务也没实现，定时器任务也没实现，这些都是在开启调度函数 osKernelStart() 中实现的。
- 为什么要空闲任务？因为 FreeRTOS ⼀旦启动，就必须要保证系统中每时每刻都有⼀个任务处于运⾏态（Runing），并且空闲任务不可以被挂起与删除，空闲任务的优先级是最低的，以便系统中其他任务能随时抢占空闲任务的 CPU 使⽤权
```c
osStatus osKernelStart (void)
```

#### 任务延时
**osDelay**: 相对延时函数。⽤于阻塞延时，调⽤该函数后，任务将进⼊阻塞状态，进⼊阻塞态的任务将让出 CPU 资源
```c
osStatus osDelay (uint32_t millisec)
```
**osDelayUntil**: 绝对延时函数。常⽤于较精确的周期运⾏任务，⽐如我有⼀个任务，希望它以固定频率定期执⾏，⽽不受外部的影响，任务从上⼀次运⾏开始到下⼀次运⾏开始的时间间隔是绝对的，⽽不是相对的
```c
osStatus osDelayUntil (uint32_t *PreviousWakeTime, uint32_t millisec)
```

#### 任务挂起与恢复
**osThreadSuspend**: 挂起指定任务。被挂起的任务绝不会得到 CPU 的使⽤权，不管该任务具有什么优先级
```c
osStatus osThreadSuspend (osThreadId thread_id)
```
**osThreadSuspendAll**: 将所有的任务都挂起
```c
osStatus osThreadSuspendAll (void)
```
**osThreadResume**: 让挂起的任务重新进⼊就绪状态，恢复的任务会保留挂起前的状态信息，在恢复的时候根据挂起时的状态继续运⾏。如果被恢复任务在所有就绪态任务中，处于最⾼优先级列表的第⼀位，那么系统将进⾏任务上下⽂的切换。可⽤在中断服务程序中
```c
BaseType_t osThreadResume( TaskHandle_t xTaskToResume )
```
**osThreadResumeAll**: 将所有的任务都恢复
```c
osStatus osThreadResumeAll (void)
```

#### 获取任务状态
**osThreadGetState**: 获取任务当前状态
```c
osThreadState osThreadGetState(osThreadId thread_id)
```
- 返回值
```c
/* Thread state returned by osThreadGetState */
typedef enum {
    osThreadRunning = 0x0, /* A thread is querying the state of itself, so must be running. */
    osThreadReady = 0x1 , /* The thread being queried is in a read or pending ready list. */
    osThreadBlocked = 0x2, /* The thread being queried is in the Blocked state. */
    osThreadSuspended = 0x3, /* The thread being queried is in the Suspended state, or is in the
    Blocked state with an infinite time out. */
    osThreadDeleted = 0x4, /* The thread being queried has been deleted, but its TCB has not yet
    been freed. */
    osThreadError = 0x7FFFFFFF
} osThreadState;
```