#!/bin/bash 
 
## php-cgi 的文件路径
PHPFCGI='/home/vagrant/php-5.5.10/sapi/cgi/php-cgi'
 
## PID 文件路径
PHP_PID="/tmp/php.pid"
 
## 绑定 TCP 地址
FCGI_BIND_ADDRESS="127.0.0.1:9008"
 
## 绑定到 Unix domain socket
#FCGI_BIND_ADDRESS="/tmp/php.sock"
 
## 派生出多少个 PHP 子进程
## 其中不包括主进程
PHP_FCGI_CHILDREN=8
 
## 每个 PHP 进程处理的最大请求数
PHP_FCGI_MAX_REQUESTS=4096
 
## 用户
USERID=verdana
 
################## no config below this line
 
# 根据用户不同，切换启动命令
if test x$UID = x0; then
  CMD="/bin/su -m -c \"$PHPFCGI -q -b $FCGI_BIND_ADDRESS\" $USERID"
else
  CMD="$PHPFCGI -b $FCGI_BIND_ADDRESS"
fi
 
echo $CMD
 
# 相关的环境变量
E="PHP_FCGI_CHILDREN=$PHP_FCGI_CHILDREN PHP_FCGI_MAX_REQUESTS=$PHP_FCGI_MAX_REQUESTS"
 
# 忽略其他的环境变量，以一个干净的环境启动
nohup env - $E sh -c "$CMD" &> /dev/null &
 
# 记录 PHP 主进程的 PID
# $! 返回的是 sh 的 PID
# 找到所有 php-cgi 进程中最小的 PID，就是主进程的 PID
MASTER_PID=`ps -e | grep 'php-cgi' | sed -n '1p' | awk '{print $1}'`
echo $MASTER_PID > "$PHP_PID"
