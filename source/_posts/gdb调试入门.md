---
title: gdb调试入门
date: 2024-10-21 21:24:55
tags: 逆向
---


# 1、gdb安装
```bash
apt install gdb
```

# 2、测试代码
```c
#include<stdio.h>


int sum(int a, int b) {
    a += 1;
    a += 2;
    a += b;

    return a + b;
}


int main() {
    int c = 0;
    c = sum(3, 9);

    printf("sum = %d", c);
}
```

## 2.1、gcc编译
`gcc`编译，将`test.c`代码编译成`test`可执行文件，注意需要使用`-g`选项，这会让编译器生成符号信息，帮助gdb等调试工具识别函数名、变量名、代码行号，这样调试过程就可以看到具体的执行信息。
```bash
gcc -g test.c -o test
```

> 注意：不添加`-g`编译，`gdb`调试时会提示
```bash
(No debugging symbols found in xxx)
```

编译后执行，查看输出
```bash
# 执行
./test

# 输出
@0rigina1 ➜ (main) $ ./test 
sum = 24
```

# 3、gdb调试
gdb有以下几种调试方式

## 3.1、启动调试
通过gdb直接启动
```bash
@0rigina1 ➜ (main) $ gdb ./test
GNU gdb (Ubuntu 9.2-0ubuntu1~20.04.2) 9.2
Copyright (C) 2020 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.
Type "show copying" and "show warranty" for details.
This GDB was configured as "x86_64-linux-gnu".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<http://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
    <http://www.gnu.org/software/gdb/documentation/>.

For help, type "help".
Type "apropos word" to search for commands related to "word"...
Reading symbols from ./test...
(gdb) 
```
此时就可以调试了。

在main函数上打一个断点，用法 `break function_name`。
如：
```bash
(gdb) break main
Breakpoint 1 at 0x116f: file test.c, line 13.
```
此时就在main函数上打下断点了。

查看所有断点`info breakpoints`。

![查看断点](image.png)

执行run开始运行程序，会停在main函数断点上。

![查看断点](image2.png)

执行`list`显示当前执行点附近的代码。
```bash
(gdb) list
1       #include<stdio.h>
2
3
4       int sum(int a, int b) {
5           a += 1;
6           a += 2;
7           a += b;
8
9           return a + b;
10      }
(gdb)
```



## 3.2、附加调试

## 3.3、核心转储调试
