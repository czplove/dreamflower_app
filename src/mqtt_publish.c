/*
 * MQTT publish
 *
 * Author: JoStudio
 */
/*
主程序分为三个步骤：

1， 调用 mqtt_new()创建 客户端对象

2， 调用 mqtt_connect() 连接服务器

3， 调用 mqtt_publish() 发布消息

*/


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "mqtt/mqtt_client.h"//这个是我对mqtt_client封装后的头文件


int mqtt_publish_sub(int argc, char ** argv) {

	mqtt_client *m; //mqtt_client 对象指针
	int ret; //返回值
	char *host = "messagesight.demos.ibm.com:1883";//测试服务器
	char *topic = "test_topic"; //主题
	char *client_id = "clientid33883";//客户端ID； 对测试服务器，可以随便写
	char *username = NULL;//用户名，用于验证身份。对测试服务器，无。
	char *password = NULL;//密码，用于验证身份。对测试服务器，无。
	int Qos; //Quality of Service

	//create new mqtt client object
	m = mqtt_new(host, MQTT_PORT, client_id); //创建对象，MQTT_PORT = 1883
	if ( m == NULL ) {
		printf("mqtt client create failure, return  code = %d\n", errno);
		return 1;
	} else {
		printf("mqtt client created\n");
	}

	//connect to server
	ret = mqtt_connect(m, username, password); //连接服务器
	if (ret != MQTT_SUCCESS ) {
		printf("mqtt client connect failure, return code = %d\n", ret);
		return 1;
	} else {
		printf("mqtt client connect\n");
	}

	//publish message
	Qos = QOS_EXACTLY_ONCE; //Qos
	ret = mqtt_publish(m, topic, "hello from Linkit 7688", Qos);//发布消息
	printf("mqtt client publish,  return code = %d\n", ret);

	mqtt_disconnect(m); //disconnect
	mqtt_delete(m);  //delete mqtt client object
	return 0;
}



