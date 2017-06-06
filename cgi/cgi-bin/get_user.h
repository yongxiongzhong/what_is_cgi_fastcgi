#!/bin/sh
# CGI程序shell版 

# 定义查询通过id查询用户信息函数
# 用数组代替数据库查询
function getUserById {
    users=(
        '{"user_id"=>1, "user_name"=>"zhangsan"}'
        '{"user_id"=>2, "user_name"=>"lisi"}'
    )

    echo $users[$1]
}

# 按照CGI协议获取QUERY_STRING里面的请求参数信息
strArr="$QUERY_STRING"
id=$(echo $strArr | cut -d = -f 2)
echo ""
getUserById $id
