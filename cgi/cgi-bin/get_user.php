<?php
// CGI程序PHP版

$query = $_SERVER['QUERY_STRING'];//获取QUERY_STRING环境变量
$strArr = explode("=", $query);//切分出id的值

$id = $strArr['1'];

$res = getUserById($id);
echo json_encode($res);

/**
 * 通过id获取用户信息，这里直接用一个数组代替
 * 实际环境中应该是访问数据库获取
 */
function getUserById($id){
    $users = array(
   	'1' => array('user_id'=>1, 'user_name'=>'zhangsan'),
	'2' => array('user_id'=>2, 'user_name'=>'lisi')
    );

    if(isset($users[$id])){
    	return $users[$id];
    }

    return array();
}
