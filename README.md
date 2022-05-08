# Nets
nets是一个非阻塞，事件驱动的多线程网络库，遵守Reactor 网络模型。

## 编程规范
- 目录名：全小写，不使用驼峰，如果需要在单词之间区分请使用下划线；
- 文件名：大驼峰；
- 函数名：小驼峰；
- 变量名：小驼峰加末尾下划线；
- 静态变量：大驼峰；
- 宏定义：全大写，单词之间加下划线；
- gtest：test_suite_name和test_name大驼峰。

## 宏参数
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
- LOG_FILE_ROLLING_SIZE：日志文件回滚大小，单位:MB，默认24M。（仅当LOG_WRITER为ROLLING_FILE时有效）

## TODO
- [ ] 优化日志Log IO，虽然现在采用的日志IO也是后台异步写的方式，但即使没有日志写入，异步线程也是每隔1秒flush一次，而每次flush都需要与前台交换Buffer，总体来说内存消耗上来说是比较大的，所以后续会改为任务队列的模型。
- [ ] 优化代码结构，尽量遵循设计模式，降低代码之间的耦合性。