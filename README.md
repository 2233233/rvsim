[TOC]

# RVSIM

## 简介

一个小型的RISC-V行为级模拟器、仅支持RV32IMF指令集，留下了RV32A指令集的接口和部分实现。最初的设想是可以运行[RVOS](https://github.com/plctlab/riscv-operating-system-mooc)的模拟器，已经通过了全部12个课程开发的实例的测试，关于RVOS课程，可以看[这里](https://www.bilibili.com/video/BV1Q5411w7z5/)。

## 项目结构

```
rvsim
│              
├─include	主要是pthread的头文件
│      pthread.h
│      sched.h
│      semaphore.h
│      
├─lib		pthread的库文件
│      libpthreadGC2.a
│      libpthreadGCE2.a
│      pthreadVC2.lib
│      pthreadVCE2.lib
│      pthreadVSE2.lib
│      
└─src
    │  avltree.c		avl树的实现，用于debug模式下管理断点
    │  bitmap.c			位图的实现，用于amo指令的保留位的实现
    │  clint.c			clint工作函数，负责定时中断和软中断
    │  debug.c			debug相关函数
    │  declare.c		声明的全局变量
    │  host.c			键盘线程和uart0交互相关
    │  init_instr.c		初始化指令映射和构建指令执行函数的参数的函数的映射
    │  instr.c			指令执行函数
    │  main.c			hart的主要工作函数
    │  plic.c			plic工作函数，负责硬件中断的仲裁
    │  trap.c			trap识别函数
    │  
    └─header
            avltree.h
            bitmap.h
            debug.h
            instr.h
            plic.h
            riscv.h
            trap.h
```

## 使用

`rvvm [-d] <image-file...>`

如果你正在学习RVOS课程，此处加载的镜像文件是编译后产生的/out目录下的os.bin文件。你可以选择在课程推荐环境（Ubuntu）下进行编译，也可以使用[芯来工具链](https://www.nucleisys.com/download.php)搭配make在windows环境下进行编译

debug部分：

- `b|breakpoint address`：设置断点，address是16进制的内存地址
- `run`：执行
- `reg [csr|plic|clint]`：默认打印通用寄存器，添加参数表示打印对应寄存器
- `mem address`：打印内存，address是16进制的内存地址
- `setp`：单步执行

## literate program

见[build your own risc-v emulator]([rvvm (2233233.github.io)](https://2233233.github.io/rvvm/))

