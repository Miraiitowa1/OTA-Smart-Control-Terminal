## FreeRTOS

### FreeRTOS的核⼼思想是将系统划分为多个独⽴的任务，每个任务都有⾃⼰的代码和堆栈空间，可以独⽴运⾏。通过任务管理器，可以创建、删除、挂起、恢复和切换任务。任务的调度是由FreeRTOS内核完成的，它使⽤优先级和时间⽚轮转等调度算法，确保⾼优先级任务得到及时执⾏。除了任务管理和调度，FreeRTOS还提供了内存管理功能，可以动态分配和释放内存，以满⾜任务的需求。此外，FreeRTOS还⽀持信号量、消息队列和互斥量等同步机制，⽤于任务间的通信和资源共享。FreeRTOS 是⽤ C 和汇编来写的，其中绝⼤部分都是⽤ C 语⾔编写的，只有极少数的与处理器密切相关的部分代码才是⽤汇编写的,FreeRTOS 结构简洁，可读性很强

### 1. 任务管理Task
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
---

### 2. 互斥量Mutex
#### 互斥量⼜称互斥信号量（本质是信号量），是⼀种特殊的⼆值信号量，它和信号量不同的是，它⽀持互斥量所有权、递归访问以及防⽌优先级翻转的特性，⽤于实现对临界资源的独占式处理。任意时刻互斥量的状态只有两种，开锁或闭锁。当互斥量被任务持有时，该互斥量处于闭锁状态，这个任务获得互斥量的所有权。当该任务释放这个互斥量时，该互斥量处于开锁状态，任务失去该互斥量的所有权。如果想要⽤于实现同步（任务之间或者任务与中断之间），⼆值信号量或许是更好的选择，虽然互斥量也可以⽤于任务与任务、任务与中断的同步，但是互斥量更多的是⽤于保护资源的互锁

#### 互斥量的创建
**osMutexCreate**：创建⼀个互斥量，并返回⼀个互斥量ID
```c
osMutexId osMutexCreate (const osMutexDef_t *mutex_def)
```
- mutex_def： 引⽤由osMutexDef定义的互斥量

**osRecursiveMutexCreate**: 于创建⼀个递归互斥量，不是递归的互斥量由函数 osMutexCreate() 创建，且只能被同⼀个任务获取⼀次，如果同⼀个任务想再次获取则会失败。递归信号量则相反，它可以被同⼀个任务获取很多次，获取多少次就需要释放多少次。递归信号量与互斥量⼀样，都实现了优先级继承机制，可以减少优先级反转的反⽣
```c
osMutexId osRecursiveMutexCreate (const osMutexDef_t *mutex_def)
```

#### 删除互斥量
**osMutexDelete**: 删除⼀个互斥量
```c
osStatus osMutexDelete (osMutexId mutex_id)
```

#### 获取互斥量
**osMutexWait**: 获取互斥量，但是递归互斥量并不能使⽤这个 API 函数获取
```c
osStatus osMutexWait (osMutexId mutex_id, uint32_t millisec)
```
- millisec：等待信号量可⽤的最⼤超时时间，单位为 tick（即系统节拍周期）。如果宏 INCLUDE_vTaskSuspend 定义为 1 且形参xTicksToWait 设置为 portMAX_DELAY ，则任务将⼀直阻塞在该信号量上（即没有超时时间）

**osRecursiveMutexWait**: 获取递归互斥量的宏，与互斥量的获取函数⼀样，osMutexWait()也是⼀个宏定义，它最终使⽤现有的队列机制，实际执⾏的函数是xQueueTakeMutexRecursive() 。 获取递归互斥量之前必须由 osRecursiveMutexCreate() 这个函数创建。要注意的是该函数不能⽤于获取由函数 osMutexCreate() 创建的互斥量
```c
osStatus osRecursiveMutexWait (osMutexId mutex_id, uint32_t millisec)
```

#### 释放互斥量
**osMutexRelease**: 释放互斥量，但不能释放由函数 osRecursiveMutexCreate() 创建的递归互斥量
```c
osStatus osMutexRelease (osMutexId mutex_id)
```

**osRecursiveMutexRelease**: 释放⼀个递归互斥量。已经获取递归互斥量的任务可以重复获取该递归互斥量。使⽤ osRecursiveMutexWait() 函数成功获取⼏次递归互斥量，就要使⽤ osRecursiveMutexRelease() 函数返还⼏次，在此之前递归互斥量都处于⽆效状态，别的任务就⽆法获取该递归互斥量。使⽤该函数接⼝时，只有已持有互斥量所有权的任务才能释放它，每释放⼀该递归互斥量，它的计数值就减 1。当该互斥量的计数值为 0 时（即持有任务已经释放所有的持有操作），互斥量则变为开锁状态，等待在该互斥量上的任务将被唤醒。如果任务的优先级被互斥量的优先级翻转机制临时提升，那么当互斥量被释放后，任务的优先级将恢复为原本设定的优先级
```c
osStatus osRecursiveMutexRelease (osMutexId mutex_id)
```
---

### 3. 消息队列Queue
#### FreeRTOS的消息队列是⼀种⽤于任务间通信的同步机制。它提供了⼀种简单、可靠的⽅式，让任务之间可以传递数据和消息，以实现资源共享和协同⼯作。消息队列的基本原理是，⼀个任务可以将⼀个消息发送到消息队列中，⽽另⼀个任务则可以从消息队列中接收该消息。发送和接收消息的任务可以是同⼀个任务，也可以是不同的任务。当消息队列为空时，接收任务会被挂起，直到有新的消息到达为⽌。消息队列还⽀持阻塞模式和超时模式。在阻塞模式下，如果队列已满或为空，发送或接收任务会被挂起，直到队列有⾜够的空间或有新的消息到达为⽌。在超时模式下，如果队列已满或为空，发送或接收任务会等待⼀段时间，如果超时仍未有⾜够的空间或新的消息到达，则函数会返回错误

