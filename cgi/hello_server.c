#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>

char *str_join(char *str1, char *str2);

int main()
{
    int server_sockfd = socket(AF_INET,SOCK_STREAM, IPPROTO_TCP);
    
    if(server_sockfd == -1){
        printf("socket error");
        return -1;
    }
    
    struct sockaddr_in server_sockaddr;/*声明一个变量，类型为协议地址类型*/
    server_sockaddr.sin_family = AF_INET;/*使用IPv4协议*/
    server_sockaddr.sin_port = htons(8887);/*监听8887端口，一般80端口被占用了，所以用这个*/
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);/*绑定本机IP，使用宏定义绑定*/
    
    //绑定端口
    if(bind(server_sockfd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr))==-1){
        printf("bind error");
        return -1;
    }
    
    //监听端口
    if(listen(server_sockfd, 20) == -1){
        printf("listen error");
        return -1;
    }
    
    //接收客户端请求
    struct sockaddr_in clnt_addr;/*只是声明，并没有赋值*/
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    int clnt_sock = accept(server_sockfd, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
    
    if(clnt_sock == -1){
        printf("appect error");
        return -1;
    }
    
    char res[1024] = {'\0'}; //定义要响应给客户端的字符串
    char req[1024] = {'\0'}; //存放客户端发送的内容，如果是浏览器访问，就是HTTP协议
    if(read(clnt_sock, req, 1024) != 0){
        char *delim = " ";
        char *p;
        char *method, *filename, *query_string;
        /**
         * 通过strtok函数切分HTTP协议内容
         * HTTP协议第一行第一个参数为 请求方式 请求路径 协议版本
         */
        method = strtok(req, delim);         // GET
        p = strtok(NULL, delim);             // /cgi-bin/get_user.php?id=1 
        filename = strtok(p, "?");           // /cgi-bin/get_user
        query_string = strtok(NULL, "?");    // id=1

        /**
         * 按照CGI协议内容设置环境变量，名字不能乱起，否则CGI程序无法解析
         * 请求方式环境变量名称为REQUEST_METHOD，请求字符串环境变量的名称为QUERY_STRING
         */
        putenv(str_join("REQUEST_METHOD=", method));//REQUEST_METHOD环境变量
        putenv(str_join("QUERY_STRING=", query_string));//QUERY_STRING环境变量

        //通过popen执行php脚本并且读取返回内容
        FILE *stream = popen(str_join("php .", filename), "r");
        fread(res, sizeof(char), sizeof(res), stream);

        write(clnt_sock, res, sizeof(res));//将内容返回到浏览器
    }
    
    //关闭连接
    close(clnt_sock);
    close(server_sockfd);
}

//拼接字符串函数
char *str_join(char *str1, char *str2) {
    char *result = malloc(strlen(str1) + strlen(str2) + 1);
    if (result == NULL){
        exit(1);
    }
    strcpy(result, str1);
    strcat(result, str2);
    return result;
}
