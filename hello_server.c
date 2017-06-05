#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
int main()
{
    int server_sockfd = socket(AF_INET,SOCK_STREAM, IPPROTO_TCP);
    
    if(server_sockfd == -1){
        printf("socket error");
        return -1;
    }
    
    struct sockaddr_in server_sockaddr;/*声明一个变量，类型为协议地址类型*/
    server_sockaddr.sin_family = AF_INET;/*使用IPv4协议*/
    server_sockaddr.sin_port = htons(8887);/*监听8887端口*/
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
    
    char res[] = "Hello World\n";//定义要响应给客户端的字符串
    char req[1024] = {'\0'}; //存放客户端发送的内容，如果是浏览器访问，就是HTTP协议
    if(read(clnt_sock, req, 1024) != 0){
        write(clnt_sock, req, sizeof(req));//如果是浏览器访问，返回给客户端HTTP协议内容
        write(clnt_sock, res, sizeof(res));//返回给浏览器Hello World\n
    }
    
    //关闭连接
    close(clnt_sock);
    close(server_sockfd);
}
