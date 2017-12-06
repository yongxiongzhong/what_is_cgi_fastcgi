# 从CGI到FastCGI到PHP-FPM
## 背景
笔者在学习这几个名词的时候，也是被百度到的相关文章迷惑。涉及到的主要名词包括
```
1. CGI协议
2. CGI脚本
3. PHP-CGI
4. FastCGI协议
5. PHP-FPM
```
要真正理解这些名词，如果我们硬生生的解释，也很难记住。我们可以从web服务器开发的历程来看，看看他们为什么会出现，以及他们解决了什么问题。
## 早些年的简单服务器
早些年的服务器很简单，你访问一个网站，比如`www.helloworld.com`，网站只返回你一个静态HTML页面。为了简单起见，我们假设网站值返回hello标题，这样流程可以用下图表示

![image](http://note.youdao.com/yws/api/personal/file/WEBaaafeea8c61053dade30fa1194db0c41?method=download&shareKey=f77b29344e7e9cb49e44797d9daa476a)

这时候server是采用C语言编写的，基本上一个简的C脚本就可以了，把这个服务器脚本命名为`hello_server.c`，代码可以到github上[下载](https://github.com/yongxiongzhong/what_is_cgi_fastcgi/blob/master/simple/hello_server.c)

在Linux环境中使用gcc编译`hello_server.c`
```
vagrant@kfz:~$ gcc hello_world.c
```
然后采用`curl`工具测试
```
vagrant@kfz:~$ curl "127.0.0.1:8887"
GET / HTTP/1.1
User-Agent: curl/7.49.1
Host: 127.0.0.1:8887
Accept: */*

Hello World

```
这里我们的重点不在HTTP响应的格式上，所以我们直接输出了`Hello World`，如果是请求静态页面，我们也是一样的思路，读取静态文件的内容然后返还给客户端。

思考：当Web时代越来越火爆，我们希望Web服务器有更多的功能，比如写博客，聊天，等等。同时，越来越多的不同领域的开发者想在web时代大显身手。很多服务器开发者发现有了以下缺点
```
#服务器功能方面
  Web服务器功能会随着这种逻辑的增长而增长，服务器会变的不专一

#语言支持方便
  越来越多的PHP，Python开发者想开发服务端程序，编写更多的功能，发现自己的语言无从下手
```
## CGI协议与CGI程序
### 什么是CGI协议和CGI程序
既然Web服务器想做的专一，但又要支持Web的飞速发展，同时还想让其他语言开发者也能加入Web开发。所以，就出了一种方式，Web服务器和其他领域的开发者拟定一条协议，在保证遵守协议的基础上，剩下的可以自由发挥。没错，这条协议就是`CGI协议`，协议的内容可以[查看](https://www.ietf.org/rfc/rfc3875)，而实现这个协议的脚本叫做`CGI程序`。CGI协议规定了需要向CGI脚本设置的环境变量和一些其他信息，CGI程序完成某一个功能，可以用PHP，Python，Shell或者C语言编写。

这样的好处Web开发更加开放了，可以用任何开发者擅长的语言开发Web程序。比如，我是一个PHP开发者，用PHP有很多现成的库访问第三方服务，入MySQL，Memcache等，但是这些第三方服务使用C语言接入还是比较麻烦的。有了CGI协议，我们的Web处理流程可以变成下图这样

![image](http://note.youdao.com/yws/api/personal/file/WEB6930b9d20e1c7361de390efa7e0b04d9?method=download&shareKey=ebd561eb88641be368153c6c34daa849)

### 实现一个PHP版的CGI程序
一般情况下，我们把CGI程序放在cgi-bin目录下面，新建PHP版CGI处理程序`cgi-bin/get_user.php`，我们的服务器程序还是使用简单的`hello_server.c`。

**目标**：用户访问URL`127.0.0.1:8887/cgi-bin/get_user.php?id=1`的的时候输出用户ID为1的用户信息

**实现**：很显然，我们要解决两个问题
```
1. 在服务器程序hello_server.c中执行get_user.php文件
2. 服务器程序hello_server.c向get_user.php中传递请求信息(id=1)
```
第一个问题我们使用C库还是`popen`来运行php脚本，第二个问题在CGI协议中规定了我们的解决方案。首先需要将请求参数放在环境变量中，其次，环境变量的名字也规定好了。这里列举一些CGI协议规定的常用的环境变量,这里只列举一些，全部的内容可以协议明细

|变量名|描述|
|--|--|
|CONTENT_TYPE|这个环境变量的值指示所传递来的信息的MIME类型。目前，环境变量CONTENT_TYPE一般都是：application/x-www-form-urlencoded,他表示数据来自于HTML表单|
|HTTP_USER_AGENT|提供包含了版本数或其他专有数据的客户浏览器信息|
|QUERY_STRING|如果服务器与CGI程序信息的传递方式是GET，这个环境变量的值即使所传递的信息。这个信息经跟在CGI程序名的后面，两者中间用一个问号'?'分隔|
|REQUEST_METHOD|提供脚本被调用的方法。对于使用 HTTP/1.0 协议的脚本，仅 GET 和 POST 有意义|

所以，关于服务端怎么向CGI程序传递请求信息的问题就可以解释了。在服务端接收到用户请求的时候需要按照CGI协议通过`putenv`设置环境变量，然后PHP程序也是按照CGI协议解析环境变量(PHP中使用$_SERVER即可)。

程序的实现，可以在这里[下载](https://github.com/yongxiongzhong/what_is_cgi_fastcgi/tree/master/cgi)

最后需要说明的，当服务器执行`php cgi-bin/get_user.php`的时候，实际上是使用php的命令行模式执行的，因为PHP已经为我们封装好了环境变量，所以我们只需要从`$_SERVER`中就可以获取我们需要的环境变量。这里只为了说明问题，实际上PHP的sapi里面有单独实现cgi接口。当然你也可以用shell实现，shell的话是直接获取环境变量

### CGI程序的缺点
CGI工作原理：每当客户请求CGI的时候，WEB服务器就请求操作系统生成一个新的CGI解释器进程(如php-cgi)，CGI进程则处理完一个请求后退出，下一个请求来时再创建新进程。我们知道，执行一个PHP程序的必须要先解析php.ini文件，然后模块初始化等等一系列工作，每次都反复这样非常浪费资源。当然，这样在访问量很少没有并发的情况也行。可是当访问量增大，并发存在，这种方式就不适合了。这也是CGI程序一个致命的缺点。

## FastCGI协议和php-cgi
### FastCGI协议
既然上面提到了CGI程序的一个致命缺陷，所以在CGI协议的基础上，又设计了`FASTCGI协议`
，协议的内容可以[查看](http://andylin02.iteye.com/blog/648412)，概括一下，主要做了以下改变
```
1. FastCGI被设计用来支持常驻（long-lived）应用进程，减少了fork-and-execute带来的开销
2. FastCGI进程通过监听的socket，收来自Web服务器的连接，这样FastCGI进程可以独立部署
3. 服务器和FastCGI监听的socket之间按照消息的形式发送环境变量和其他数据
```
所以，我们称实现了`FastCGI协议`的程序为`FastCGI程序`，在php中的`sapi/cgi/php-cgi`实现了这个协议，虽然它的名字取名为php-cgi，但其实是支持FacsCGI协议的。所以，实现了FastCGI协议的服务器和FastCGI程序的交互方式如下图所示

![image](http://note.youdao.com/yws/api/personal/file/WEB683ab0ce6daa0f9103971b05b3f4f5a4?method=download&shareKey=439b6e0fc69fd7f15815c6bce0960a9a)

### PHP中FastCGI实现
可能很少人用过php-cgi，因为目前大家都使用php-fpm（先不讨论php-fpm）。这里介绍一下php-cgi的使用。

1.php-cgi如何启动监听socket

php-cgi可执行程序为编译后的php目录下面的`sapi/cgi/php-cgi`。通过`-b`参数指定需要监听的ip和端口即可，比如我们想绑定本机的9004端口
```
shell> ./php-cgi -b 127.0.0.1:9004

```

2.如何启动多个FastCGI进程

这里需要使用`PHP_FCGI_CHILDREN`环境变量表示派生多少个子进程，我们可以写一个shell来启动php-cgi，点击可以[下载](https://github.com/yongxiongzhong/what_is_cgi_fastcgi/blob/master/fastcgi/start.sh)这个shell文件。注意替换成自己的cgi路径后再使用

3.通过shell脚本启动FastCGI管理器

执行`start.sh`脚本之后，可以查看到启动了多个FastCGI进程

```
shell> ps aux  | grep php-cgi | grep -v grep
vagrant   6785  0.0  0.0   1936   524 pts/3    S    01:07   0:00 sh -c /home/vagrant/php-5.5.10/sapi/cgi/php-cgi -b 127.0.0.1:9004
vagrant   6792  0.0  0.3  13760  3188 ?        Ss   01:07   0:00 /home/vagrant/php-5.5.10/sapi/cgi/php-cgi -b 127.0.0.1:9004
vagrant   6793  0.0  0.1  13760  1384 ?        S    01:07   0:00 /home/vagrant/php-5.5.10/sapi/cgi/php-cgi -b 127.0.0.1:9004
vagrant   6794  0.0  0.1  13760  1384 ?        S    01:07   0:00 /home/vagrant/php-5.5.10/sapi/cgi/php-cgi -b 127.0.0.1:9004
vagrant   6795  0.0  0.1  13760  1384 ?        S    01:07   0:00 /home/vagrant/php-5.5.10/sapi/cgi/php-cgi -b 127.0.0.1:9004
vagrant   6796  0.0  0.1  13760  1384 ?        S    01:07   0:00 /home/vagrant/php-5.5.10/sapi/cgi/php-cgi -b 127.0.0.1:9004
vagrant   6797  0.0  0.1  13760  1384 ?        S    01:07   0:00 /home/vagrant/php-5.5.10/sapi/cgi/php-cgi -b 127.0.0.1:9004
vagrant   6798  0.0  0.1  13760  1384 ?        S    01:07   0:00 /home/vagrant/php-5.5.10/sapi/cgi/php-cgi -b 127.0.0.1:9004
vagrant   6799  0.0  0.1  13760  1384 ?        S    01:07   0:00 /home/vagrant/php-5.5.10/sapi/cgi/php-cgi -b 127.0.0.1:9004
vagrant   6800  0.0  0.1  13760  1384 ?        S    01:07   0:00 /home/vagrant/php-5.5.10/sapi/cgi/php-cgi -b 127.0.0.1:9004
```

4.和Nginx配合测试

在Nginx配置文件中加上这一段

```
location ~ \.php$ {
    fastcgi_pass   127.0.0.1:9004;
    include        fastcgi.conf;
}
```
重启nginx之后就可以测试nginx访问php了。

### php-cgi的的缺点
php-cgi实现的已经很完美了，但是我们的需求总是有点苛刻，导致它暴露了一下问题
1. 当我们更改配置文件后，php-cgi显然无法平滑重启
2. 我们fork的进程个数和请求量正比，请求繁忙时，fork进程多，动态调整，先让php-cgi没做到

## spawn-fcgi和PHP-FPM
既然上面提及php-cgi实现的FastCGI问题官方没有解决。那自然有第三方做这个事情，我们可以使用Lighttpd的spawn-fcgi来管理php-cgi进程。注意，这里的spawn-fcgi是进程管理器，只是管理实现FastCGI协议的php-cgi，这个的安装和使用方式可以参照鸟哥的[博客](http://www.laruence.com/2009/07/28/1030.html/comment-page-1#comment-217419)

还有一个做的比较好的，php-fpm，以前php-fpm并没有纳入php安装包文件中，只是在它的卓越表现，得到了越来越多的人认可，后面就被纳入了官方默认安装包文件中。php-fpm是从5.3开始才进入PHP源代码主干的，可以独立运行，不依赖php-cgi，换句话说，他自己实现了FastCGI协议。

所以，spawn-fcgi和PHP-FPM不是同一个东西，前者是进程管理器，后者却实现FastCGI协议并且支持进程平滑重启

## GIT目录说明
simple - 最简单服务器例子

cgi - PHP/Shell版CGI程序实现

fastcgi - 多进程方式启动php-cgi脚本