#### 消息队列创建与删除
**osMessageQId**: 队列ID
```c
/// Message ID identifies the message queue (pointer to a message queue control block).
/// \note CAN BE CHANGED: \b os_messageQ_cb is implementation specific in every CMSIS-RTOS.
typedef QueueHandle_t osMessageQId;
```

**osMessageCreate**: 使⽤动态内存的⽅式创建⼀个新的队列
```c
osMessageQId osMessageCreate (const osMessageQDef_t *queue_def, osThreadId thread_id)
```
- queue_def： 引⽤由osMessageQDef定义的队列

**osMessageDelete**: 队列删除函数是根据消息队列ID直接删除的，删除之后这个消息队列的所有信息都会被系统回收清空，不能再次使⽤这个消息队列了
```c
osStatus osMessageDelete (osMessageQId queue_id)
```

#### 消息发送与接收
**osMessagePut**: ⽤于向队列尾部发送⼀个队列消息。消息以拷⻉的形式⼊队，⽽不是以引⽤的形式。可⽤在中断服务程序中
```c
osStatus osMessagePut (osMessageQId queue_id, uint32_t info, uint32_t millisec)
```
- info： 要发送的数据， ⼤⼩4字节

**osMessageGet**: ⽤于从⼀个队列中接收消息并把消息从队列中删除。接收的消息是以拷⻉的形式进⾏的，所以我们必须提供⼀个⾜够⼤空间的缓冲区。具体能够拷⻉多少数据到缓冲区，这个在队列创建的时候已经设定。可⽤在中断服务程序中
```c
osEvent osMessageGet (osMessageQId queue_id, uint32_t millisec)
```

**osMessagePeek**: osMessagePeek() 也是从从队列中接收数据单元，不同的是并不从队列中删出接收到的单元。osMessagePeek() 从队列⾸接收到数据后，不会修改队列中的数据，也不会改变数据在队列中的存储序顺。可⽤在中断服务程序中
```c
osEvent osMessagePeek (osMessageQId queue_id, uint32_t millisec)
```

#### 查询消息个数
**osMessageWaiting**: ⽤于查询队列中当前有效数据单元个数
```c
uint32_t osMessageWaiting(osMessageQId queue_id)
```
---

### 4. 信号量Semaphore
#### 信号量是⼀种⽤于多任务协调和同步的机制。它可以⽤来管理共享资源的访问，避免多个任务同时访问同⼀个资源⽽导致的冲突和竞争条件。FreeRTOS中的信号量是⼀个计数器，可以被多个任务共享。它有两种类型：⼆进制信号量和计数信号量。⼆进制信号量只有两个状态：0和1。它可以⽤来实现互斥访问，即只允许⼀个任务访问共享资源。当⼀个任务获取到⼆进制信号量后，其他任务就⽆法获取该信号量，直到该任务释放信号量为⽌。计数信号量可以有多个状态，取决于初始化时的计数值。它可以⽤来实现资源的共享访问，即允许多个任务同时访问共享资源，但有⼀定的限制。当计数信号量的值为0时，任务需要等待，直到有其他任务释放信号量为⽌。当计数信号量的值⼤于0时，任务可以获取信号量并继续执⾏

#### 信号量创建与释放
**osSemaphoreCreate**: 创建⼀个信号量，并返回⼀个ID
```c
sSemaphoreId osSemaphoreCreate (const osSemaphoreDef_t *semaphore_def, int32_t count)
```
- count：信号量数量

**osSemaphoreDelete**: 删除⼀个信号量，包括⼆值信号量，计数信号量，互斥量和递归互斥量。如果有任务阻塞在该信号量上，那么不要删除该信号量
```c
osStatus osSemaphoreDelete (osSemaphoreId semaphore_id)
```

**osSemaphoreRelease**: ⽤于释放信号量的宏。释放的信号量对象必须是已经被创建的，可以⽤于⼆值信号量、计数信号量、互斥量的释放，但不能释放由函数xSemaphoreCreateRecursiveMutex() 创建的递归互斥量。可⽤在中断服务程序中。
对于⼆值信号量就是就是加1操作，计数信号量也是加1操作
```c
osStatus osSemaphoreRelease (osSemaphoreId semaphore_id)
```

#### 获取信号量
**osSemaphoreWait**: ⽤于获取信号量，不带中断保护。获取的信号量对象可以是⼆值信号量、计数信号量和互斥量，但是递归互斥量并不能使⽤这个 API 函数获取。可⽤在中断服务程序中。对于⼆值信号量就是就是减1操作，计数信号量也是减1操作
```c
int32_t osSemaphoreWait (osSemaphoreId semaphore_id, uint32_t millisec)
```

#### ⼆值信号量
##### 创建信号量时，系统会为创建的信号量对象分配内存，把可⽤信号量初始化为⽤⼾⾃定义的个数， ⼆值信号量最⼤可⽤信号量个数为 1。⼆值信号量获取，任何任务都可以从创建的⼆值信号量资源中获取⼀个⼆值信号量，获取成功则返回正确，否则任务会根据⽤⼾指定的阻塞超时时间来等待其它任务/中断释放信号量。在等待这段时间，系统将任务变成阻塞态，任务将被挂到该信号量的阻塞等待列表中。假如某个时间中断/任务释放了信号量，那么，由于获取⽆效信号量⽽进⼊阻塞态的任务将获得信号量并且恢复为就绪态状态

#### 计数信号量
##### 信号量数量自定义

---

### 5. 软件定时器Timer