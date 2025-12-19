---
title: 为任意 PHP 版本构建兼容的 xdebug 调试环境
date: 2025-12-12 06:32:20
tags:
- PHP
- 调试
- 代码审计
- Xdebug
---
# 1、概述
> 在代码审计过程中，往往需要借助调试手段来进一步验证。本文针对离线PHP项目，介绍使用 Docker 构建任意版本的 Xdebug 实现兼容任意版本的 PHP。
# 2、前提要点
整体步骤如下：
1、通过 Docker 来安装特定版本的 PHP
2、PHP 安装后，通过 phpize 编译 xdebug.so
3、将编译后的 xdebug.so 放入离线环境
4、配置 php.ini、配置 PhpStorm
5、开始调试

注意：
* 选择的 xdebug 版本要兼容离线环境的 PHP 版本
  * 如离线环境的 PHP 版本为 5.3.x，选择的 xdebug 版本要兼容该版本。
* 构建 Docker 镜像时，基础镜像的 glibc 版本要与离线环境 glibc 版本一致
  * 如离线环境是 CentOS 6，glibc 版本为 2.12，构建时要使用 CentOS 6，这样才能保证 glibc 版本一致
# 3、确定离线环境信息
## 3.1、查看项目 PHP 版本
运行以下命令查看当前 PHP 版本：
```bash
php -v
```
输出如下，记录版本信息（如PHP 5.3.3）。
```bash
PHP 5.3.3 (cli) (built: Dec 12 2025 08:14:16) 
Copyright (c) 1997-2010 The PHP Group
Zend Engine v2.3.0, Copyright (c) 1998-2010 Zend Technologies
```
## 3.2、查看离线环境 glibc 版本
运行以下命令查看当前 glibc 版本
```bash
ldd --version
```
输出如下，记录版本信息（如 (GNU libc) 2.12）。
```bash
ldd (GNU libc) 2.12
Copyright (C) 2010 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
Written by Roland McGrath and Ulrich Drepper.
```
到这里已经确定了版本信息：
* PHP 5.3.3
* glibc 2.12
## 3.3、确定版本
PHP 各版本源码下载
[https://museum.php.net](https://museum.php.net/php5/php-5.3.3.tar.gz)
Xdebug 各版本源码下载
https://pecl.php.net/package/xdebug
根据 Xdebug 官方[兼容性说明](http://xdebug.org.cn/docs/compat.php)，兼容 PHP 5.3.3 的 xdebug 版本为 2.2
![对照版本](1.png)
* PHP 5.3.3
  * https://museum.php.net/php5/php-5.3.3.tar.gz
* Xdebug 2.2.x
  * https://pecl.php.net/package/xdebug/2.2.5
# 4、构建镜像与 Xdebug
由于 glibc 是跟随 Linux 发行版本固定的，所以在选择 Docker 基础镜像时，要注意匹配该版本。
根据 GPT 得知，glibc 2.12 应该采用的 Docker 基础镜像为 `centos:6.10`
在确定 Docker 基础镜像、PHP 版本、glibc 版本后，得到的 Dockerfile如下
## 4.1、编写 Dockerfile
```dockerfile
FROM centos:6.10

# 修复 YUM 源（使用 vault）
RUN sed -i 's|mirrorlist=|#mirrorlist=|g' /etc/yum.repos.d/CentOS-*.repo && \
    sed -i 's|#baseurl=|baseurl=|g' /etc/yum.repos.d/CentOS-*.repo && \
    sed -i 's|http://mirror.centos.org/centos/$releasever|http://vault.centos.org/6.10|g' /etc/yum.repos.d/CentOS-*.repo

RUN yum clean all && yum makecache

# 构建依赖
RUN yum install -y wget gcc make autoconf automake libtool \
    libxml2-devel openssl-devel libcurl-devel libjpeg-devel libpng-devel

WORKDIR /tmp

# ---- 安装 PHP 5.3.3 ----
RUN wget http://museum.php.net/php5/php-5.3.3.tar.gz && \
    tar -xzf php-5.3.3.tar.gz && \
    cd php-5.3.3 && \
    ./configure --prefix=/usr/local/php-5.3.3 \
      --enable-cli --disable-all --disable-dom --disable-libxml --disable-simplexml && \
    make -j$(nproc) && \
    make install && \
    cp php.ini-development /usr/local/php-5.3.3/lib/php.ini

# ---- 安装 Xdebug 2.2.5 ----
RUN wget https://pecl.php.net/get/xdebug-2.2.5.tgz && \
    tar -xzf xdebug-2.2.5.tgz && \
    cd xdebug-2.2.5 && \
    /usr/local/php-5.3.3/bin/phpize && \
    ./configure --with-php-config=/usr/local/php-5.3.3/bin/php-config --enable-xdebug && \
    make -j$(nproc) && \
    make install

CMD ["/bin/bash"]

```
## 4.2、构建镜像
执行以下命令，构建 Docker 镜像，在构建过程中，会安装 PHP、编译 Xdebug
```bash
docker build -t docker build -t glibc2.12-php5.3.3-xdebug2.2.5 .
```
构建完成，执行 `docker images` 结果如下
```bash
REPOSITORY                       TAG       IMAGE ID       CREATED       SIZE
glibc2.12-php5.3.3-xdebug2.2.5   latest    9f12fb6022b2   2 hours ago   849MB
```
## 4.3、创建容器
之后，执行以下命令创建容器，将容器命名为 `test`
```bash
docker run -itd --name test glibc2.12-php5.3.3-xdebug2.2.5 bash
```
容器启动后，执行以下命令进入容器 Shell
```bash
docker exec -it test bash
```
进入容器，定位 xdebug.so
```bash
find / -name xdebug.so
```
输出如下，这三个 md5 一致，哪个都可以
```bash
/tmp/xdebug-2.2.5/.libs/xdebug.so
/tmp/xdebug-2.2.5/modules/xdebug.so
/usr/local/php-5.3.3/lib/php/extensions/no-debug-non-zts-20090626/xdebug.so
```
## 4.4、拷贝目标产物 xdebug.so
从容器中拷贝 xdebug.so 到当前目录得到 xdebug.so
```bash
docker cp test:/tmp/xdebug-2.2.5/.libs/xdebug.so .
```
# 5、调试配置
## 5.1、离线环境配置
将 xdebug.so 放到离线环境中的任意位置，如 `/etc/xdebug.so` 
编辑 php.ini，将以下内容加到尾部（此处是xdebug2.x写法，xdebug3.x 写法不同需要修改）
```ini
[xdebug]
; xdebug.so 文件位置
zend_extension=/etc/xdebug.so

; 开启远程调试
xdebug.remote_enable=1

; 如网络环境如下
; Windows（192.168.10.1）上运行PhpStorm，打算用PhpStorm调试 PHP 代码，
; Windows 上通过 VMWare 运行虚拟机，该虚拟机是离线环境（192.168.10.2）
; 则此处填写 Windows IP
xdebug.remote_host=192.168.146.1

; 调试端口，后续设置 PhpStorm 调试时，保持一致
xdebug.remote_port=9000

; 让 Xdebug 在每次请求开始时自动启动调试会话
xdebug.remote_autostart=1

; 调试日志文件
xdebug.remote_log=/tmp/xdebug.log

; 可以随便改，后续设置 PhpStorm 调试时，保持一致
xdebug.idekey=PHPSTORM
```
## 5.2、PhpStorm配置
在设置中配置调试端口
![PhpStorm](2.png)
