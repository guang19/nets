# Nets
nets是一个基于Linux平台的多线程C++网络库

## 编程规范
- 目录名：全小写，不使用驼峰，如果需要在单词之间区分请使用下划线；
- 文件名：大驼峰；
- 函数名：小驼峰；
- 变量名：小驼峰加末尾下划线；
- 静态变量：大驼峰；
- 宏定义：全大写，单词之间加下划线；
- gtest：test_suite_name和test_name大驼峰。

## 宏参数
- LOG_LEVEL：日志等级，默认DEBUG。可选：TRACE,DEBUG,INFO,WARN,ERROR,FATAL；
- LOG_WRITER_TYPE：日志输出目标，默认STDOUT。可选：STDOUT,SINGLE_FILE，DAILY_FILE，ROLLING_FILE；
  - STDOUT：标准输出；
  - SINGLE_FILE：输出单文件，不回滚，不计大小与时间；
  - DAILY_FILE：每日生成一个文件，不计大小；
  - ROLLING_FILE：按照文件大小回滚。
- LOG_FILE：日志文件，默认为：“/tmp/nets/nets.log”，请使用绝对路径；
- LOG_FILE_ROLLING_SIZE：日志文件回滚大小，单位:MB，默认24M。（仅当LOG_WRITER为ROLLING_FILE时有效）