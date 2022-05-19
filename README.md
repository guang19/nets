# Nets
nets是一个基于Linux的非阻塞，事件驱动的多线程网络库，遵守Reactor网络模型。

## 代码规范
- 目录命名：全小写，不使用驼峰，如果需要在单词之间区分请使用下划线；
- 文件命名：大驼峰；
- 函数命名：小驼峰，如果函数是用于获取POD的字段，那么需要加get前缀，即getXXX；
- 变量命名：小驼峰加末尾下划线；
- 静态变量命名：大驼峰；
- 宏定义命名：全大写，单词之间加下划线；
- gtest：test_suite_name和test_name大驼峰。
- 头文件顺序：首先按文件相关性，比如测试文件必须最先引入gtest.h；.cpp文件的的第一个头文件必须是其对应的.h。其次，标准库应该先被引入，接下来是第三方库的头文件，最后是自己的库的头文件。所有的头文件按字母顺序排序。

## 宏参数
nets支持使用外部宏定义的方式调整一些关键的参数。

### 线程池配置：
- CORE_POOL_SIZE: 核心线程数，默认为可用CPU核心数的2倍，类型：size_t；
- MAX_POOL_SIZE: 最大线程数，默认为可用CPU核心数的2倍，类型：size_t；
- IDLE_KEEP_ALIVE_TIME：空闲线程可存活的最长时间，默认30000，单位：ms，类型：time_t；
- TASK_QUEUE_SIZE: 任务队列大小，默认24，类型：size_t。

### 日志配置
- LOG_LEVEL：日志等级，默认0（DEBUG）。可选：
  - 0：TRACE；
  - 1：DEBUG；
  - 2：INFO；
  - 3：WARN；
  - 4：ERROR；
  - 5：FATAL。
- LOG_WRITER_TYPE：日志输出目标，默认为0（STDOUT）。可选：
  - 0：STDOUT：标准输出；
  - 1：SINGLE_FILE：输出单文件，不回滚，不计大小与时间；
  - 2：DAILY_FILE：每日生成一个文件，不计大小；
  - 3：ROLLING_FILE：按照文件大小回滚。
- LOG_FILE：日志文件，默认为：“/tmp/nets/nets.log”，请使用绝对路径；
- LOG_FILE_ROLLING_SIZE：日志文件回滚大小，单位:MB，默认24M（仅当LOG_WRITER为ROLLING_FILE时有效）。

## TODO
- [x] ~~优化日志Log IO，虽然现在采用的日志IO也是后台异步写的方式，但即使没有日志写入，异步线程也是每隔1秒flush一次，而每次flush都需要与前台交换Buffer，总体来说内存消耗上来说是比较大的，所以后续会改为任务队列的模型。~~
- [ ] 因为一条日志一个Buffer，所以后续考虑将LogBuffer的内存分配从堆改为栈；
- [ ] 考虑增加基于io_uring的Proactor模型。