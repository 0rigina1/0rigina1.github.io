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
gdb有以下几种调试方式等
## 3.1、调试方式
### 3.1.1、启动调试
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

### 3.2.1、附加调试
```bash
# 查询进程pid
ps -ef | grep process_name

# gdb附加
gdb -p PID
```

## 3.2、调试

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
8
9           return a + b;
10      }
11
12
13      int main() {
14          int c = 0;
15          c = sum(3, 9);
16
17          printf("sum = %d", c);
```

使用`step`或`next`来逐步执行代码。
- `step`或简写`s`进入函数
- `next`或简写`n`执行下一行。
- `continue`或简写`c`继续程序执行直到下一个断点或程序结束。
- `print variable_name`获取变量当前值

单步执行一行，此时将0赋值给c。
```bash
(gdb) next
14          int c = 0;
```

获取变量c的值，c此时为0
```bash
(gdb) print c
$1 = 0
```

step单步进入sum函数，
```bash
(gdb) step
15          c = sum(3, 9);
(gdb) step
sum (a=21845, b=1431654848) at test.c:4
4       int sum(int a, int b) {
(gdb) step
5           a += 1;
```

此时a传参为3，此行第五行执行`a += 1`，单步next后，打印a的值为4。
```bash
(gdb) s
sum (a=21845, b=1431654848) at test.c:4
4       int sum(int a, int b) {
(gdb) s
5           a += 1;
(gdb) n
6           a += 2;
(gdb) print a
$1 = 4
```

修改变量值
```bash
(gdb) set var a = 100
(gdb) print a
$2 = 100
```

结果
```bash
(gdb) n
__libc_start_main (main=0x55555555516f <main>, argc=1, argv=0x7fffffffd168, init=<optimized out>, fini=<optimized out>, rtld_fini=<optimized out>, stack_end=0x7fffffffd158) at ../csu/libc-start.c:342
342     ../csu/libc-start.c: No such file or directory.
(gdb) n
sum = 120
```
